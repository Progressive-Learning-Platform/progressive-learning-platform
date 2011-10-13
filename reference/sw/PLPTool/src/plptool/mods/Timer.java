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


/**
 *
 * Simple timer module that increments at every cycle, is writable as well.
 *
 * @author fritz
 */
public class Timer extends PLPSimBusModule {

    private plptool.PLPSimCore sim;

    public Timer(long addr, plptool.PLPSimCore sim) {
        super(addr, addr, true);
        this.sim = sim;
    }

    public int eval() {
        if (super.isInitialized(super.startAddr)) {
            long timer = (Long)super.read(super.startAddr) & 0xffffffffL;
            timer++;
            if(timer == 0xffffffffL)
                sim.setIRQ(2);
            else
                sim.maskIRQ(0xfffffffdL);

            super.write(super.startAddr, timer, false);
        } else {
            super.write(super.startAddr,(long) 0,false);
        }
        return Constants.PLP_OK;
    }

    public int gui_eval(Object x) {
        //No GUI
	return Constants.PLP_OK;
    }

    @Override public void reset() {
        super.writeReg(startAddr, new Long(0L), false);
    }

    public String introduce() {
        return "Timer";
    }

    @Override
    public String toString() {
        return "Timer";
    }

}
