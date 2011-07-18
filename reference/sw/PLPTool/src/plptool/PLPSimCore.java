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
import java.util.ArrayList;

/**
 * This abstract class defines the simulated CPU interface.
 *
 * @see PLPMIPSSim
 * @author wira
 */
public abstract class PLPSimCore {

    /**
     * Start address for reset.
     */
    protected long startAddr;

    /**
     * Simulation flags. This flag is used by various modules to indicate
     * whether certain events have occured.
     */
    protected Long sim_flags;

    /**
     * Numbers of instructions issued.
     */
    protected int instructionCount;

    /**
     * External interrupt
     */
    protected long IRQ;

    /**
     * CPU front-side bus.
     */
    public PLPSimBus bus;

    /**
     * Breakpoints
     */
    public mod_breakpoint breakpoints;

    /**
     * Reset handler, to be implemented by the actual simulation core
     */
    abstract public int reset();

    /**
     * Step handler, to be implemented by the actual simulation core.
     */
    abstract public int step();

    /**
     * Simulator application will call this program when it instantiates
     * a simulation core.
     */
    abstract public int loadProgram(PLPAsm asm);

    /**
     * Returns simulation flags.
     *
     * @return Returns the simulation flags.
     */
    public long getFlags() {
        return sim_flags;
    }

    /**
     * Get the number of instructions that have been issued on this core.
     *
     * @return Returns instruction count in int.
     */
    public int getinstrcount() {
        return instructionCount;
    }

    /**
     * Set starting address
     */
    public void setStartAddr(long addr) {
        startAddr = addr;
    }

    /**
     * Set interrupt bit (mask)
     */
    public void setIRQ(long IRQ) {
        this.IRQ |= IRQ;
    }

    /**
     * Mask interrupt bit
     */
    public void maskIRQ(long IRQ) {
        this.IRQ &= IRQ;
    }

    /**
     * Get interrupt bits that are currently set
     */
    public long getIRQ() {
        return IRQ;
    }

    /**
     * The simulation runner will check on this address to evaluate breakpoints
     */
    public long visibleAddr;

    /**
     * Breakpoint module for the simulation
     */
    public class mod_breakpoint {
        private ArrayList<Long> addresses;

        public mod_breakpoint() {
            addresses = new ArrayList<Long>();
        }

        public void add(long addr) {
            addresses.add(addr);
        }

        public boolean remove(long addr) {
            boolean ret = false;

            for(int i = 0; i < addresses.size(); i++) {
                if((long) addresses.get(i) == addr) {
                    addresses.remove(i);
                    ret = true;
                }
            }

            return ret;
        }

        public boolean has_breakpoint() {
            return addresses.size() > 0 ? true : false;
        }

        public boolean is_breakpoint(long addr) {
            for(int i = 0; i < addresses.size(); i++) {
                if((long) addresses.get(i) == addr) {
                    return true;
                }
            }

            return false;
        }

    }
}
