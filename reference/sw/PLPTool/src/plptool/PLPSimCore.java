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
    public int getInstrCount() {
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
        Msg.D("setIRQ: New IRQ is " + String.format("0x%08x", IRQ), 5, this);
    }

    /**
     * Mask interrupt bit
     */
    public void maskIRQ(long IRQ) {
        this.IRQ &= IRQ;
        Msg.D("maskIRQ: New IRQ is " + String.format("0x%08x", IRQ), 5, this);
    }

    /**
     * Get interrupt bits that are currently set
     */
    public long getIRQ() {
        Msg.D("getIRQ: " + String.format("0x%08x", IRQ), 5, this);
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
        private ArrayList<breakpoint> breakpoints;

        public mod_breakpoint() {
            breakpoints = new ArrayList<breakpoint>();
        }

        public void add(long addr, int fileMap, int lineNumMap) {
            breakpoints.add(new breakpoint(addr, fileMap, lineNumMap));
        }

        public boolean remove(long addr) {
            boolean ret = false;

            for(int i = 0; i < breakpoints.size(); i++) {
                if((long) breakpoints.get(i).getAddr() == addr) {
                    breakpoints.remove(i);
                    ret = true;
                }
            }

            return ret;
        }

        public void clear() {
            breakpoints = new ArrayList<breakpoint>();
        }

        public boolean hasBreakpoint() {
            return breakpoints.size() > 0 ? true : false;
        }

        public boolean isBreakpoint(long addr) {
            for(int i = 0; i < breakpoints.size(); i++) {
                if((long) breakpoints.get(i).getAddr() == addr) {
                    return true;
                }
            }

            return false;
        }

        public boolean isBreakpoint(int fIndex, int lineNum) {
            for(int i = 0; i < breakpoints.size(); i++) {
                if((breakpoints.get(i).getFileMap() == fIndex) &&
                   (breakpoints.get(i).getLineNumMap() == lineNum)) {
                    return true;
                }
            }

            return false;
        }
    }

    class breakpoint {
        private long addr;
        private int fileMap;
        private int lineNumMap;

        public breakpoint(long addr, int fileMap, int lineNumMap) {
            this.addr = addr;
            this.fileMap = fileMap;
            this.lineNumMap = lineNumMap;
        }

        public long getAddr() {
            return addr;
        }

        public int getFileMap() {
            return fileMap;
        }

        public int getLineNumMap() {
            return lineNumMap;
        }
    }
}
