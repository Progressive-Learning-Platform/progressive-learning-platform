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

    public Timer(long addr) {
        super(addr, addr, true);
    }

    public int eval() {
        if (super.isInitialized(super.startAddr)) {
            long timer = (Long)super.read(super.startAddr);
            timer++;
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
