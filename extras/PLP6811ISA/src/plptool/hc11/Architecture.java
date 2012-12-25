/*
    Copyright 2012 David Fritz, Brian Gordon, Wira Mulia

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

package plptool.hc11;

import plptool.*;
import plptool.dmf.*;
import plptool.gui.ProjectDriver;
import plptool.gui.PLPToolApp;

import javax.swing.*;

/**
 *
 * @author wira
 */
public class Architecture extends PLPArchitecture {
    JMenuItem menuBuffaloInterface;
    JMenuItem menuExportS19;
    JMenuItem menuExportListing;
    JSeparator menuSeparator;
    Buffalo b;
    public String ttyName;
    public String ttyBaud;

    public Architecture() {
        super(6811, "hc11", null);
        Msg.M("***************************************");
        Msg.M("PLP HC11 ISA Implementation Module");
        Msg.M("***************************************");
        Msg.M("You are using an alpha product and there may be bugs...");
        Msg.M("...you have been warned!");

        hasAssembler = true;
    }

    @Override
    public void init() {
        menuBuffaloInterface = new JMenuItem("BUFFALO Interface");
        menuExportS19 = new JMenuItem("Export S19 File...");
        menuExportListing = new JMenuItem("Export Assembly Listing...");
        menuSeparator = new JSeparator();
        menuBuffaloInterface.setAccelerator(KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F7, 0));
        Object[] callbacks = CallbackRegistry.getCallbacks(CallbackRegistry.SAVE_CONFIG);
        boolean registered = false;
        for(int i = 0; i < callbacks.length; i++)
            if(callbacks[i] instanceof SaveConfig)
                registered = true;
        if(!registered)
            CallbackRegistry.register(new SaveConfig(), CallbackRegistry.SAVE_CONFIG);
        else
            Msg.D("HC11 Save Config already registered.", 3, null);

        ttyName = PLPToolApp.getAttributes().get("hc11_ttyName");
        ttyBaud = PLPToolApp.getAttributes().get("hc11_ttyBaud");

        b = new Buffalo(plp);
        b.setLocationRelativeTo(plp.g_dev);

        menuBuffaloInterface.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent e) {
                if(!plp.isSerialSupported()) {
                    Msg.E("BUFFALO requires native RXTX library for serial communication.",
                            Constants.PLP_BACKEND_NO_NATIVE_SERIAL_LIBS, null);
                    return;
                }
                b.setVisible(true);
            }
        });

        menuExportListing.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent e) {
                if(plp.isAssembled()) {
                    String str = "";
                    java.io.File f =
                            PLPToolbox.saveFileDialog(Constants.launchPath,
                                PLPToolbox.createFileFilter("lst", "Listing file (.lst)"));
                    if(f != null) {
                        try {
                            str += "Project: " + plp.plpfile.getAbsolutePath() + "\n";
                            for(int i = 0; i < plp.getAsms().size(); i++) {
                                str += "===============================================================================\n";
                                str += plp.getAsm(i).getAsmFilePath() + "\n";
                                str += "-------------------------------------------------------------------------------\n";
                                str += plp.getAsm(i).getAsmString() + "\n";
                            }
                            str += "===============================================================================\n";
                            str += ((Asm)plp.asm).generateListing();
                            PLPToolbox.writeFile(str, f.getAbsolutePath());
                        } catch(Exception ex) {
                            PLPToolbox.showErrorDialog(plp.g_dev, ex.getMessage());
                        }
                    }
                } else
                    Msg.E("Program must be assembled first.", Constants.PLP_GENERIC_ERROR, null);
            }
        });

        menuExportS19.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent e) {
                if(plp.isAssembled()) {
                    java.io.File f =
                            PLPToolbox.saveFileDialog(Constants.launchPath,
                                PLPToolbox.createFileFilter("s19", "Motorola S19 File (.s19)"));
                    if(f != null) {
                        try {
                            PLPToolbox.writeFile(((Asm)plp.asm).generateS19(), f.getAbsolutePath());
                        } catch(Exception ex) {
                            PLPToolbox.showErrorDialog(plp.g_dev, ex.getMessage());
                        }
                    }
                } else
                    Msg.E("Program must be assembled first.", Constants.PLP_GENERIC_ERROR, null);
            }
        });

        plp.g_dev.addToolsItem(menuSeparator);
        plp.g_dev.addToolsItem(menuBuffaloInterface);
        plp.g_dev.addToolsItem(menuExportListing);
        plp.g_dev.addToolsItem(menuExportS19);
        b = new Buffalo(plp);
    }

    public PLPAsm createAssembler() {
        return new Asm(plp.getAsms());
    }

    public PLPSerialProgrammer createProgrammer() {
        return null;
    }

    public PLPSimCore createSimCore() {
        return null;
    }

    public PLPSimCoreGUI createSimCoreGUI() {
        return null;
    }

    @Override
    public void newProject(ProjectDriver plp) {
        plp.getAsm(0).setAsmString("; Main 68HC11 Assembly Source");
    }

    @Override
    public void listing() {
        //Msg.W("Not implemented... yet", null);
        if(plp.isAssembled()) {
            Asm asm = (Asm) plp.asm;
            Msg.D("--- Listing ---", 3, null);
            String[] listing = asm.generateListing().split("\\n");
            for(int i = 0; i < listing.length; i++)
                Msg.p(listing[i]);
            Msg.P();
            System.out.println(asm.generateListing());
        }
    }

    @Override
    public String getQuickReferenceString() {
        return "<h1>Wira's PLP 68HC11 Assembler and BUFFALO Interface</h2>" +
                "<p>BETA Module, <b>use at your own risk!</b></p>";
    }

    @Override
    public void cleanup() {
        Msg.M("PLP 68HC11 ISA Implementation is cleaning up.");
        plp.g_dev.removeToolsItem(menuExportS19);
        plp.g_dev.removeToolsItem(menuExportListing);
        plp.g_dev.removeToolsItem(menuBuffaloInterface);
        plp.g_dev.removeToolsItem(menuSeparator);
        b.terminate();
        b.dispose();
    }

    class SaveConfig implements Callback {
        public boolean callback(int num, Object params) {
            java.io.FileWriter out = (java.io.FileWriter) params;
            if(ttyName != null && ttyBaud != null) {
                try {
                    out.write("hc11_ttyName::" + ttyName + "\n");
                    out.write("hc11_ttyBaud::" + ttyBaud + "\n");
                } catch(java.io.IOException e) {
                    Msg.E("HC11 save config failed.", Constants.PLP_IO_WRITE_ERROR, null);
                }
            }

            return true;
        }
    }
}
