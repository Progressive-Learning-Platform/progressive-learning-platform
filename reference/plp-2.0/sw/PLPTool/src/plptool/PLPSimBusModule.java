package plptool;

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

import java.util.TreeMap;

/**
 * This is an abstract for a I/O module that can be attached to PLPMIPSSim
 * front-side bus.
 *
 * @author wira
 */
public abstract class PLPSimBusModule {
    protected TreeMap<Long, Long> values;
    protected TreeMap<Long, Boolean>  isInstr;
    protected boolean wordAligned;
    protected long startAddr;
    protected long endAddr;
    protected boolean enabled;

    public PLPSimBusModule() {
        values = new TreeMap<Long, Long>();
        isInstr = new TreeMap<Long, Boolean>();
        enabled = false;
    }

    public int write(long addr, long data, boolean isInstr) {
        if(!enabled)
            return PLPMsg.PLP_SIM_MODULE_DISABLED;

        if(wordAligned && ((addr / 4) >= endAddr || addr < startAddr))
            return PLPMsg.E("write(" + String.format("0x%08x", addr) + "): Address out of range.",
                            PLPMsg.PLP_SIM_OUT_ADDRESS_OUT_OF_RANGE, this);
        else if(wordAligned && addr % 4 != 0)
            return PLPMsg.E("write(" + String.format("0x%08x", addr) + "): Unaligned memory.",
                            PLPMsg.PLP_SIM_OUT_UNALIGNED_MEMORY, this);
        else if(!wordAligned && ((addr) >= endAddr || addr < startAddr))
            return PLPMsg.E("write(" + String.format("0x%08x", addr) + "): Address out of range.",
                            PLPMsg.PLP_SIM_OUT_ADDRESS_OUT_OF_RANGE, this);
        else if(wordAligned) {
            values.put(new Long(addr / 4), new Long(data));
            this.isInstr.put(new Long(addr / 4), isInstr);
        }
        else {
            values.put(new Long(addr), new Long(data));
            this.isInstr.put(new Long(addr), isInstr);
        }

        return PLPMsg.PLP_OK;
    }

    public long read(long addr) {
        if(wordAligned && ((addr / 4) >= endAddr || addr < startAddr)) {
            PLPMsg.E("read(" + String.format("0x%08x", addr) + "): Address out of range.",
                     PLPMsg.PLP_SIM_OUT_ADDRESS_OUT_OF_RANGE, this);
            return PLPMsg.PLP_ERROR_RETURN;
        }
        else if (wordAligned && addr % 4 != 0) {
            PLPMsg.E("read(" + String.format("0x%08x", addr) + "): Unaligned memory.",
                            PLPMsg.PLP_SIM_OUT_UNALIGNED_MEMORY, this);
            return PLPMsg.PLP_ERROR_RETURN;
        }
        else if(!wordAligned && ((addr) >= endAddr || addr < startAddr)) {
            PLPMsg.E("read(" + String.format("0x%08x", addr) + "): Address out of range.",
                            PLPMsg.PLP_SIM_OUT_ADDRESS_OUT_OF_RANGE, this);
            return PLPMsg.PLP_ERROR_RETURN;
        }
        else if(!wordAligned && !values.containsKey(addr)) {
            if(PLPCfg.cfgSimDynamicMemoryAllocation) {
                PLPMsg.I("read(" + String.format("0x%08x", addr) +
                         "): Dynamic memory allocation.", this);
                values.put(addr, (long) 0);
                isInstr.put(addr, false);
                return 0;
            }
            PLPMsg.E("read(" + String.format("0x%08x", addr) + "): Address not initialized.",
                             PLPMsg.PLP_SIM_UNINITIALIZED_MEMORY, this);
            return PLPMsg.PLP_ERROR_RETURN;
        }
        else if(wordAligned && !values.containsKey(addr / 4)) {
            if(PLPCfg.cfgSimDynamicMemoryAllocation) {
                PLPMsg.I("read(" + String.format("0x%08x", addr) +
                         "): Dynamic memory allocation.", this);
                values.put(addr / 4, (long) 0);
                isInstr.put(addr / 4, false);
                return 0;
            }
            PLPMsg.E("read(" + String.format("0x%08x", addr) + "): Address not initialized.",
                             PLPMsg.PLP_SIM_UNINITIALIZED_MEMORY, this);
            return PLPMsg.PLP_ERROR_RETURN;
        }
        else if(wordAligned) {
            return values.get(addr / 4);
        }
        else
            return values.get(addr);
    }

    public void clear() {
        values = new TreeMap<Long, Long>();
        isInstr = new TreeMap<Long, Boolean>();
    }

    // returns size of memory in WORDS
    public long size() {
        return endAddr - startAddr + 1;
    }

    public long endAddr() {
        return endAddr;
    }

    public long startAddr() {
        return startAddr;
    }

    public void enable() {
        enabled = true;
    }

    public void disable() {
        enabled = false;
    }

    public boolean enabled() {
        return enabled;
    }

    public boolean isInstr(long addr) {
        if(wordAligned)
            // return super.isInstr[(int) addr / 4];
            return isInstr.get(addr / 4);
        else
            // return super.isInstr[(int) addr];
            return isInstr.get(addr);
    }

    public boolean iswordAligned() {
        return wordAligned;
    }

    // each module does its own thing when evaluated. But most of them
    // will HAVE to do something!
    abstract int eval();

    // introduction string when the module is loaded
    abstract String introduce();
}
