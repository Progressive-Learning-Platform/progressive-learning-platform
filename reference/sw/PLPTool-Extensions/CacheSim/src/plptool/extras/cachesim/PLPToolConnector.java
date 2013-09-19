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

package plptool.extras.cachesim;

import plptool.*;
import plptool.dmf.*;
import plptool.gui.*;

import javax.swing.JMenuItem;
import java.awt.event.*;

/**
 *
 * @author Wira
 */
public class PLPToolConnector implements ModuleInterface5 {

    public String getName() {
        return "PLPTool Cache Simulator Module";
    }

    public String getDescription() {
        return "This module captures bus read and write events from the " +
               "simulation and simulates a memory hierarchy. The module " +
               "also provides visualization that can be used as a teaching " +
               "aid in class.";
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
        CallbackRegistry.register(new BusReadCallback(), CallbackRegistry.BUS_POST_READ);
        CallbackRegistry.register(new BusWriteCallback(), CallbackRegistry.BUS_WRITE);
        CallbackRegistry.register(new GUIUpdateCallback(), CallbackRegistry.GUI_UPDATE);
        CallbackRegistry.register(new ModeSetCallback(),
                CallbackRegistry.PROJECT_NEW,
                CallbackRegistry.PROJECT_OPEN_SUCCESSFUL,
                CallbackRegistry.EXIT,
                CallbackRegistry.EVENT_SIMULATE,
                CallbackRegistry.EVENT_DESIMULATE,
                CallbackRegistry.SIM_RESET,
                CallbackRegistry.SIM_STEP,
                CallbackRegistry.SIM_POST_STEP
        );
        CallbackRegistry.register(new DebugCallback(), CallbackRegistry.COMMAND);
        Log.plp = plp;

        JMenuItem menuClient = new JMenuItem("Create a Cache Simulator...");
        menuClient.setIcon(new javax.swing.ImageIcon(PLPToolApp.getImage("plptool.extras.cachesim.icon.png")));
        menuClient.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                Log.spawnCacheFrame();
            }
        });
        if(plp.g()) {
            plp.g_dev.addToolsItem(menuClient);
        }

        return Constants.PLP_OK;
    }


}
