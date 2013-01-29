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

/**
 *
 * @author Wira
 */
public class PLPToolConnector implements ModuleInterface5 {

    public static ProjectDriver plp;
    public static int mode;

    public static final int EDITING = 0;
    public static final int SIMULATION_RESET = 1;
    public static final int SIMULATION_IDLE = 2;
    public static final int SIMULATION_STEP = 3;

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
        PLPToolConnector.plp = plp;

        return Constants.PLP_OK;
    }


}
