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
import java.awt.Color;

/**
 * @author wira
 */
public class SevenSegments extends PLPSimBusModule {
    public SevenSegments(long addr) {
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
            ((SevenSegmentsFrame)x).setAddress(super.startAddr());
            ((SevenSegmentsFrame)x).setValue(0);

            return Constants.PLP_SIM_BUS_ERROR;
        }

        long value = (Long) super.read(super.startAddr);

        ((SevenSegmentsFrame)x).setAddress(super.startAddr());
        ((SevenSegmentsFrame)x).setValue(value);

        // Combinational logic
        for(int j = 3; j >= 0; j--) {
            for(int i = 7; i >= 0; i--) {
                if((value & (long) Math.pow(2, 8 * j + i)) == (long) Math.pow(2, 8 * j + i))
                    ((SevenSegmentsFrame)x).getLED(j, i).setBackground(Color.BLACK);
                else
                    ((SevenSegmentsFrame)x).getLED(j, i).setBackground(Color.RED);
            }
        }

        return Constants.PLP_OK;
    }

    @Override public void reset() {
        super.writeReg(startAddr, new Long(0xffffffffL), false);
    }

    public String introduce() {
        return "Seven Segments Display";
    }

    @Override
    public String toString() {
        return "SevenSegments";
    }
}
