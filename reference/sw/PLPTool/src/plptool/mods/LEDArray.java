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

import java.awt.Color;
import plptool.PLPSimBusModule;
import plptool.Constants;

/**
 * An example of a bus module utilizing the gui_eval function. When evaluated,
 * this module lights up the correct LEDs on the LEDArrayFrame class.
 *
 * @author wira
 */
public class LEDArray extends PLPSimBusModule {

    public LEDArray(long addr) {
        super(addr, addr, true);
    }

    public int eval() {
        // No need to eval every cycle
        return Constants.PLP_OK;
    }

    public int gui_eval(Object x) {
        if(!enabled)
            return Constants.PLP_OK;

        // Get register value
        if(!isInitialized(startAddr)) {
            ((LEDArrayFrame)x).setAddress(super.startAddr());
            ((LEDArrayFrame)x).setValue(0);

            return Constants.PLP_SIM_BUS_ERROR;
        }
        
        long value = (Long) super.read(super.startAddr);

        ((LEDArrayFrame)x).setAddress(super.startAddr());
        ((LEDArrayFrame)x).setValue(value);

        // Combinational logic
        for(int i = 7; i >= 0; i--) {
            if((value & (long) Math.pow(2, i)) == (long) Math.pow(2, i))
                ((LEDArrayFrame)x).getLED(i).setBackground(Color.GREEN);
            else
                ((LEDArrayFrame)x).getLED(i).setBackground(Color.BLACK);
        }

        return Constants.PLP_OK;
    }

    @Override public void reset() {
        super.writeReg(super.startAddr, new Long(0L), false);
    }

    public String introduce() {
        return "LED array";
    }

    @Override
    public String toString() {
        return "LEDArray";
    }
}
