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
    public boolean                             saved;
    public int                                 open_asm;   // current open ASM

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

        saved = false;
        plpfile = null;
    }

    public int setArch(String arch) {
        this.arch = arch;

        return Constants.PLP_OK;
    }

    public int savePLPFile(String path) {

        return Constants.PLP_OK;
    }

    public int newPLPFile() {
        saved = false;
        plpfile = "Unsaved Project";

        asms = new ArrayList<plptool.PLPAsmSource>();
        asms.add(new plptool.PLPAsmSource("# main source file", "main.asm", 0));
        open_asm = 0;

        if(g) {
            refreshProjectView();
            g_simsh.destroySimulation();
            g_simsh.setVisible(false);
        }

        return Constants.PLP_OK;
    }

    public int savePLPFile() {

        asms.get(open_asm).setAsmString(g_dev.getEditor().getText());

        if(plpfile == null || plpfile.equals("Unsaved Project"))
            return PLPMsg.E("No PLP project file open.",
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
            meta += "START=" + asm.getAddrTable()[0] + "\n";
            meta += "DIRTY=0\n\n";
        }
        else {
            meta += "DIRTY=1\n\n";
        }

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
        byte[] data = new byte[meta.length()];
        for(i = 0; i < meta.length(); i++) {
            data[i] = (byte) meta.charAt(i);
        }
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

        if(asm != null && asm.isAssembled()) {
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

        PLPMsg.I(plpfile + " written", null);

        } catch(Exception e) {
            return PLPMsg.E("genPLP(): Unable to write to " + plpfile + "\n" +
                     e, Constants.PLP_FILE_SAVE_ERROR, null);
        }


        return Constants.PLP_OK;
    }

    public int openPLPFile(String path) {
        File plpFile = new File(path);

        PLPMsg.I("Opening " + path, this);

        if(!plpFile.exists())
            return PLPMsg.E(path + " not found.",
                            Constants.PLP_FILE_OPEN_ERROR, this);

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
                PLPMsg.I(" - ASM file [" + asmIndex + "]: " + entry.getName(), this);
                asmIndex++;
            }

            if(entry.getName().equals("plp.metafile")) {
                meta = metaStr;
            }
        }
        
        if(asmIndex == 0) {
            return PLPMsg.E("Invalid PLP File: no .asm files found.",
                            Constants.PLP_FILE_OPEN_ERROR, this);
        }

        }
        catch(Exception e) {
            return PLPMsg.E("Invalid PLP archive: " + path,
                            Constants.PLP_FILE_OPEN_ERROR, this);
        }

        plpfile = path;
        saved = true;
        open_asm = 0;

        if(g) {
            refreshProjectView();
            g_simsh.destroySimulation();
            g_simsh.setVisible(false);
        }

   

        return Constants.PLP_OK;
    }

    public int refreshProjectView() {
        DefaultMutableTreeNode root = new DefaultMutableTreeNode(plpfile);
        DefaultMutableTreeNode srcRoot = new DefaultMutableTreeNode("Source Files");
        root.add(srcRoot);
        for(int i = 0; i < asms.size(); i++)
            srcRoot.add(new DefaultMutableTreeNode(i + "::" + asms.get(i).getAsmFilePath()));

        g_dev.getProjectTree().setModel(new DefaultTreeModel(root));
        g_dev.getProjectTree().expandRow(1);
        g_dev.getEditor().setText(asms.get(open_asm).getAsmString());
        g_dev.getEditor().setEnabled(true);
        g_dev.getEditor().setCaretPosition(0);
        g_dev.enableBuildControls();
        g_dev.setCurFile(asms.get(open_asm).getAsmFilePath());

        return Constants.PLP_OK;
    }

    public int link() {

        return Constants.PLP_OK;
    }

    public int assemble() {

        PLPMsg.I("Assembling...", this);
        asms.get(open_asm).setAsmString(g_dev.getEditor().getText());

        if(asms == null || asms.isEmpty())
            return PLPMsg.E("No source files are open.",
                            Constants.PLP_NO_ASM_OPEN, this);

        // ...assemble asm objects... //
        if(arch.equals("plpmips")) {
            int ret;
            asm = new plptool.mips.Asm(asms);

            if(asm.preprocess(0) == Constants.PLP_OK)
                asm.assemble();
        }

        PLPMsg.I("Done.", this);

        return Constants.PLP_OK;
    }

    public int simulate() {
        PLPMsg.I("Starting simulation...", this);

        if(!asm.isAssembled())
            return PLPMsg.E("The project is not assembled.",
                            Constants.PLP_ASM_NOT_ASSEMBLED, this);

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
        return "";
    }
}
