/*
    Copyright 2013 Wira Mulia

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

package plptool.extras.socket;

import plptool.*;
import plptool.dmf.*;
import plptool.gui.*;

/**
 *
 * @author Wira
 */
public class PLPToolConnector implements ModuleInterface5 {
    public String getName() {
        return "PLPTool Socket Interface Module";
    }

    public String getDescription() {
        return "This module enables control of PLPTool with a socket interface.";
    }

    public int[] getVersion() {
        int[] ver = {1, 0};
        return ver;
    }

    public int[] getMinimumPLPToolVersion() {
        int[] ver = {5, 0};
        return ver;
    }

    public int initialize(ProjectDriver plp) {
        int port = 12800;
        String attr;
        CallbackRegistry.register(new HeadlessCallback(), CallbackRegistry.EVENT_HEADLESS_START);
        attr = PLPToolApp.getAttributes().get("SocketInterfacePort");
        if(attr != null) {
            try {
                port = Integer.parseInt(attr);
            } catch (NumberFormatException nfe) {
                Msg.W("SocketInterface: Failed to parse port '" + attr + "', reverting to default port 12800", null);
                port = 12800;
            }
        }
        SocketInterface.setPort(port);
        attr = PLPToolApp.getAttributes().get("OutputLogFile");
        if(attr != null)
            Msg.setLogOutStreamFile(attr);
        attr = PLPToolApp.getAttributes().get("ErrorLogFile");
        if(attr != null)
            Msg.setLogErrStreamFile(attr);

        return Constants.PLP_OK;
    }
}
