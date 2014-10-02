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
import plptool.PLPToolbox;
import plptool.dmf.*;

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

	long mask = (Long) super.readReg(super.startAddr) & 0xfffffffe;
	long stat = ((Long) super.readReg(super.startAddr+4) & 0xfffffffe) |
                    sim.getIRQ();
        super.writeReg(super.startAddr+4, new Long(stat), false);

        boolean gie = ((Long) super.readReg(super.startAddr) & 1) == 1;

        if(gie && ((stat & mask) != 0) && ((SimCore)sim).IRQAck == 0) {

            Msg.D("IRQ: " + String.format("%02x", sim.getIRQ()) +
                  " stat: " + String.format("%02x", stat) +
                  " mask: " + String.format("%02x", mask), 3, this);

	    // raise IRQ
            ((SimCore)sim).int_state = 3;
            
            // clear GIE
            super.writeReg(super.startAddr, new Long(mask), false);
        }

        return Constants.PLP_OK;
    }

    public int gui_eval(Object x) {
        //No GUI
	return Constants.PLP_OK;
    }

    @Override public void reset() {
        super.writeReg(super.startAddr, new Long(0L), false);
        super.writeReg(super.startAddr+4, new Long(0L), false);
    }

    public String introduce() {
        return "PLP Interrupt Controller";
    }

    @Override
    public int write(long addr, Object data, boolean isInstr) {
        Msg.D("Writing " + ((Long) data) + " to " + String.format("0x%08x", addr), 1, this);
        super.writeReg(addr, data, isInstr);
        
        return Constants.PLP_OK;
    }


    @Override
    public Object read(long addr) {
        if(addr == super.startAddr+4) {
            return (Long) super.readReg(addr) | 0x1L;
        } else
            return super.readReg(addr);
    }

    @Override
    public String toString() {
        return "InterruptController";
    }
}
