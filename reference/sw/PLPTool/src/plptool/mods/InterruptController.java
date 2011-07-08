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

import plptool.Constants;
import plptool.Msg;
import plptool.PLPSimBusModule;
import plptool.mips.*;

/**
 * PLP interrupt controller
 *
 * @author wira
 */
public class InterruptController extends PLPSimBusModule {

    SimCore sim;

    public InterruptController(long addr, plptool.PLPSimCore sim) {
        super(addr, addr+4, true);
        this.sim = (SimCore) sim;
    }

    public int eval() {

        if(!(sim instanceof SimCore))
            return Constants.PLP_SIM_UNSUPPORTED_ARCHITECTURE;

	long stat = (Long) super.readReg(super.startAddr) & 0xfffffffe;
	long mask = (Long) super.readReg(super.startAddr + 0x4) & 0xfffffffe;

        boolean gie = ((Long) super.readReg(super.startAddr + 0x4) & 1) == 1;

	// IRQ = (stat[31:0] & mask[31:0] != 0)
        if(gie && ((stat & mask) != 0)) {

	    // raise IRQ
            sim.setIRQ(stat);

            super.writeReg(super.startAddr, new Long(0x1L),false);
        }

        return Constants.PLP_OK;
    }

    public int gui_eval(Object x) {
        //No GUI
	return Constants.PLP_OK;
    }

    public String introduce() {
        return "PLP Interrupt Controller";
    }

    @Override
    public Object read(long addr) {
        if(addr == super.startAddr) {
            return (Long) super.readReg(addr) | 0x1L;
        }
        else
            return super.readReg(addr);
    }

    @Override
    public String toString() {
        return "InterruptController";
    }
}
