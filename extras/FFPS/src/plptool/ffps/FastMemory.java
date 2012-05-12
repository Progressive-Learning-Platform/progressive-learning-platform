/*
    Copyright 2012 PLP Contributors

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

    public FastMemory() {}

    @Override
    public void setNewParameters(long startAddr, long endAddr, boolean isWordAligned) {
        super.setNewParameters(startAddr, endAddr, isWordAligned);
        this.id = "Dynamically Loaded";
        data = new long[(int)(endAddr-startAddr)/4+1];
    }

    public FastMemory(long addr, int size, String id) {
        super(addr, addr+size-4, true);
        this.id = id;
        data = new long[size/4+1];
    }

    public int eval() {
        return Constants.PLP_OK;
    }

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
