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

import plptool.PLPSimBusModule;
import plptool.Constants;

/**
 * PLPID is a module that returns the version string and board frequency
 * of the system.
 *
 * @author wira
 */
public class PLPID extends PLPSimBusModule {

    long frequency = 20;

    public PLPID(long addr) {
        super(addr, addr + 4, true);
    }

    public void updateFrequency(long f) {
        frequency = f;
    }

    public int eval() {
        // No need to eval every cycle
        return Constants.PLP_OK;
    }

    public int gui_eval(Object x) {
	return Constants.PLP_OK;
    }

    @Override
    public Object read(long addr) {
        if(addr == startAddr)
            return new Long(0x202);
        else if(addr == startAddr + 4)
            return frequency;

        return null;
    }

    @Override public void reset() {
        super.clear();
    }

    public String introduce() {
        return "PLPID";
    }

    @Override
    public String toString() {
        return "PLPID";
    }
}
