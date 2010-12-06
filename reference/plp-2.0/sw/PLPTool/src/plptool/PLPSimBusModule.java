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
import java.util.Iterator;

/**
 * This is an abstract for a general purpose module that can be attached to the
 * front-side bus. Registers for the module are implemented using TreeMap.
 * This class provides all the necessary functions to implement a PLP
 * simulator bus module, such as read and write functions, and address space
 * management and error checking. The module then can be extended by
 * implementing eval() and gui_eval() functions, effectively tying the
 * module to the simulation core.
 *
 * @see PLPSimBus
 * @author wira
 */
public abstract class PLPSimBusModule {
    /**
     * Registers for the module
     */
    protected TreeMap<Long, Object> values;

    /**
     * Denotes each entry in the register whether it is an instruction.
     * Useful for main memory implementations or caches.
     */
    protected TreeMap<Long, Boolean>  isInstr;

    /**
     * Denotes whether the registers are addressed in memory-aligned fashion.
     */
    protected boolean wordAligned;

    /**
     * Starting address of the address space
     */
    protected long startAddr;

    /**
     * Final address of the address space (inclusive)
     */
    protected long endAddr;

    /**
     * Write-enable and whether evaluations are enabled, but subclasses
     * can choose to ignore this
     */
    protected boolean enabled;

    /**
     * The constructor for the superclass requires the address space and
     * whether the registers of the module are word-aligned
     *
     * @param startAddr
     * @param endAddr
     * @param wordAligned
     */
    public PLPSimBusModule(long startAddr, long endAddr, boolean wordAligned) {
        values = new TreeMap<Long, Object>();
        isInstr = new TreeMap<Long, Boolean>();
        this.startAddr = startAddr;
        this.endAddr = endAddr;
        this.wordAligned = wordAligned;
        enabled = false;
    }

    /**
     * Write data to one of the module's registers. Only possible when the
     * module is enabled. The write command is ignored otherwise.
     *
     * @param addr Address to write to
     * @param data Data to be written
     * @param isInstr Denotes whether the value to be written is an instruction
     * @return PLP_OK, or error code
     */
    public int write(long addr, Object data, boolean isInstr) {
        if(!enabled)
            return PLPMsg.PLP_SIM_MODULE_DISABLED;

        if(addr > endAddr || addr < startAddr)
            return PLPMsg.E("write(" + String.format("0x%08x", addr) + "): Address is out of range.",
                            PLPMsg.PLP_SIM_OUT_ADDRESS_OUT_OF_RANGE, this);
        else if(wordAligned && addr % 4 != 0)
            return PLPMsg.E("write(" + String.format("0x%08x", addr) + "): Requested address is unaligned.",
                            PLPMsg.PLP_SIM_OUT_UNALIGNED_MEMORY, this);
        else {
            if(values.containsKey(addr)) {
                values.remove(addr);
                this.isInstr.remove(addr);
            }
            values.put(new Long(addr), data);
            this.isInstr.put(new Long(addr), isInstr);
        }

        return PLPMsg.PLP_OK;
    }

    /**
     * Read one of the module's registers
     *
     * @param addr Address to read from
     * @return Data, or PLP_ERROR_RETURN
     */
    public Object read(long addr) {
        if(addr > endAddr || addr < startAddr) {
            PLPMsg.E("read(" + String.format("0x%08x", addr) + "): Address is out of range.",
                     PLPMsg.PLP_SIM_OUT_ADDRESS_OUT_OF_RANGE, this);
            return null;
        }
        else if (wordAligned && addr % 4 != 0) {
            PLPMsg.E("read(" + String.format("0x%08x", addr) + "): Requested address is unaligned.",
                            PLPMsg.PLP_SIM_OUT_UNALIGNED_MEMORY, this);
            return null;
        }
        else if(!values.containsKey(addr)) {
            if(PLPCfg.cfgSimDynamicMemoryAllocation) {
                PLPMsg.I("read(" + String.format("0x%08x", addr) +
                         "): Dynamic memory allocation.", this);
                values.put(addr, new Long(0));
                isInstr.put(addr, false);
                return 0;
            }
            PLPMsg.E("read(" + String.format("0x%08x", addr) + "): Address is not initialized.",
                             PLPMsg.PLP_SIM_UNINITIALIZED_MEMORY, this);
            return null;
        }
        else
            return values.get(addr);
    }

    /**
     * Reinitialize the module's registers
     */
    public void clear() {
        values = new TreeMap<Long, Object>();
        isInstr = new TreeMap<Long, Boolean>();
    }

    /**
     * Return the set of addresses and values in an n x 2 array. This can
     * be useful to modules that have to walk through the whole register file
     * on evaluation.
     *
     * @return n x 2 Object array containing addresses and values of the
     * memory module
     */
    public Object[][] getValueSet() {
        Object[][] valueSet = new Object[values.size()][2];
        int index = 0;

        Iterator keyIterator = values.keySet().iterator();

        while(keyIterator.hasNext()) {
            valueSet[index][0] = keyIterator.next();
            valueSet[index][1] = values.get((Long) valueSet[index][0]);
            index++;
        }

        return valueSet;
    }

    /**
     * Returns the width of the address space
     *
     * @return The width of the address space in (long)
     */
    public long size() {
        if(!wordAligned)
            return endAddr - startAddr + 1;
        else
            return (endAddr - startAddr) / 4 + 1;
    }

    /**
     * Returns the end of the address space
     *
     * @return Final address in (long)
     */
    public long endAddr() {
        return endAddr;
    }

    /**
     * Returns the beginning of the address space
     *
     * @return Starting address in (long)
     */
    public long startAddr() {
        return startAddr;
    }

    /**
     * Enable the module.
     */
    public void enable() {
        enabled = true;
    }

    /**
     * Disable the module.
     */
    public void disable() {
        enabled = false;
    }

    /**
     * Returns whether the module is enabled
     *
     * @return Status of module in (boolean)
     */
    public boolean enabled() {
        return enabled;
    }

    /**
     * Returns whether the specified register contains an instruction or not
     *
     * @param addr Address to read from
     * @return Whether the specified register contains instruction or not
     */
    public boolean isInstr(long addr) {
        return isInstr.get(addr);
    }

    /**
     * Returns whether the registers of the module are word-aligned
     *
     * @return Whether the registers of the module are word-aligned
     */
    public boolean iswordAligned() {
        return wordAligned;
    }

    /**
     * True if the specified address is initialized. False otherwise
     *
     * @param addr The address to lookup
     * @return Returns boolean
     */
    public boolean isInitialized(long addr) {
        return values.containsKey(addr);
    }

    /**
     * The eval() function represents the behavior of the module itself.
     * For example, an eval function for an array of LED will read its
     * register and light up proper LEDs.
     *
     * @return int
     */
    abstract public int eval();

    /**
     * gui_eval is designed for simulator developers / users to allow the
     * module to interact with the simulation environment directly.
     *
     * @param x Reference to a simulator object that this module will interact
     * with.
     * @return
     */
    abstract public int gui_eval(Object x);

    // introduction string when the module is loaded
    abstract public String introduce();
}
