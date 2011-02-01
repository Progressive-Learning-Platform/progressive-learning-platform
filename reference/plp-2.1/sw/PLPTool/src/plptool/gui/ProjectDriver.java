/*
    Copyright 2010 David Fritz, Brian Gordon, Wira Mulia

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
public class ProjectDriver extends Thread {

    public SingleFrameApplication              app;        // App

    public String                              plpfile;    // current PLP file
    public boolean                             modified;
    public int                                 open_asm;   // current open ASM
    public int                                 main_asm;   // main program
    public String                              curdir;     // current workiing dir

    private String                             arch;       // architecture
    public byte[]                              binimage;   // binary image
    public String                              hexstring;  // hex string

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
    public plptool.gui.SimShell                g_simsh;    // PLP Simulator Frontend
    public plptool.gui.IORegistryFrame         g_ioreg;    // I/O registry GUI
    public plptool.gui.Develop                 g_dev;      // IDE GUI
    public plptool.gui.SimErrorFrame           g_err;      // Error frame
    public plptool.gui.AboutBoxDialog          g_about;    // About frame
    public plptool.gui.OptionsFrame            g_opts;     // Options frame
    public plptool.gui.ProgrammerDialog        g_prg;      // Programming dialog
    public plptool.gui.AsmNameDialog           g_fname;    // ASM Name dialog
    public plptool.gui.SimRunner               g_simrun;   // SimRunner thread
    private boolean                            g;          // are we driving a GUI?

    // Desktop
    public javax.swing.JDesktopPane            g_desktop;  // Desktop pane

    // Programmer
    public gnu.io.SerialPort                   p_port;
    public int                                 p_progress;
    public TimeoutWatcher                      p_watchdog;

    public ProjectDriver(boolean g, String arch) {
        this.g = g;
        this.arch = arch;

        modified = false;
        plpfile = null;
    }

    public int setArch(String arch) {
        this.arch = arch;

        return Constants.PLP_OK;
    }

    public String getArch() {
        return arch;
    }

    public int create() {
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

    public int create(String asmPath) {
        modified = true;
        plpfile = "Unsaved Project";

        asms = new ArrayList<plptool.PLPAsmSource>();
        if(importAsm(asmPath) != Constants.PLP_OK) {
            asms.add(new plptool.PLPAsmSource("# main source file", "main.asm", 0));
        }
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

    public int save() {

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
            PLPMsg.D("Creating verilog hex code...", 2, this);
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

        PLPMsg.D("Writing plp.metafile...", 2, this);
        TarArchiveEntry entry = new TarArchiveEntry("plp.metafile");
        entry.setSize(meta.length());
        tOut.putArchiveEntry(entry);
        byte[] data = meta.getBytes();
        tOut.write(data);
        tOut.flush();
        tOut.closeArchiveEntry();

        for(i = 0; i < sourceList.size(); i++) {
            PLPAsmSource asmFile = sourceList.get(i);
            PLPMsg.D("Writing " + asmFile.getAsmFilePath() + "...", 2, this);
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
            PLPMsg.D("Writing out verilog hex code...", 2, this);
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
            PLPMsg.D("Writing out binary image...", 2, this);
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
        } else if(binimage != null) {
            PLPMsg.D("Writing out old (dirty) verilog hex code...", 2, this);
            entry = new TarArchiveEntry("plp.hex");
            entry.setSize(hexstring.length());
            tOut.putArchiveEntry(entry);
            tOut.write(hexstring.getBytes());
            tOut.flush();
            tOut.closeArchiveEntry();
            
            PLPMsg.D("Writing out old (dirty) binary image...", 2, this);
            entry = new TarArchiveEntry("plp.image");
            entry.setSize(binimage.length);
            tOut.putArchiveEntry(entry);
            tOut.write(binimage);
            tOut.flush();
            tOut.closeArchiveEntry();
        }

        tOut.close();

        modified = false;
        if(g) refreshProjectView(false);
        PLPMsg.I(plpfile + " written", null);

        } catch(Exception e) {
            if(Constants.debugLevel >= 10) e.printStackTrace();
            return PLPMsg.E("save(): Unable to write to " + plpfile + "\n" +
                     e, Constants.PLP_FILE_SAVE_ERROR, this);
        }


        return Constants.PLP_OK;
    }

    public int open(String path) {
        File plpFile = new File(path);
        boolean dirty = true;

        PLPMsg.I("Opening " + path, null);

        if(!plpFile.exists())
            return PLPMsg.E("open(" + path + "): File not found.",
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
                PLPMsg.I(asmIndex + ": " 
                         + entry.getName()
                         + " (" + entry.getSize() + " bytes)", null);
                asmIndex++;
            }

            else if(entry.getName().equals("plp.metafile")) {
                meta = metaStr;
                Scanner metaScanner;

                String lines[] = meta.split("\\r?\\n");
                if(lines[0].equals("PLP-2.1"))  {
                    metaScanner = new Scanner(meta);
                    metaScanner.findWithinHorizon("MAINSRC=", 0);
                    main_asm = metaScanner.nextInt();
                } else {
                    PLPMsg.I("WARNING: This is not a PLP-2.1 project file. Opening anyways.", this);
                    main_asm = 0;
                }

                metaScanner = new Scanner(meta);
                metaScanner.findWithinHorizon("DIRTY=", 0);
                if(metaScanner.nextInt() == 0)
                    dirty = false;
            }

            else if(entry.getName().equals("plp.image")) {
                binimage = new byte[(int) entry.getSize()];
                binimage = image;
            }

            else if(entry.getName().equals("plp.hex")) {
                hexstring = new String(image);
            }
        }
        
        if(asmIndex == 0) {
            return PLPMsg.E("open(" + path + "): no .asm files found.",
                            Constants.PLP_BACKEND_INVALID_PLP_FILE, null);
        }

        }
        catch(Exception e) {
            e.printStackTrace();
            return PLPMsg.E("open(" + path + "): Invalid PLP archive.",
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
        PLPMsg.errorCounter = 0;

        boolean wasAssembled = false;

        if(asm != null)
            wasAssembled = asm.isAssembled();

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
            if(!wasAssembled)
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

        if(asm.getObjectCode().length == 0)
            return PLPMsg.E("simulate(): Empty program.",
                            Constants.PLP_BACKEND_EMPTY_PROGRAM, this);

        if(arch.equals("plpmips")) {
            sim = (plptool.mips.SimCore) new plptool.mips.SimCore((plptool.mips.Asm) asm, -1);
            sim.setStartAddr(asm.getAddrTable()[0]);
            sim.reset();

            if(g) {
                g_simsh.destroySimulation();
                g_ioreg = new IORegistryFrame(this);

                g_sim = new plptool.mips.SimCoreGUI(this);
                g_simsh.getSimDesktop().add(g_sim);
                g_simsh.getSimDesktop().add(g_err);
                g_simsh.getSimDesktop().add(g_ioreg);
                g_sim.updateComponents();
                g_sim.setVisible(true);
                g_err.setVisible(true);
                ioreg.loadPreset(0, this);

                g_simsh.setVisible(true);
            }
        }

        return Constants.PLP_OK;
    }

    public int program(String port) {
        PLPMsg.I("Programming to " + port, this);

        try {

        if(arch.equals("plpmips") && asm != null && asm.isAssembled()) {
            prg = new plptool.mips.SerialProgrammer(this);
            prg.connect(port, Constants.PLP_BAUDRATE);
            p_progress = 0;
            p_watchdog = new TimeoutWatcher(this);

            if(g) {
                g_prg.getProgressBar().setMinimum(0);
                g_prg.getProgressBar().setMaximum(asm.getObjectCode().length - 1);
            }

            PLPMsg.D("Starting worker threads", 2, this);
            prg.start();
            p_watchdog.start();
            return Constants.PLP_OK;

        } else
            return PLPMsg.E("No assembled sources.",
                            Constants.PLP_PRG_SOURCES_NOT_ASSEMBLED, this);

        } catch(Exception e) {
            e.printStackTrace();
            return PLPMsg.E("Programming failed.\n" + e,
                            Constants.PLP_GENERIC_ERROR, this);
        }
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

    public int newAsm(String name) {
        asms.add(new PLPAsmSource("# New ASM File", name, asms.size()));

        modified = true;

        if(g) refreshProjectView(true);

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

    public boolean g() {
        return g;
    }

    @Override
    public String toString() {
        return "PLP(" + plpfile + ")";
    }

    // ******************************************************************
    // Asynchronous project driver

    public int command = 0;
    public String strArg = null;
    public int intArg = -1;
    public boolean busy = false;

    @Override
    public void run() {
        try {
        busy = false;

        while(command != -1) {

            this.wait();
            busy = true;
            switch(command) {
                case 1:
                    this.assemble();
                    break;
                case 2:
                    this.create();
                    break;
                case 3:
                    this.create(strArg);
                    break;
            }

            busy = false;
        }

        PLPMsg.I("ProjectDriver thread exiting", this);
        
        } catch(Exception e) {
            PLPMsg.E("ProjectDriver thread quits unexpectedly",
                    Constants.PLP_BACKEND_THREAD_EXCEPTION, this);
        }
    }
}
