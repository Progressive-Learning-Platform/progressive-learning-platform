/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.gui;

import plptool.*;

import org.jdesktop.application.SingleFrameApplication;

import org.apache.commons.compress.archivers.tar.TarArchiveOutputStream;
import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;

import java.util.Scanner;
import java.util.ArrayList;

import javax.swing.tree.*;

/**
 * This is the PLPTool application project backend.
 *
 * @author wira
 */
public class PLPBackend {

    public SingleFrameApplication              app;        // App

    public String                              plpfile;    // current PLP file
    public boolean                             modified;
    public int                                 open_asm;   // current open ASM
    public int                                 main_asm;   // main program

    public plptool.PLPCfg                      cfg;        // Configuration
    public plptool.PLPMsg                      msg;        // Messaging class

    public ArrayList<plptool.PLPAsmSource>     asms;       // Assembly files
    public String                              meta;       // Meta String

    public plptool.PLPAsm                      asm;        // Assembler
    public plptool.PLPAsm[]                    asm_array;  // Asm array
    public plptool.PLPLinker                   lnkr;       // Linker
    public plptool.PLPSerialProgrammer         prg;        // Programmer

    public plptool.PLPSimCore                  sim;        // Simulation core
    public plptool.mods.IORegistry             ioreg;      // I/O registry

    // GUI Members
    public plptool.PLPSimCoreGUI               g_sim;      // Sim Core GUI

    // PLP GUI Windows
    public plptool.gui.PLPSimShell             g_simsh;    // PLP Simulator Frontend
    public plptool.gui.PLPIORegistry           g_ioreg;    // I/O registry GUI
    public plptool.gui.PLPDevelop              g_dev;      // IDE GUI
    public plptool.gui.PLPErrorFrame           g_err;      // Error frame
    public plptool.gui.PLPToolAboutBox         g_about;    // About frame
    public plptool.gui.PLPOptions              g_opts;     // Options frame
    private boolean                            g;          // are we driving a GUI?

    // Desktop
    public javax.swing.JDesktopPane            g_desktop;  // Desktop pane

    // Architecture
    private String                             arch;

    public PLPBackend(boolean g, String arch) {
        this.g = g;
        this.arch = arch;

        modified = false;
        plpfile = null;
    }

    public int setArch(String arch) {
        this.arch = arch;

        return Constants.PLP_OK;
    }

    public int newPLPFile() {
        modified = true;
        plpfile = "Unsaved Project";

        asms = new ArrayList<plptool.PLPAsmSource>();
        asms.add(new plptool.PLPAsmSource("# main source file", "main.asm", 0));
        open_asm = 0;
        main_asm = 0;

        meta =  "PLP-2.1\n";
        meta += "START=0x0\n";
        meta += "DIRTY=1\n\n";
        meta += "MAINSRC=0";

        PLPMsg.I("New project initialized.", null);

        if(g) {
            refreshProjectView(false);
            g_simsh.destroySimulation();
            g_simsh.setVisible(false);
            g_dev.disableSimControls();
        }

        return Constants.PLP_OK;
    }

    public int savePLPFile() {

        // commit changes of currently open source file
        if(g) updateAsm(open_asm, g_dev.getEditorText());
        assemble();

        if(plpfile == null || plpfile.equals("Unsaved Project"))
            return PLPMsg.E("No PLP project file is open. Use Save As.",
                            Constants.PLP_FILE_SAVE_ERROR, null);

        ArrayList<PLPAsmSource> sourceList;
        String verilogHex = "";
        long[] objCode = null;
        PLPAsmSource temp;
        int i;

        try {

        File outFile = new File(plpfile);

        meta = "PLP-2.1\n";

        if(asm != null && asm.isAssembled()) {
            objCode = asm.getObjectCode();
            verilogHex = plptool.mips.Formatter.writeVerilogHex(objCode);
            if(objCode.length > 0)
                meta += "START=" + asm.getAddrTable()[0] + "\n";
            else
                meta += "START=0\n";
            meta += "DIRTY=0\n";
        }
        else {
            meta += "DIRTY=1\n";
        }

        meta += "MAINSRC=" + main_asm + "\n";
        meta += "\n";

        sourceList = asms;

        for(i = 0; i < sourceList.size(); i++) {
            temp = (PLPAsmSource) sourceList.get(i);
            meta += temp.getAsmFilePath() + "\n";
        }

        // Create plpfile (a tar archive)
        TarArchiveOutputStream tOut = new TarArchiveOutputStream(new FileOutputStream(outFile));

        TarArchiveEntry entry = new TarArchiveEntry("plp.metafile");
        entry.setSize(meta.length());
        tOut.putArchiveEntry(entry);
        byte[] data = meta.getBytes();
        tOut.write(data);
        tOut.flush();
        tOut.closeArchiveEntry();

        for(i = 0; i < sourceList.size(); i++) {
            PLPAsmSource asmFile = sourceList.get(i);
            entry = new TarArchiveEntry(asmFile.getAsmFilePath());
            entry.setSize(asmFile.getAsmString().length());
            tOut.putArchiveEntry(entry);

            // We are not expecting an .asm file with size greater than 4GiB
            // ... I hope...
            byte[] fileStr = asmFile.getAsmString().getBytes();
            tOut.write(fileStr);
            tOut.flush();
            tOut.closeArchiveEntry();
        }

        if(asm != null && asm.isAssembled() && objCode != null) {
            // Write hex image
            entry = new TarArchiveEntry("plp.hex");
            entry.setSize(verilogHex.length());
            tOut.putArchiveEntry(entry);
            data = new byte[verilogHex.length()];
            for(i = 0; i < verilogHex.length(); i++) {
                data[i] = (byte) verilogHex.charAt(i);
            }
            tOut.write(data);
            tOut.flush();
            tOut.closeArchiveEntry();

            // Write binary image, 4-byte big-endian packs
            entry = new TarArchiveEntry("plp.image");
            entry.setSize(objCode.length * 4);
            tOut.putArchiveEntry(entry);
            data = new byte[objCode.length * 4];
            for(i = 0; i < objCode.length; i++) {
                data[4*i] = (byte) (objCode[i] >> 24);
                data[4*i+1] = (byte) (objCode[i] >> 16);
                data[4*i+2] = (byte) (objCode[i] >> 8);
                data[4*i+3] = (byte) (objCode[i]);
            }
            tOut.write(data);
            tOut.flush();
            tOut.closeArchiveEntry();
        }

        tOut.close();

        modified = false;
        if(g) refreshProjectView(false);
        PLPMsg.I(plpfile + " written", null);

        } catch(Exception e) {
            if(Constants.debugLevel >= 10) e.printStackTrace();
            return PLPMsg.E("savePLPFile: Unable to write to " + plpfile + "\n" +
                     e, Constants.PLP_FILE_SAVE_ERROR, this);
        }


        return Constants.PLP_OK;
    }

    public int openPLPFile(String path) {
        File plpFile = new File(path);
        boolean dirty = true;

        PLPMsg.I("Opening " + path, null);

        if(!plpFile.exists())
            return PLPMsg.E("openPLPFile(" + path + "): File not found.",
                            Constants.PLP_BACKEND_PLP_OPEN_ERROR, null);

        asms = new ArrayList<plptool.PLPAsmSource>();

        try {

        TarArchiveInputStream tIn = new TarArchiveInputStream(new FileInputStream(plpFile));
        TarArchiveEntry entry;
        byte[] image;
        String metaStr;
        int asmIndex = 0;

        while((entry = tIn.getNextTarEntry()) != null) {
            image = new byte[(int) entry.getSize()];
            tIn.read(image, 0, (int) entry.getSize());
            metaStr = new String(image);

            if(entry.getName().endsWith("asm")) {
                asms.add(new plptool.PLPAsmSource(metaStr, entry.getName(), asmIndex));
                PLPMsg.I(" - ASM file [" + asmIndex + "]: " + entry.getName(), null);
                asmIndex++;
            }

            if(entry.getName().equals("plp.metafile")) {
                meta = metaStr;

                Scanner metaScanner = new Scanner(meta);
                metaScanner.findWithinHorizon("MAINSRC=", 0);
                main_asm = metaScanner.nextInt();
                metaScanner = new Scanner(meta);
                metaScanner.findWithinHorizon("DIRTY=", 0);
                if(metaScanner.nextInt() == 0)
                    dirty = false;
                
            }
        }
        
        if(asmIndex == 0) {
            return PLPMsg.E("openPLPFile(" + path + "): no .asm files found.",
                            Constants.PLP_BACKEND_INVALID_PLP_FILE, null);
        }

        }
        catch(Exception e) {
            e.printStackTrace();
            return PLPMsg.E("openPLPFile(" + path + "): Invalid PLP archive.",
                            Constants.PLP_BACKEND_INVALID_PLP_FILE, null);
        }

        plpfile = path;
        modified = false;
        open_asm = main_asm;

        if(g) {
            refreshProjectView(false);
            g_simsh.destroySimulation();
            g_simsh.setVisible(false);
            g_dev.disableSimControls();
            if(!dirty)
                assemble();
            modified = false;
        }

        return Constants.PLP_OK;
    }

    public int updateWindowTitle() {
        File fHandler = new File(plpfile);
        String windowTitle = fHandler.getName() + ((modified) ? "*" : "") +
                             " - PLP Software Tool " + Constants.versionString;
        g_dev.setTitle(windowTitle);

        return Constants.PLP_OK;
    }

    public int refreshProjectView(boolean commitCurrentAsm) {
        if(commitCurrentAsm)
            updateAsm(open_asm, g_dev.getEditorText());

        updateWindowTitle();

        DefaultMutableTreeNode root = new DefaultMutableTreeNode(plpfile);
        DefaultMutableTreeNode srcRoot = new DefaultMutableTreeNode("Source Files");
        DefaultMutableTreeNode metaRoot = new DefaultMutableTreeNode("Meta Information");
        root.add(srcRoot);
        root.add(metaRoot);
        for(int i = 0; i < asms.size(); i++)
            srcRoot.add(new DefaultMutableTreeNode(i + "::" + asms.get(i).getAsmFilePath()));

        Scanner metaScanner = new Scanner(meta);
        metaScanner.findWithinHorizon("DIRTY=", 0);
        int meta_dirty =  metaScanner.nextInt();
        metaRoot.add(new DefaultMutableTreeNode("meta.DIRTY=" + meta_dirty));
        metaRoot.add(new DefaultMutableTreeNode("meta.MAINSRC=" + main_asm));
        

        g_dev.getProjectTree().setModel(new DefaultTreeModel(root));
        for(int i = 0; i < g_dev.getProjectTree().getRowCount(); i++)
            g_dev.getProjectTree().expandRow(i);
        
        g_dev.setEditorText(asms.get(open_asm).getAsmString());
        g_dev.getEditor().setEnabled(true);
        g_dev.getEditor().setCaretPosition(0);
        g_dev.enableBuildControls();

        String header = asms.get(open_asm).getAsmFilePath();

        if(main_asm == open_asm)
            header += " <main program>";

        g_dev.setCurFile(header);

        return Constants.PLP_OK;
    }

    public int link() {

        return Constants.PLP_OK;
    }

    public int assemble() {

        PLPMsg.I("Assembling...", null);

        if(g) g_dev.disableSimControls();

        if(g) asms.get(open_asm).setAsmString(g_dev.getEditor().getText());

        if(asms == null || asms.isEmpty())
            return PLPMsg.E("assemble(): No source files are open.",
                            Constants.PLP_BACKEND_EMPTY_ASM_LIST, this);

        // ...assemble asm objects... //
        if(arch.equals("plpmips")) {
            asm = new plptool.mips.Asm(asms);

            if(asm.preprocess(main_asm) == Constants.PLP_OK)
                asm.assemble();
        }

        if(g && asm != null && asm.isAssembled()) {
            modified = true;
            g_dev.enableSimControls();
        }

        PLPMsg.I("Done.", null);

        return Constants.PLP_OK;
    }

    public int simulate() {
        PLPMsg.I("Starting simulation...", null);

        if(!asm.isAssembled())
            return PLPMsg.E("simulate(): The project is not assembled.",
                            Constants.PLP_BACKEND_NO_ASSEMBLED_OBJECT, this);

        if(arch.equals("plpmips")) {
            sim = (plptool.mips.SimCore) new plptool.mips.SimCore((plptool.mips.Asm) asm, -1);
            sim.reset();

            if(g) {
                g_simsh.destroySimulation();
                g_ioreg = new PLPIORegistry(this);

                g_sim = new plptool.mips.SimCoreGUI(this);
                g_simsh.getSimDesktop().add(g_sim);
                g_simsh.getSimDesktop().add(g_err);
                g_simsh.getSimDesktop().add(g_ioreg);
                g_sim.updateComponents();
                g_sim.setVisible(true);
                g_err.setVisible(true);

                g_simsh.setVisible(true);
            }
        }

        return Constants.PLP_OK;
    }

    public PLPAsmSource getAsm(int index) {
        if(asms == null || index < 0 || index >= asms.size()) {
            PLPMsg.E("updateAsm: Invalid index.",
                     Constants.PLP_BACKEND_BOUND_CHECK_FAILED, this);
            return null;
        }

        return asms.get(index);
    }

    public int updateAsm(int index, String newStr) {
        if(asms == null || index < 0 || index >= asms.size())
            return PLPMsg.E("updateAsm: Invalid index.",
                            Constants.PLP_BACKEND_BOUND_CHECK_FAILED, this);

        if(!asms.get(index).getAsmString().equals(newStr))
            modified = true;

        asms.get(index).setAsmString(newStr);

        return Constants.PLP_OK;
    }

    public int importAsm(String path) {
        File asmFile = new File(path);

        PLPMsg.I("Importing " + path, null);

        if(!asmFile.exists())
            return PLPMsg.E("importAsm(" + path + "): file not found.",
                            Constants.PLP_BACKEND_ASM_IMPORT_ERROR, this);

        String existingPath;
        for(int i = 0; i < asms.size(); i++) {
            existingPath = asms.get(i).getAsmFilePath();

            if(existingPath.equals(path) ||
               existingPath.equals(asmFile.getName())) {
                return PLPMsg.E("importAsm(" + path + "): File with the same name already exists.",
                                Constants.PLP_BACKEND_IMPORT_CONFLICT, this);
            }
        }

        asms.add(new PLPAsmSource(null, path, asms.size()));
        asms.get(asms.size() - 1).setAsmFilePath(asmFile.getName());

        modified = true;

        if(g) refreshProjectView(true);

        return Constants.PLP_OK;
    }

    public int exportAsm(int index, String path) {
        File asmFile = new File(path);

        PLPMsg.I("Exporting " + asms.get(index).getAsmFilePath() +
                 " to " + path, null);

        if(asms == null || index < 0 || index >= asms.size())
            return PLPMsg.E("exportAsm: Invalid index.",
                            Constants.PLP_BACKEND_BOUND_CHECK_FAILED, this);

        if(asmFile.exists()) {
            return PLPMsg.E("exportAsm: " + path + " exists.",
                            Constants.PLP_FILE_SAVE_ERROR, this);
        }

        try {

        FileWriter asmWriter = new FileWriter(asmFile);
        asmWriter.write(asms.get(index).getAsmString());
        asmWriter.close();

        } catch(Exception e) {
            if(Constants.debugLevel >= 10) e.printStackTrace();
            return PLPMsg.E("exportAsm(" + asms.get(index).getAsmFilePath() +
                            "): Unable to write to " + path + "\n",
                            Constants.PLP_FILE_SAVE_ERROR, this);
        }

        return Constants.PLP_OK;
    }

    public int removeAsm(int index) {
        if(asms.size() <= 1) {
            return  PLPMsg.E("removeAsm: Can not delete last source file.",
                            Constants.PLP_BACKEND_DELETING_LAST_ASM_ERROR, this);
        }

        if(asms == null || index < 0 || index >= asms.size())
            return  PLPMsg.E("removeAsm: Invalid index.",
                            Constants.PLP_BACKEND_BOUND_CHECK_FAILED, this);

        modified = true;
        if(index == main_asm)
            main_asm = 0;
        if(index < open_asm) {
            if(g) updateAsm(open_asm, g_dev.getEditorText());
            open_asm--;
        }
        else if(index == open_asm && open_asm != 0)
            open_asm--;
        else
            if(g) updateAsm(open_asm, g_dev.getEditorText());
            
        PLPMsg.I("Removing " + asms.get(index).getAsmFilePath(), null);
        asms.remove(index);
        if(g) refreshProjectView(false);

        return Constants.PLP_OK;
    }

    public int command(String command) {

        return Constants.PLP_OK;
    }

    public void updateComponents() {
        g_sim.updateComponents();
        g_dev.updateComponents();
        ioreg.gui_eval();
    }

    @Override
    public String toString() {
        return "PLPBackend(" + plpfile + ")";
    }
}
