/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.ffps;

import plptool.Constants;
import plptool.PLPSimBusModule;

/**
 *
 * @author wira
 */
public class Timer extends PLPSimBusModule {
    private long value;

    public Timer(long addr) {
        super(addr, addr, true);
    }

    @Override
    public int eval() {
        value++;
        if(value == (long) Math.pow(2, 32))
            value = 0;
        return Constants.PLP_OK;
    }

    @Override
    public int gui_eval(Object x) {
        return Constants.PLP_OK;
    }

    @Override
    public Object read(long addr) {
        if(addr==startAddr) return (Long) value;
        return null;
    }

    @Override
    public int write(long addr, Object data, boolean isInstr) {
        if(addr==startAddr) value = (Long) data;
        return Constants.PLP_OK;
    }

    public void reset() {
        value = 0;
    }

    public String introduce() {
        return "FFPS Timer Module";
    }

    public String toString() {
        return introduce();
    }
}
