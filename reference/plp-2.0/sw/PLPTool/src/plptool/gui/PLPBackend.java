/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.gui;

import plptool.Constants;

/**
 * This is the PLPTool application backend.
 *
 * @author wira
 */
public class PLPBackend {

    public plptool.PLPCfg                      cfg;        // Configuration
    public plptool.PLPMsg                      msg;        // Messaging class

    public plptool.PLPAsm                      asm;        // Assembler
    public plptool.PLPAsm[]                    asm_array;  // Asm array
    public plptool.PLPLinker                   lnkr;       // Linker
    public plptool.PLPSerialProgrammer         prg;        // Programmer

    public plptool.PLPSimCore                  sim;        // Simulation core
    public plptool.mods.IORegistry             ioreg;      // I/O registry

    // GUI Members
    public plptool.PLPSimCoreGUI               g_sim;      // Sim GUI
    public plptool.gui.PLPToolView             g_main;     // Main GUI Window
    public plptool.gui.PLPIORegistry           g_ioreg;    // I/O registry GUI
    public plptool.gui.PLPDevelop              g_dev;      // IDE GUI
    public plptool.gui.PLPErrorFrame           g_err;      // Error frame
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

    public int link() {

        return Constants.PLP_OK;
    }

    public int assemble() {

        // ...assemble asm objects... //

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
