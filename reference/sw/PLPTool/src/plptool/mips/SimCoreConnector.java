/*
    Copyright 2011 David Fritz, Brian Gordon, Wira Mulia

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
package plptool.mips;

import plptool.PLPSimBusModule;
import plptool.Constants;
import plptool.mips.visualizer.CPUVisualization;
import plptool.Msg;

/**
 *
 * @author wira
 */
public class SimCoreConnector extends PLPSimBusModule {

    CPUVisualization v;

    public SimCoreConnector(CPUVisualization v) {
        super(-1, -1, true);
        this.v = v;
    }

    public int eval() {
        if(enabled)
            v.update();
        return Constants.PLP_OK;
    }

    public int gui_eval(Object x) {
        return Constants.PLP_OK;
    }

    public void reset() {
        v.reset();
        v.update();
    }

    public String introduce() {
        return "SimCoreConnector";
    }

    @Override
    public String toString() {
        return "SimCoreConnector";
    }
}
