/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.gui;

import plptool.Constants;
import plptool.PLPMsg;

import org.apache.commons.compress.archivers.tar.TarArchiveOutputStream;
import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileInputStream;

import java.util.Scanner;
import java.util.ArrayList;

/**
 * This is the PLPTool application project backend.
 *
 * @author wira
 */
public class PLPBackend {

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
    }

    public int setArch(String arch) {
        this.arch = arch;

        return Constants.PLP_OK;
    }

    public int savePLPFile(String path) {

        return Constants.PLP_OK;
    }

    public int openPLPFile(String path) {
        File plpFile = new File(path);
        asms = new ArrayList<plptool.PLPAsmSource>();

        if(!plpFile.exists())
            return PLPMsg.E(path + " not found.",
                            Constants.PLP_FILE_OPEN_ERROR, this);

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
                asmIndex++;
            }

            if(entry.getName().equals("plp.metafile")) {
                meta = metaStr;
            }
        }

        }
        catch(Exception e) {

        }


        return Constants.PLP_OK;
    }

    public int link() {

        return Constants.PLP_OK;
    }

    public int assemble() {

        if(asms.isEmpty())
            return PLPMsg.E("No source files are open.",
                            Constants.PLP_NO_ASM_OPEN, this);

        // ...assemble asm objects... //
        if(arch.equals("plpmips")) {
            int ret;
            asm = new plptool.mips.Asm(asms);

            if(asm.preprocess(0) == Constants.PLP_OK)
                asm.assemble();
        }

        if(g) {
            // ...drive GUI ... //
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
}
