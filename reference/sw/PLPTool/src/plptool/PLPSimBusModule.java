package plptool;

/*
    Copyright 2010-2014 David Fritz, Brian Gordon, Wira Mulia

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
public abstract class PLPSimBusModule extends Thread {
    /**
     * Denotes whether this module should run on its own thread. Defaults
     * to false.
     */
    public boolean threaded = false;
    public boolean stop;

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
     * This variable tells the bus whether it actually stores data in the
     * mapped region. Defaults to false. Useful for modules that need to see
     * bus accesses but do not require registers (cache simulators, snoopers,
     * etc)
     */
    public boolean phantom;

    /**
     * PLPSimBusModule defaults to returning the current version of PLPTool.
     * User modules can override this. May be useful for dynamic modules
     * 
     * @return String of PLPTool version
     */
    public String getVersion() { return Text.versionString; }

    /**
     * The constructor for the superclass requires the address space and
     * whether the registers of the module are word-aligned
     *
     * @param startAddr Starting address of the module's address space
     * @param endAddr End address of the module's address space
     * @param wordAligned Whether the module's address space is word aligned
     */
    public PLPSimBusModule(long startAddr, long endAddr, boolean wordAligned) {
        values = new TreeMap<Long, Object>();
        isInstr = new TreeMap<Long, Boolean>();
        this.startAddr = startAddr;
        this.endAddr = endAddr;
        this.wordAligned = wordAligned;
        enabled = false;
        phantom = false;
    }

    /**
     * Empty/nullary constructor for dynamic module loading
     */
    public PLPSimBusModule() {}

    /**
     * Write data to one of the module's registers. Only possible when the
     * module is enabled. The write command is ignored otherwise.
     *
     * @param addr Address to write to
     * @param data Data to be written
     * @param isInstr Denotes whether the value to be written is an instruction
     * @return PLP_OK, or error code
     */
    public synchronized final int writeReg(long addr, Object data, boolean isInstr) {
        if(addr > endAddr || addr < startAddr)
            return Msg.E("write(" + String.format("0x%08x", addr) + "): Address is out of range.",
                            Constants.PLP_SIM_OUT_ADDRESS_OUT_OF_RANGE, this);
        else if(wordAligned && addr % 4 != 0)
            return Msg.E("write(" + String.format("0x%08x", addr) + "): Requested address is unaligned.",
                            Constants.PLP_SIM_OUT_UNALIGNED_MEMORY, this);
        else if(!enabled)
            return Msg.E("write(" + String.format("0x%08x", addr) + "): Module is disabled.",
                            Constants.PLP_SIM_MODULE_DISABLED, this);
        else {
            if(values.containsKey(addr)) {
                values.remove(addr);
                this.isInstr.remove(addr);
            }
            values.put(new Long(addr), data);
            this.isInstr.put(new Long(addr), isInstr);
        }

        return Constants.PLP_OK;
    }

    /**
     * Read one of the module's registers
     *
     * @param addr Address to read from
     * @return Data, or PLP_ERROR_RETURN
     */
    public synchronized final Object readReg(long addr) {
        if(addr > endAddr || addr < startAddr) {
            Msg.E("read(" + String.format("0x%08x", addr) + "): Address is out of range.",
                     Constants.PLP_SIM_OUT_ADDRESS_OUT_OF_RANGE, this);
            return null;
        }
        else if (wordAligned && addr % 4 != 0) {
            Msg.E("read(" + String.format("0x%08x", addr) + "): Requested address is unaligned.",
                            Constants.PLP_SIM_OUT_UNALIGNED_MEMORY, this);
            return null;
        }
        else if(!values.containsKey(addr)) {
            if(Config.simBusReturnsZeroForUninitRegs) {
                return 0L;
            }
            Msg.E("read(" + String.format("0x%08x", addr) + "): Address is not initialized.",
                             Constants.PLP_SIM_UNINITIALIZED_MEMORY, this);
            return null;
        }
        else
            return values.get(addr);
    }
    
    /**
     * Wrapper function for writeReg. Developers may wish to override this
     * function to implement module-specific write actions without sacrificing
     * PLPSimBusModule original writeReg functionality. This is the actual
     * function that will be called by PLPSimBus.
     * 
     * @param addr Address to write to
     * @param data Data to be written
     * @param isInstr Denotes whether the value to be written is an instruction
     * @return PLP_OK, or error code
     */
    public synchronized int write(long addr, Object data, boolean isInstr) {
        return writeReg(addr, data, isInstr);
    }

    /**
     * Wrapper function for readReg. Developers may wish to override this
     * function to implement module-specific read actions without sacrificing
     * PLPSimBusModule original readReg functionality. This is the actual
     * function that will be called by PLPSimBus.
     *
     * @param addr Address to read from
     * @return Data, or PLP_ERROR_RETURN
     */
    public synchronized Object read(long addr) {
        return readReg(addr);
    }

    /**
     * Reinitialize the module's registers
     */
    public synchronized void clear() {
        values = new TreeMap<Long, Object>();
        isInstr = new TreeMap<Long, Boolean>();
    }

    /**
     * This method can be useful to modules that have to walk through the whole
     * register file on evaluation.
     *
     * @return n x 3 Object array containing addresses, values of the
     * memory module and boolean denoting whether the value is instruction or
     * not.
     */
    public synchronized Object[][] getValueSet() {
        Object[][] valueSet = new Object[values.size()][3];
        int index = 0;

        Iterator keyIterator = values.keySet().iterator();

        while(keyIterator.hasNext()) {
            valueSet[index][0] = keyIterator.next();
            valueSet[index][1] = values.get((Long) valueSet[index][0]);
            valueSet[index][2] = isInstr.get((Long) valueSet[index][0]);
            index++;
        }

        return valueSet;
    }

    /**
     * Return the width of the address space
     *
     * @return The width of the address space in (long)
     */
    public synchronized final long size() {
        if(!wordAligned)
            return endAddr - startAddr + 1;
        else
            return (endAddr - startAddr) / 4 + 1;
    }

    /**
     * Return the end of the address space
     *
     * @return Final address in (long)
     */
    public synchronized final long endAddr() {
        return endAddr;
    }

    /**
     * Return the beginning of the address space
     *
     * @return Starting address in (long)
     */
    public synchronized final long startAddr() {
        return startAddr;
    }

    /**
     * Enable the module.
     */
    public synchronized void enable() {
        enabled = true;
    }

    /**
     * Disable the module.
     */
    public synchronized void disable() {
        enabled = false;
    }

    /**
     * Removal routine
     */
    public synchronized void remove() {

    }

    /**
     * Return whether the module is enabled
     *
     * @return Status of module in (boolean)
     */
    public synchronized boolean enabled() {
        return enabled;
    }

    /**
     * Return whether the specified register contains an instruction or not
     *
     * @param addr Address to read from
     * @return Whether the specified register contains instruction or not
     */
    public synchronized final boolean isInstr(long addr) {
        if(isInstr.get(addr) != null)
            return isInstr.get(addr);
        else
            return false;
    }

    /**
     * Return whether the registers of the module are word-aligned
     *
     * @return Whether the registers of the module are word-aligned
     */
    public synchronized final boolean isWordAligned() {
        return wordAligned;
    }

    /**
     * Check whether the address is properly aligned.
     *
     * @param addr Address to check
     * @return False if alignment check fails, true otherwise
     */
    public synchronized boolean checkAlignment(long addr) {
        if(wordAligned)
            return (addr % 4 == 0);
        else
            return true;
    }

    /**
     * True if the specified address is initialized. False otherwise
     *
     * @param addr The address to lookup
     * @return Returns boolean
     */
    public synchronized final boolean isInitialized(long addr) {
        return values.containsKey(addr);
    }

    /**
     * Reset attributes for dynamically loaded modules
     *
     * @param startAddr Starting address of the module's address space
     * @param endAddr End address of the module's address space
     * @param wordAligned Whether the module's address space is word aligned
     */
    public void setNewParameters(long startAddr, long endAddr, boolean isWordAligned) {
        values = new TreeMap<Long, Object>();
        isInstr = new TreeMap<Long, Boolean>();
        this.startAddr = startAddr;
        this.endAddr = endAddr;
        this.wordAligned = isWordAligned;
    }

    /**
     * The eval() function represents the behavior of the module itself.
     * For example, an eval function for an array of LED will read its
     * register and light up proper LEDs. Simulation cores will call this
     * function every cycle, so this may slow down the simulation
     * considerably.
     *
     * @return Status code.
     */
    abstract public int eval();

    /**
     * gui_eval is designed for simulator developers / users to allow the
     * module to interact with the simulation environment directly. This
     * function is only called every time the GUI components are refreshed.
     * Simulation cores do not/should not call this function every cycle.
     *
     * @param x Reference to a frame object that this module will interact
     * with.
     * @return Status code.
     */
    abstract public int gui_eval(Object x);

    /**
     * This method is called by PLPSimBus.reset(). It is up to the module
     * on how this should be implemented.
     */
    abstract public void reset();

    /**
     * Introduction string when the module is loaded.
     */
    abstract public String introduce();
}
