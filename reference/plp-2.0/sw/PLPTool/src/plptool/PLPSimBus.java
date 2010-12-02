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

import java.util.ArrayList;

/**
 *
 * @author wira
 */
public class PLPSimBus {
    private ArrayList<PLPSimBusModule> bus_modules;

    public PLPSimBus(PLPMIPSSim sim) {
        // modules attached to the front side bus
        bus_modules = new ArrayList<PLPSimBusModule>();

        bus_modules.add(sim.memory);
    }

    public int add(PLPSimBusModule module) {
        boolean ret = bus_modules.add(module);

        if(ret)
            return PLPMsg.PLP_OK;
        else
            return PLPMsg.E("Failed to attach module " + module,
                            PLPMsg.PLP_SIM_BUS_ERROR, this);
    }

    public int remove(int index) {
        bus_modules.remove(index);

        return PLPMsg.PLP_OK;
    }

    public long read(long addr) {
        Object[] modules = bus_modules.toArray();
        for(int i = modules.length - 1; i >= 0; i--) {
            if(addr >= ((PLPSimBusModule)modules[i]).startAddr() &&
               addr <= ((PLPSimBusModule)modules[i]).endAddr())
                return ((PLPSimBusModule)modules[i]).read(addr);
        }

        return -1;
    }

    public int write(long addr, long data, boolean isInstr) {
        boolean noMapping = true;
        Object[] modules = bus_modules.toArray();
        for(int i = modules.length - 1; i >= 0; i--) {
            if(addr >= ((PLPSimBusModule)modules[i]).startAddr() &&
               addr <= ((PLPSimBusModule)modules[i]).endAddr()) {
                ((PLPSimBusModule)modules[i]).write(addr, data, isInstr);
                noMapping = false;
            }
        }

        if(noMapping)
            return PLPMsg.PLP_SIM_UNINITIALIZED_MEMORY;
        else
            return PLPMsg.PLP_OK;
    }

    public int eval() {
        int ret = PLPMsg.PLP_OK;
        Object[] modules = bus_modules.toArray();
        for(int i = 0; i < modules.length; i++)
            ret += ((PLPSimBusModule)modules[i]).eval();

        return ret;
    }

    public int eval(int index) {
        Object[] modules = bus_modules.toArray();
        try {
        return ((PLPSimBusModule)modules[index]).eval();

        } catch(Exception e) {
            return PLPMsg.E("eval(" + index + "): error: " + e,
                            PLPMsg.PLP_SIM_BUS_ERROR, this);
        }
    }

    /**
     * Enable all I/O mods.
     *
     * @return Returns 0 on completion.
     */
    public int enableiomods() {
        Object[] modules =bus_modules.toArray();

        for(int i = 0; i < modules.length; i++)
            ((PLPSimBusModule)modules[i]).enable();

        return PLPMsg.PLP_OK;
    }

    /**
     * Disable all I/O mods. Including the main memory. Be careful.
     *
     * @return Returns 0 on completion.
     */
    public int disableiomods() {
        Object[] modules = bus_modules.toArray();

        for(int i = 0; i < modules.length; i++)
            ((PLPSimBusModule)modules[i]).disable();

        return PLPMsg.PLP_OK;
    }

    public boolean enabled(int index) {
        Object[] modules = bus_modules.toArray();

        try {
        return ((PLPSimBusModule)modules[index]).enabled();

        } catch(Exception e) {
            PLPMsg.E("enabled(" + index + "): error: " + e,
                     PLPMsg.PLP_SIM_BUS_ERROR, this);

            return false;
        }
    }

    public int enableio(int index) {
        Object[] modules = bus_modules.toArray();

        try {
        ((PLPSimBusModule)modules[index]).enable();
        return PLPMsg.PLP_OK;

        } catch(Exception e) {
            return PLPMsg.E("enableio(" + index + "): error: " + e,
                            PLPMsg.PLP_SIM_BUS_ERROR, this);
        }
    }

    public int disableio(int index) {
        Object[] modules = bus_modules.toArray();

        try {
        ((PLPSimBusModule)modules[index]).disable();
        return PLPMsg.PLP_OK;

        } catch(Exception e) {
            return PLPMsg.E("disableio(" + index + "): error: " + e,
                            PLPMsg.PLP_SIM_BUS_ERROR, this);
        }
    }

    public String introduceio(int index) {
        Object[] modules = bus_modules.toArray();
        try {

        return ((PLPSimBusModule)modules[index]).introduce();

        } catch(Exception e) {
            PLPMsg.E("introduceio(" + index + "): error: " + e,
                     PLPMsg.PLP_SIM_BUS_ERROR, this);

            return null;
        }
    }

    public int nummods() {
        Object[] modules = bus_modules.toArray();

        return modules.length;
    }

    @Override public String toString() {
        return "PLPSimBus";
    }
}
