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
public class FastMemory extends PLPSimBusModule {
    private String id;
    private long[] data;

    public FastMemory(long addr, int size, String id) {
        super(addr, addr+size-4, true);
        this.id = id;
        data = new long[size/4+1];
    }

    @Override
    public int eval() {
        return Constants.PLP_OK;
    }

    @Override
    public int gui_eval(Object x) {
        return Constants.PLP_OK;
    }

    @Override
    public Object read(long addr) {
        int index = (int) (addr-startAddr)/4;
        if(index >= 0 && index < data.length) return data[index];
        return null;
    }

    @Override
    public int write(long addr, Object data, boolean isInstr) {
        int index = (int) (addr-startAddr)/4;
        if(index >= 0 && index < this.data.length)
            this.data[index] = (Long) data;
        return Constants.PLP_OK;
    }

    public void reset() {
        data = new long[data.length];
    }

    public String introduce() {
        return "FFPS Fast Memory: " + id;
    }

    public String toString() {
        return "FFPS Fast Memory: " + id;
    }
}
