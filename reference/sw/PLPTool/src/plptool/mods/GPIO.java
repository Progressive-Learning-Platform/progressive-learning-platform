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

package plptool.mods;

import plptool.PLPSimBusModule;
import plptool.Constants;

/**
 * The GPIO module as described in the PLP manual.
 *
 * @author wira
 */
public class GPIO extends PLPSimBusModule {

    private long oldTristate;

    public GPIO(long addr) {
        super(addr, addr+8, true);
        oldTristate = 0;
    }

    public int eval() {
        // No need to eval every cycle
        return Constants.PLP_OK;
    }

    public int gui_eval(Object x) {
        if(!enabled)
            return Constants.PLP_OK;

        GPIOFrame frame = (GPIOFrame) x;

        long curTristate = (Long) readReg(startAddr);
        if(curTristate != oldTristate) {
            frame.setTristateRegisterValues(curTristate);
            oldTristate = curTristate;
        }

        if(frame.isVisible())
            frame.updateOutputs();

        return Constants.PLP_OK;
    }

    @Override public void reset() {
        writeReg(startAddr, new Long(0L), false);
        writeReg(startAddr+4, new Long(0L), false);
        writeReg(startAddr+8, new Long(0L), false);
    }

    @Override public int write(long addr, Object data, boolean isInstr) {
        long mask;
        long d = (Long) data & 0xff;
        long prev, setMask, clearMask;

        // write to block A
        if(addr == startAddr+4) {
            mask = (Long) readReg(startAddr) & 0xffL;
            prev = (Long) readReg(startAddr+4);

            // write to block B
        } else if (addr == startAddr + 8) {
            mask = ((Long) readReg(startAddr) >> 8) & 0xffL;
            prev = (Long) readReg(startAddr+8);

        } else
            return writeReg(addr, data, false);
        
        setMask = d & mask;
        clearMask = d | ~mask;
        d = (prev | setMask) & clearMask;
        return writeReg(addr, d, false);
    }

    public String introduce() {
        return "GPIO";
    }

    @Override
    public String toString() {
        return "GPIO";
    }
}
