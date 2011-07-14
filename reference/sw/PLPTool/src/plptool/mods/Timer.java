/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
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

    public String introduce() {
        return "Timer";
    }

    @Override
    public String toString() {
        return "Timer";
    }

}
