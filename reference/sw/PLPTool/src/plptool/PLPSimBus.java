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

package plptool;

import plptool.dmf.DynamicModuleFramework;
import plptool.dmf.CallbackRegistry;
import plptool.gui.BusEvent;
import plptool.mods.MemModule;
import java.util.ArrayList;

/**
 * This class implements the simulation core bus / front-side bus. Simulator
 * users can attach I/O module classes that extend the PLPSimBusModule
 * abstract class. This object is ideally used by the simulation core during
 * memory access. For example, the PLPTool MIPS simulation core attaches
 * its memory module to the bus, and performs bus write and read during
 * the MEM stage of the pipeline. All modules attached to the bus can 
 * be accessed through this class' methods, or directly using the getModRef
 * function.
 *
 * @see PLPSimCore
 * @see plpmips.PLPMIPSSim
 * @see PLPSimBusModule
 * @see plptool.mods.MemModule
 * @see getModRef(int)
 * @author wira
 */
public class PLPSimBus {
    /**
     * The modules attached to the bus.
     */
    private ArrayList<PLPSimBusModule> bus_modules;

    /**
     * The default constructor for the class will start with a bus with
     * nothing attached to it. This may be preferable for CPUs that do not
     * use this bus to write to main memory.
     *
     * @see PLPSimBus(PLPSimCore)
     */
    public PLPSimBus() {
        bus_modules = new ArrayList<PLPSimBusModule>();
    }

    /**
     * This constructor will attach the provided memory module to the bus.
     * Can be used with simulator cores with main memory attached to the bus.
     *
     * @param PLPSimCore The simulator core
     * @see PLPSimBus()
     */
    public PLPSimBus(MemModule mem) {
        bus_modules = new ArrayList<PLPSimBusModule>();
        bus_modules.add(mem);
    }

    /**
     * Attach a module to the bus.
     *
     * @param module PLPSimBusModule-based class to be added
     * @return Index of the new added module, or -1 if error occurred
     */
    public int add(PLPSimBusModule module) {
        boolean ret = bus_modules.add(module);

        if(ret) {
            int index = bus_modules.indexOf(module);
            Object[] params = {module, index};
            CallbackRegistry.callback(CallbackRegistry.BUS_ADD, params);
            return index;
        } else
            return Msg.E("Failed to attach module " + module,
                            Constants.PLP_ERROR_RETURN, this);
    }

    /**
     * Remove a module specified by the index from this bus.
     *
     * @param index
     * @return Always returns PLP_OK
     */
    public int remove(int index) {
        Object[] params = {bus_modules.get(index), index};
        CallbackRegistry.callback(CallbackRegistry.BUS_REMOVE, params);
        bus_modules.remove(index);

        return Constants.PLP_OK;
    }

    /**
     * Issue a read to the bus. This will return a value from specified address
     * starting with the module with lowest index (main memory preferably).
     * If there's any address space overlap, the module with lower index will
     * be read, but all modules do get the read command.
     *
     * @param addr Address to read from
     * @return Data with successful read, -1 otherwise
     */
    public synchronized Object read(long addr) {
        CallbackRegistry.callback(CallbackRegistry.BUS_READ, addr);
        Object value = null;
        PLPSimBusModule module;
        for(int i = bus_modules.size() - 1; i >= 0; i--) {
            module = bus_modules.get(i);
            if(addr >= module.startAddr() &&
               addr <= module.endAddr()) {
                if(!module.phantom) {                    
                    value = module.read(addr);
                } else
                    module.read(addr);
            }
        }

        if(value != null) {
            Object[] params = {addr, value};
            CallbackRegistry.callback(CallbackRegistry.BUS_POST_READ, params);
            return value;  
        }

        else if(value == null && this.isMapped(addr))
            Msg.E("read(" + String.format("0x%08x", addr) + "):" +
                 " Mapped module returned no data, check previous error.",
                 Constants.PLP_SIM_MODULE_NO_DATA_ON_READ, this);
        else if(!this.isMapped(addr))
            Msg.E("read(" + String.format("0x%08x", addr) + "):" +
                 " This address is not in any module's address space.",
                 Constants.PLP_SIM_UNMAPPED_MEMORY_ACCESS, this);

        return null;
    }

    /**
     * Issue a write to the bus. The write will be issued to all modules
     * if there's an address overlap.
     *
     * @param addr Address to write to
     * @param data Data to write
     * @param isInstr Specify whether the data is instruction or not
     * @return PLP_OK on successful operation, error code otherwise
     */
    public synchronized int write(long addr, Object data, boolean isInstr) {
        Object[] params = {addr, data, isInstr};
        CallbackRegistry.callback(CallbackRegistry.BUS_WRITE, params);
        Msg.D("Writing " + String.format("0x%08x", ((Long) data)) + " to " + String.format("0x%08x", addr), 5, this);
        boolean noMapping = true;
        int ret = Constants.PLP_OK;
        PLPSimBusModule module;
        for(int i = bus_modules.size() - 1; i >= 0; i--) {
            module = bus_modules.get(i);
            if(addr >= module.startAddr() &&
               addr <= module.endAddr()) {
                ret = module.write(addr, data, isInstr);
                noMapping = false;
            }
        }

        if(noMapping)
            return Msg.E("write(" + String.format("0x%08x", addr) + "):" +
                            " This address is not in any module's address space.",
                            Constants.PLP_SIM_UNMAPPED_MEMORY_ACCESS, this);
        else
            return ret;
    }
    
    /**
     * Check if the specified address is mapped to a module AND it is
     * initialized.
     *
     * @param addr Address to check
     * @return true if the specified address is valid and initialized
     */
    public synchronized boolean isInitialized(long addr) {
        PLPSimBusModule module;
        for(int i = bus_modules.size() - 1; i >= 0; i--) {
            module = bus_modules.get(i);
            if(addr >= module.startAddr() &&
               addr <= module.endAddr() &&
               module.isInitialized(addr))
               return true;
        }

        return false;
    }

    /**
     * Check if the specified address is mapped
     *
     * @param addr Address to check
     * @return true if the specified address is valid
     */
    public synchronized boolean isMapped(long addr) {
        PLPSimBusModule module;
        for(int i = bus_modules.size() - 1; i >= 0; i--) {
            module = bus_modules.get(i);
            if(addr >= module.startAddr() &&
               addr <= module.endAddr() &&
               module.checkAlignment(addr))
               return true;
        }

        return false;
    }

    /**
     * Check if the specified address is mapped and is holding an instruction
     *
     * @param addr Address to check
     * @return true if the specified address contains an instruction
     */
    public synchronized boolean isInstr(long addr) {
        PLPSimBusModule module;
        for(int i = bus_modules.size() - 1; i >= 0; i--) {
            module = bus_modules.get(i);
            if(addr >= module.startAddr() &&
               addr <= module.endAddr() &&
               module.isInstr(addr))
               return true;
        }

        return false;
    }

    /**
     * Evaluate all modules attached to the bus.
     *
     * @return PLP_OK, or error code
     */
    public synchronized int eval() {
        int ret = Constants.PLP_OK;
        CallbackRegistry.callback(CallbackRegistry.BUS_EVAL, null);
        for(int i = 0; i < bus_modules.size(); i++)
            ret += bus_modules.get(i).eval();

        return ret;
    }

    /**
     * Evaluate module that has the specified index
     *
     * @param index Index of the module
     * @return PLP_OK, or error code
     */
    public int eval(int index) {
        try {
            return bus_modules.get(index).eval();
        } catch(Exception e) {
            return Msg.E("eval(" + index + "): error: " + e,
                            Constants.PLP_SIM_BUS_ERROR, this);
        }
    }

    /**
     * Execute a GUI evaluation on the module that has the specified index
     *
     * @param index Index of the module
     * @param x Object to draw to, left to the module to implement
     * @return PLP_OK, or error code
     */
    public int gui_eval(int index, Object x) {
        try {
        if(!bus_modules.get(index).threaded)
            return bus_modules.get(index).gui_eval(x);
        else {
            bus_modules.get(index).notify();
            return Constants.PLP_OK;
        }

        } catch(Exception e) {
            return Msg.E("eval(" + index + "): error: " + e,
                            Constants.PLP_SIM_BUS_ERROR, this);
        }
    }

    /**
     * Enable all I/O mods.
     *
     * @return Returns 0 on completion.
     */
    public synchronized int enableAllModules() {
        for(int i = 0; i < bus_modules.size(); i++)
            bus_modules.get(i).enable();

        return Constants.PLP_OK;
    }

    /**
     * Disable all I/O mods. Including the main memory if it is attached to
     * the bus. Be careful.
     *
     * @return Returns 0 on completion.
     */
    public synchronized int disableAllModules() {
        for(int i = 0; i < bus_modules.size(); i++)
            bus_modules.get(i).disable();

        return Constants.PLP_OK;
    }

    /**
     * Return whether the specified module is enabled or not.
     *
     * @param index Index of the module
     * @return Boolean on whether the module is enabled
     */
    public synchronized boolean getEnabled(int index) {
        try {
            return bus_modules.get(index).enabled();

        } catch(Exception e) {
            Msg.E("enabled(" + index + "): error: " + e,
                     Constants.PLP_SIM_BUS_ERROR, this);

            return false;
        }
    }

    /**
     * Enable specified module, allowing evaluations and writes
     *
     * @param index Index of the module
     * @return PLP_OK, or error code
     */
    public synchronized int enableMod(int index) {        
        try {
            bus_modules.get(index).enable();
        return Constants.PLP_OK;

        } catch(Exception e) {
            return Msg.E("enableio(" + index + "): error: " + e,
                            Constants.PLP_SIM_BUS_ERROR, this);
        }
    }

    /**
     * Disable specified module, disallowing evaluations and writes
     *
     * @param index Index of the module
     * @return PLP_OK, or error code
     */
    public synchronized int disableMod(int index) {
        try {
            bus_modules.get(index).disable();
        return Constants.PLP_OK;

        } catch(Exception e) {
            return Msg.E("disableio(" + index + "): error: " + e,
                            Constants.PLP_SIM_BUS_ERROR, this);
        }
    }

    /**
     * Clear saved values of specified module. This renders all registers
     * of the module to be marked uninitialized
     *
     * @param index Index of the module
     * @return PLP_OK, or error code
     */
    public int clearModRegisters(int index) {
        try {
            bus_modules.get(index).clear();
        return Constants.PLP_OK;

        } catch(Exception e) {
            return Msg.E("cleario(" + index + "): error: " + e,
                            Constants.PLP_SIM_BUS_ERROR, this);
        }
    }

    /**
     * Return the information string for the specified module.
     *
     * @param index Index of the module
     * @return String of introduction
     */
    public String introduceMod(int index) {
        try {
            return bus_modules.get(index).introduce();
        } catch(Exception e) {
            Msg.E("introduceio(" + index + "): error: " + e,
                     Constants.PLP_SIM_BUS_ERROR, this);

            return null;
        }
    }

    /**
     * Return the starting address of the specified module.
     *
     * @param index Index of the module
     * @return Starting address in (long)
     */
    public synchronized long getModStartAddress(int index) {
        try {
            return bus_modules.get(index).startAddr();
        } catch(Exception e) {
            Msg.E("iostartaddr(" + index + "): error: " + e,
                     Constants.PLP_SIM_BUS_ERROR, this);

            return Constants.PLP_ERROR_RETURN;
        }
    }

    /**
     * Return the ending address of the specified module
     *
     * @param index Index of the module
     * @return Ending address in (long)
     */
    public synchronized long getModEndAddress(int index) {
        try {
            return bus_modules.get(index).endAddr();
        } catch(Exception e) {
            Msg.E("ioendaddr(" + index + "): error: " + e,
                     Constants.PLP_SIM_BUS_ERROR, this);

            return Constants.PLP_ERROR_RETURN;
        }
    }

    /**
     * Return number of modules attached to this bus
     *
     * @return Number of modules in (int)
     */
    public synchronized int getNumOfMods() {
        return bus_modules.size();
    }

    /**
     * Return a reference to the requested module for direct access.
     *
     * @param index Index of the module
     * @return PLPSimBusModule reference of the module
     */
    public synchronized PLPSimBusModule getRefMod(int index) {
        try {
            return bus_modules.get(index);
        } catch(Exception e) {
            Msg.E("getRefMod(" + index + "): error: " + e,
                     Constants.PLP_SIM_BUS_ERROR, this);

            return null;
        }
    }

    /**
     * Issue bus.write(0) on all registers of the specified module
     *
     * @param index Index of the module
     */
    public synchronized void issueZeroes(int index) {
        PLPSimBusModule module = bus_modules.get(index);

        for(int i = 0; i < module.size(); i++) {
            this.write(module.startAddr() + i * (module.isWordAligned() ? 4 : 1), new Long(0), false);
        }
    }

    /**
     * Call reset on all modules attached to this bus.
     */
    public void reset() {
        for(int i = 0; i < bus_modules.size(); i++) {
            bus_modules.get(i).reset();
        }
    }

    /**
     * Like read(), but ignores errors
     *
     * @param addr Address to read from
     * @return Data with successful read, -1 otherwise
     */
    public synchronized Object uncheckedRead(long addr) {
        Object value = null;
        PLPSimBusModule module;
        for(int i = bus_modules.size() - 1; i >= 0; i--) {
            module = bus_modules.get(i);
            if(addr >= module.startAddr() &&
               addr <= module.endAddr())
                value = module.read(addr);
        }

        if(value != null)
            return value;

        return null;
    }

    @Override public String toString() {
        return "PLPSimBus";
    }
}
