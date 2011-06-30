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

	long stat = (Long) super.readReg(super.startAddr);
	long mask = (Long) super.readReg(super.startAddr + 0x4);

	// IRQ = (stat[30:0] & mask[30:0] != 0) & stat[31]<GIE> 
        if((((stat & 0xefffffffL) & (mask & 0xefffffffL)) != 0)
                && (stat & 0x80000000L) == 0x80000000L) {

            // disable GIE
            super.writeReg(super.startAddr, stat & 0x7fffffffL, false);

	    // raise IRQ
            sim.setIRQ(stat);
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
	long ISR = (Long) super.readReg(super.startAddr+24);
        if(addr == super.startAddr) {
            Asm asm = new Asm("li $k0," + ISR, "inline");
            asm.preprocess(0);
            asm.assemble();
            return asm.getObjectCode()[0];
        }
        else if(addr == super.startAddr + 4) {
            Asm asm = new Asm("li $k0," + ISR, "inline");
            asm.preprocess(0);
            asm.assemble();
            return asm.getObjectCode()[1];
        }
        else if(addr == super.startAddr + 8) {
            Asm asm = new Asm("jr $k0", "inline");
            asm.preprocess(0);
            asm.assemble();
            return asm.getObjectCode()[0];
        }
        else if(addr == super.startAddr + 12) {
            return 0L;
        }
        else
            return super.readReg(addr);
    }

    @Override
    public String toString() {
        return "InterruptController";
    }
}
