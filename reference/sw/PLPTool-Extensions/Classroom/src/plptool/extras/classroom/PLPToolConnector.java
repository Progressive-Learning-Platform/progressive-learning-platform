/*
    Copyright 2012 PLP Contributors

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

package plptool.extras.classroom;

import plptool.*;
import plptool.dmf.*;
import plptool.gui.ProjectDriver;
import javax.swing.JMenuItem;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 *
 * @author Wira
 */
public class PLPToolConnector implements ModuleInterface5 {
    private ProjectDriver plp;
    private boolean active = false;
    private ServerService service;
    private ServerControl serverControl;
    private Client client;

    public String getName() {
        return "PLPTool Collaborative Programming Module";
    }

    public String getDescription() {
        return "This module enables collaborative environment in classroom by" +
                "using the network.";
    }

    public int initialize(ProjectDriver plp) {
        this.plp = plp;
        active = plp.plpfile != null;
        Msg.I("enabled", this);
        serverControl = new ServerControl(plp);
        client = new Client();
        JMenuItem menuServerControl = new JMenuItem("Classroom Server Controls...");
        menuServerControl.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                if(active)
                    serverControl.setVisible(true);
            }
        });
        JMenuItem menuClient = new JMenuItem("Classroom Client...");
        menuClient.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                client.setVisible(true);
            }
        });
        if(plp.g()) {
            plp.g_dev.addToolsItem(menuClient);
            plp.g_dev.addToolsItem(menuServerControl);
        }
        Callback_Project_Change_Handler cb = new Callback_Project_Change_Handler();
        CallbackRegistry.register(cb,
                CallbackRegistry.PROJECT_OPEN_SUCCESSFUL,
                CallbackRegistry.PROJECT_NEW);
        return Constants.PLP_OK;
    }

    public void setActive() {
        active = true;
    }

    public int[] getVersion() {
        int[] version = {0, 2};
        return version;
    }

    public int[] getMinimumPLPToolVersion() {
        int[] version = {5, 0};
        return version;
    }

    @Override
    public String toString() {
        return "PLPTool Classroom Module";
    }

    class Callback_Project_Change_Handler implements Callback {
        public boolean callback(int callbackNum, Object param) {
            Msg.D("Now we're talking.", 2, null);
            setActive();
            return true;
        }
    }
}

