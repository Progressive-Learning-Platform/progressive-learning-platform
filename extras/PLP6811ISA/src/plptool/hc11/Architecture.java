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
import plptool.gui.ProjectDriver;

import javax.swing.*;

/**
 *
 * @author wira
 */
public class Architecture extends PLPArchitecture {
    JMenuItem menuBuffaloInterface;
    JMenuItem menuExportS19;
    JSeparator menuSeparator;
    Buffalo b;

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
        menuSeparator = new JSeparator();
        menuBuffaloInterface.setAccelerator(KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F7, 0));
        b = new Buffalo(plp);
        b.setLocationRelativeTo(plp.g_dev);

        menuBuffaloInterface.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent e) {
                b.setVisible(true);
            }
        });

        menuExportS19.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent e) {
                if(plp.isAssembled()) {
                    java.io.File f = PLPToolbox.saveFileDialog(Constants.launchPath, null);
                    if(f != null) {
                        PLPToolbox.writeFile(((Asm)plp.asm).generateS19(), f.getAbsolutePath());
                    }
                } else
                    Msg.E("Program must be assembled first.", Constants.PLP_GENERIC_ERROR, null);
            }
        });

        plp.g_dev.addToolsItem(menuSeparator);
        plp.g_dev.addToolsItem(menuBuffaloInterface);
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
        Msg.W("Not implemented... yet", null);
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
        plp.g_dev.removeToolsItem(menuBuffaloInterface);
        plp.g_dev.removeToolsItem(menuSeparator);
        b.terminate();
        b.dispose();
    }
}
