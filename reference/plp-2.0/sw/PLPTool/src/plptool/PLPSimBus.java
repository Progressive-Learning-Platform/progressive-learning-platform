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

/**
 *
 * @author wira
 */
public class PLPSimBus {
    private PLPSimBusModule[] bus_modules;

    public PLPSimBus(PLPMIPSSim sim) {
        // modules attached to the front side bus
        bus_modules = new PLPSimBusModule[3];

        bus_modules[0] = sim.memory;
        bus_modules[1] = sim.example_io;
        bus_modules[2] = new io_example_external((long) 0x8000800 << 4);

        PLPMsg.M("Built-in modules:");
        for(int i = 0; i < bus_modules.length; i++)
            PLPMsg.M(" " + i + ": " + bus_modules[i].introduce());
    }

    public long read(long addr) {
        for(int i = bus_modules.length - 1; i >= 0; i--) {
            if(addr >= bus_modules[i].startAddr() &&
               addr <= bus_modules[i].endAddr())
                return bus_modules[i].read(addr);
        }

        return -1;
    }

    public int write(long addr, long data, boolean isInstr) {
        boolean noMapping = true;

        for(int i = bus_modules.length - 1; i >= 0; i--) {
            if(addr >= bus_modules[i].startAddr() &&
               addr <= bus_modules[i].endAddr()) {
                bus_modules[i].write(addr, data, isInstr);
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

        for(int i = 0; i < bus_modules.length; i++)
            ret += bus_modules[i].eval();

        return ret;
    }

    /**
     * Enable all I/O mods.
     *
     * @return Returns 0 on completion.
     */
    public int enableiomods() {
        for(int i = 0; i < bus_modules.length; i++)
            bus_modules[i].enable();

        return PLPMsg.PLP_OK;
    }

    /**
     * Disable all I/O mods. Including the main memory. Be careful.
     *
     * @return Returns 0 on completion.
     */
    public int disableiomods() {
        for(int i = 0; i < bus_modules.length; i++)
            bus_modules[i].disable();

        return PLPMsg.PLP_OK;
    }

    public boolean enabled(int index) {
        try {
        return bus_modules[index].enabled();

        } catch(Exception e) {
            PLPMsg.E("enabled(): error.",
                      PLPMsg.PLP_SIM_BUS_ERROR, this);

            return false;
        }
    }

    public int enableio(int index) {
        try {
        bus_modules[index].enable();
        return PLPMsg.PLP_OK;

        } catch(Exception e) {
            return PLPMsg.E("enableio(): error.",
                            PLPMsg.PLP_SIM_BUS_ERROR, this);
        }
    }

    public int disableio(int index) {
        try {
        bus_modules[index].disable();
        return PLPMsg.PLP_OK;

        } catch(Exception e) {
            return PLPMsg.E("disableio(): error.",
                            PLPMsg.PLP_SIM_BUS_ERROR, this);
        }
    }

    public String introduceio(int index) {
        try {

        return bus_modules[index].introduce();

        } catch(Exception e) {
            PLPMsg.E("introduceio(): error.",
                     PLPMsg.PLP_SIM_BUS_ERROR, this);
            return null;
        }
    }

    public int nummods() {
        return bus_modules.length;
    }
}
