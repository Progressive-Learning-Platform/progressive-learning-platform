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

package plptool.extras.execviz;

import plptool.dmf.ModuleInterface5;
import plptool.dmf.Callback;
import plptool.dmf.CallbackRegistry;

import plptool.Msg;
import plptool.Constants;
import plptool.gui.ProjectDriver;

/**
 *
 * @author Wira
 */
public class PLPToolConnector implements ModuleInterface5 {
        private ProjectDriver plp;

	public String getName()                         { return "Program Execution Visualizer"; }
	public String getDescription()                  {
            return "A module that visualizes a program execution.";
        }
	public int[] getVersion()			{ int[] ver = {1, 0}; return ver; }
	public int[] getMinimumPLPToolVersion()         { int[] ver = {5, 0}; return ver; }

	public int initialize(ProjectDriver plp) {
		this.plp = plp;
		Msg.P("Initializing!");
		CallbackRegistry.register(new Callback_Step(), CallbackRegistry.SIM_STEP);
                return Constants.PLP_OK;
	}

	class Callback_Step implements Callback {
		public boolean callback(int callbackNum, Object param) {
			
			return true;
		}
	}
}
