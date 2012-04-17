/*
    Copyright 2010 David Fritz, Brian Gordon, Wira Mulia

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

package plptool.mods;
import plptool.Constants;
import plptool.PLPSimBusModule;
import plptool.Msg;

/**
 * PLP Switches I/O module.
 *
 * @see PLPSimBusModule
 * @author wira
 */
public class Switches extends PLPSimBusModule {

    public Switches(long addr) {
        super(addr, addr, true);
    }

    public int eval() {
        // No need to eval every cycle
        return Constants.PLP_OK;
    }

    public int gui_eval(Object x) {
        return Constants.PLP_OK;
    }

    public String introduce() {
        return "Switches";
    }

    @Override
    public int write(long addr, Object data, boolean isInstr) {
        return Msg.E("write(): Write to read only register.",
                        Constants.PLP_SIM_WRITE_TO_READONLY_MODULE, this);
    }

    @Override public void reset() {
        // nothing, read only module
    }

    @Override
    public String toString() {
        return "Switches";
    }
}
