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

package plptool.extras.collab;

import plptool.*;
import plptool.gui.ProjectDriver;
import plptool.gui.ProjectEvent;
import javax.swing.JMenuItem;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 *
 * @author Wira
 */
public class PLPToolConnector implements PLPGenericModule {
    private ProjectDriver plp;
    private boolean active = false;
    private ServerService service;
    private ServerControl serverControl;
    private Client client;

    public Object hook(Object param) {
        if(param instanceof ProjectDriver) {
            this.plp = (ProjectDriver) param;
            active = plp.plpfile != null;
            init();
            return true;
        } else if(param instanceof ProjectEvent) {
            ProjectEvent ev = (ProjectEvent) param;
            switch(ev.getIdentifier()) {
                case ProjectEvent.NEW_PROJECT:
                case ProjectEvent.PROJECT_OPEN:
                    active = true;
            }
            return true;
        }

        return null;
    }

    private void init() {
        Msg.I("enabled", this);
        serverControl = new ServerControl(plp);
        client = new Client();
        JMenuItem menuServerControl = new JMenuItem("Collaborative Programming Server Controls...");
        menuServerControl.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                if(active)
                    serverControl.setVisible(true);
            }
        });
        JMenuItem menuClient = new JMenuItem("Collaborative Programming Client...");
        menuClient.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                client.setVisible(true);
            }
        });
        plp.g_dev.addToolsItem(menuClient);
        plp.g_dev.addToolsItem(menuServerControl);
    }

    public String getVersion() {
        return "0.1";
    }

    @Override
    public String toString() {
        return "Collaborative Programming";
    }
}
