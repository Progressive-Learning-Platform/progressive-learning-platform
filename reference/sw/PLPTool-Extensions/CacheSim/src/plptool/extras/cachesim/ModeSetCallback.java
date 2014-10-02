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

import plptool.dmf.*;

/**
 *
 * @author Wira
 */
public class ModeSetCallback implements Callback {
    public boolean callback(int num, Object param) {
        switch(num) {
            case CallbackRegistry.PROJECT_NEW:
            case CallbackRegistry.PROJECT_OPEN_SUCCESSFUL:
            case CallbackRegistry.EVENT_DESIMULATE:
                Log.mode = Log.EDITING;
                Log.reset();
                break;
            case CallbackRegistry.EVENT_SIMULATE:
            case CallbackRegistry.SIM_RESET:
                Log.mode = Log.SIMULATION_RESET;
                Log.reset();
                break;
            case CallbackRegistry.SIM_STEP:
                Log.cycle++;
                Log.mode = Log.SIMULATION_STEP;
                break;
            case CallbackRegistry.SIM_POST_STEP:
                Log.mode = Log.SIMULATION_IDLE;
                break;
        }

        return true;
    }
}
