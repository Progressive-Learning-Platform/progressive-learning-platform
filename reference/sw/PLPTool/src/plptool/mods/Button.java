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
import plptool.PLPSimBusModule;
import plptool.Msg;

/**
 * PLP Button Interrupt module.
 *
 * @see PLPSimBusModule
 * @author wira
 */
public class Button extends PLPSimBusModule {

    private boolean pressed;
    private plptool.PLPSimCore sim;
    private long bit;
    private long mask;

    public Button(long bit, long mask, plptool.PLPSimCore sim) {
        super(-1, -1, true);
        pressed = false;
        this.sim = sim;
        this.bit = bit;
        this.mask = mask;
    }

    public int eval() {
        if(!enabled)
            return Constants.PLP_OK;

        if(pressed) {
            sim.setIRQ(bit);
        } else
            sim.maskIRQ(mask);

        return Constants.PLP_OK;
    }

    public void setPressedState(boolean s) {
        pressed = s;
    }

    public int gui_eval(Object x) {
        return Constants.PLP_OK;
    }

    public String introduce() {
        return "Interrupt Button";
    }

    @Override public void reset() {
        // nothing, read only module
    }

    @Override
    public String toString() {
        return "Interrupt Button";
    }
}
