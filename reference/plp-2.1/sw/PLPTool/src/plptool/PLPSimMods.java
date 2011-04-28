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

import plptool.mods.MemModule;
import plptool.mips.SimCore;

/**
 * PLP first-party modules collection. See PLPSimCL for an example on how
 * these modules can be utilized.
 *
 * @see PLPSimBusModule
 * @see PLPSimBus
 * @see PLPSimCL
 * @see PLPSimCore
 * @see PLPMIPSSim
 * @see mod_io_leds
 * @see mod_cache_hier
 * @see mod_mips_sniffer
 * @author wira
 */
public class PLPSimMods {
    public mod_io_leds                      io_leds;
    public mod_cache_hier                   cache_hier;
    public mod_mips_sniffer                 mips_sniffer;

    /**
     * The constructor of PLPSimMods is where all the first-party modules
     * here initialized. Register address and parameters are assigned here.
     *
     * @param sim Developers can selectively expose different parts of the
     * simulator to the modules by utilizing this reference.
     * @see PLPSimCore
     */
    public PLPSimMods(PLPSimCore sim) {
        io_leds = new mod_io_leds((long) 0x8000400 << 4 | 4);
        cache_hier = new mod_cache_hier(sim.memory);
        mips_sniffer = new mod_mips_sniffer((SimCore) sim, (long) 0x9000800 << 4);
    }

/******************************************************************************/

    /**
     * This is an example module written for the PLP simulator that interacts
     * with the simulation core solely using the front side bus.
     *
     * TODO: add gui_eval when the simulator GUI is ready for this stage of development.
     */
    public class mod_io_leds extends PLPSimBusModule {

        public mod_io_leds(long addr) {
            super(addr, addr, true); // This I/O only has 1 register
            super.write(addr, new Long(0), false);
        }

        // When this module is evaluated, do this:
        public int eval() {
            if(!enabled)
                return Constants.PLP_OK;

            // Get register value
            if(!isInitialized(startAddr))
                return Constants.PLP_SIM_BUS_ERROR;

            long value = (Long) super.read(startAddr);

            PLPMsg.m(this + ": ");

            // Combinational logic
            for(int i = 7; i >= 0; i--) {
                if((value & (long) Math.pow(2, i)) == (long) Math.pow(2, i))
                    PLPMsg.m("* ");
                else
                    PLPMsg.m(". ");
            }

            PLPMsg.M("");

            return Constants.PLP_OK;
        }

        public int gui_eval(Object x) {
            return Constants.PLP_OK;
        }

        public String introduce() {
            return "PLPTool 2.0 io_leds: 8-LED array";
        }

        // Make sure to do this for error tracking
        @Override public String toString() {
            return "io_leds";
        }
    }

/******************************************************************************/
     
    /**
     * This is an example of a module that snoops the front-side bus. By
     * making its address space equal to the address space of the main memory,
     * it will see all memory transactions issued by the core. This allows
     * the module to simulate a cache memory hierarchy.
     */
    public class mod_cache_hier extends PLPSimBusModule {
        MemModule L1_I;
        MemModule L1_D;
        MemModule L2;

        public mod_cache_hier(MemModule mem) {
            super(0, mem.size() * 4 - 4, true);
        }

        // When this module is evaluated, do this:
        public int eval() {
            return Constants.PLP_OK;
        }

        public int gui_eval(Object x) {
            return Constants.PLP_OK;
        }

        public String introduce() {
            return "PLPTool 2.0 Cache Hierarchy Module";
        }

        // The read and write methods can be overridden. Do all the fancy
        // cache stuff here.
        @Override public Object read(long addr) {

            return 0;
        }

        @Override public int write(long addr, Object data, boolean isInstr) {

            return 0;
        }

        // Make sure to do this for error tracking
        @Override public String toString() {
            return "cache_hier";
        }
    }

/******************************************************************************/

    /**
     * This is an example of a pervasive module. By passing the simulation
     * core to this module, it has access to everything. It also utilizes
     * PLPSimBusModule basic feature set so the simulation core can interact
     * with this module through the front side bus.
     */
    public class mod_mips_sniffer extends PLPSimBusModule {

        public mod_mips_sniffer(SimCore sim, long regaddr) {
            super(regaddr, regaddr, true);
        }

        // When this module is evaluated, do this:
        public int eval() {
            return Constants.PLP_OK;
        }

        public int gui_eval(Object x) {
            return Constants.PLP_OK;
        }

        public String introduce() {
            return "PLPTool 2.0 MIPS Bus Sniffer";
        }

        // Make sure to do this for error tracking
        @Override public String toString() {
            return "mips_sniffer";
        }
    }
}

