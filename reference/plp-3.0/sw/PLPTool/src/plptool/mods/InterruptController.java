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
import plptool.gui.ProjectDriver;
import plptool.mips.*;

/**
 * PLP interrupt controller
 *
 * @author wira
 */
public class InterruptController extends PLPSimBusModule {

    ProjectDriver plp;

    public InterruptController(ProjectDriver plp) {
        super(0xf0700000L, 0xf0700000L+32, true);
        this.plp = plp;
    }

    public int eval() {

        if(!plp.getArch().equals("plpmips"))
            return Constants.PLP_SIM_UNSUPPORTED_ARCHITECTURE;

        if((((Long) super.readReg(0xf0700010L) & 0xefffffffL)
          & ((Long) super.readReg(0xf0700014L) & 0xefffffffL)) != 0
                && ((Long) super.readReg(0xf0700010L) & 0x80000000L) == 0x80000000L) {
            super.writeReg(0xf070001cL, ((SimCore)plp.sim).pc.eval(), false);
            plp.sim.setIRQ(1);
            super.writeReg(0xf0700010L, (Long) super.readReg(0xf0700010L) & 0xefffffffL, false);
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
            Asm asm = new Asm("li $k0," + super.readReg(super.startAddr+24), "inline");
            asm.preprocess(0);
            asm.assemble();
            return asm.getObjectCode()[0];
        }
        else if(addr == super.startAddr + 4) {
            Asm asm = new Asm("li $k0," + super.readReg(super.startAddr+24), "inline");
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
