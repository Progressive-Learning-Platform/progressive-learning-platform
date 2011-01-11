/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.gui;

import plptool.Constants;
import plptool.PLPMsg;

import org.jdesktop.application.Application;
import org.jdesktop.application.SingleFrameApplication;

import org.apache.commons.compress.archivers.tar.TarArchiveOutputStream;
import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileInputStream;

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
    public plptool.gui.PLPSimulator            g_simui;    // PLP Simulator Frontend
    public plptool.gui.PLPIORegistry           g_ioreg;    // I/O registry GUI
    public plptool.gui.PLPDevelop              g_dev;      // IDE GUI
    public plptool.gui.PLPErrorFrame           g_err;      // Error frame
    public plptool.gui.PLPToolAboutBox         g_about;    // About Frame
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

        refreshProjectView();

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

        

        if(g) {
            refreshProjectView();
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
        g_dev.getEditor().setText(asms.get(0).getAsmString());
        g_dev.getEditor().setEnabled(true);
        g_dev.enableBuildControls();
        open_asm = 0;
        g_dev.setCurFile(asms.get(open_asm).getAsmFilePath());

        app.hide(g_simui);
        g_simui.destroySimulation();

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
            g_simui.destroySimulation();
            g_ioreg = new PLPIORegistry(this);
            sim = (plptool.mips.SimCore) new plptool.mips.SimCore((plptool.mips.Asm) asm, -1);
            sim.reset();
            g_sim = new plptool.mips.SimCoreGUI(this);
            g_simui.getSimDesktop().add(g_sim);
            g_simui.getSimDesktop().add(g_err);
            g_simui.getSimDesktop().add(g_ioreg);
            g_sim.updateComponents();
            g_sim.setVisible(true);
            g_err.setVisible(true);

            app.show(g_simui);
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
        return "PLPBackend(" + plpfile + ")";
    }
}
