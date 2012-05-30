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

package plptool.ffps;

import plptool.Constants;
import plptool.PLPSimBusModule;

/**
 *
 * @author wira
 */
public class Timer extends PLPSimBusModule {
    private long value;
    private long overflow = (long) Math.pow(2, 32);

    public Timer() {}

    public Timer(long addr) {
        super(addr, addr, true);
    }

    @Override
    public int eval() {
        value++;
        if(value == overflow)
            value = 0;
        return Constants.PLP_OK;
    }

    @Override
    public int gui_eval(Object x) {
        return Constants.PLP_OK;
    }

    @Override
    public Object read(long addr) {
        if(addr==startAddr) return (Long) value;
        return null;
    }

    @Override
    public int write(long addr, Object data, boolean isInstr) {
        if(addr==startAddr) value = (Long) data;
        return Constants.PLP_OK;
    }

    public void reset() {
        value = 0;
    }

    public String introduce() {
        return "FFPS Timer Module";
    }

    public String toString() {
        return introduce();
    }
}
