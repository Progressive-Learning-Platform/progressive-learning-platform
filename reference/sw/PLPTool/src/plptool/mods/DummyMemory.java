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
import plptool.Text;
import plptool.PLPSimBusModule;

/**
 * Dummy memory module. Will return 0 if read is called.
 *
 * @author wira
 */
public class DummyMemory extends PLPSimBusModule {
    public DummyMemory(long addr, long size, boolean wordAligned) {
        super(addr, addr + (wordAligned ? size - 4: size - 1), wordAligned);
    }

    public int eval () { return Constants.PLP_OK; }

    public int gui_eval(Object x) { return Constants.PLP_OK; }

    public String introduce() {
        return "Dummy Memory Module " + Text.versionString;
    }

    @Override public Long read(long addr) {
        return new Long(0);
    }

    @Override public void reset() {
        super.clear();
    }

    @Override public String toString() {
        return "MemModule";
    }
}
