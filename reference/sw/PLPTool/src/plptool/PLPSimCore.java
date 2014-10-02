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
import plptool.dmf.CallbackRegistry;

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
     * PLPSimCore default constructor initializes required data structures
     */
    public PLPSimCore() {
        bus = new PLPSimBus();
        breakpoints = new mod_breakpoint();
        visibleAddr = -1;
        instructionCount = 0;
        sim_flags = 0L;
        IRQ = 0L;
    }

    /**
     * PLPSimCore defaults to returning the current version of PLPTool.
     * User simcores can override this. May be useful for dynamic modules
     *
     * @return String of PLPTool version
     */
    public String getVersion() { return Text.versionString; }

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
     * Wrapper for the step handler. This is the actual method called by the
     * framework
     *
     * @return
     */
    public int stepW() {
        int ret;
        CallbackRegistry.callback(CallbackRegistry.SIM_STEP, null);
        ret = step();
        CallbackRegistry.callback(CallbackRegistry.SIM_POST_STEP, null);
        return ret;
    }

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
     * Overridable developer-specified generic hook.
     *
     * @param param An object to pass to the hook
     * @return A reference to an object returned from the hook function
     */
    public Object hook(Object param) {return null;};

    /**
     * Breakpoint module for the simulation
     */
    public class mod_breakpoint {
        private ArrayList<Breakpoint> breakpoints;

        /**
         * The Breakpoint constructor creates a new arraylist of breakpoints
         */
        public mod_breakpoint() {
            breakpoints = new ArrayList<Breakpoint>();
        }

        /**
         * Add a Breakpoint to the simulation core
         *
         * @param addr The address of the program to break on
         * @param fileMap File mapper for GUI purposes
         * @param lineNumMap File number mapper for GUI purposes
         */
        public void add(long addr, int fileMap, int lineNumMap) {
            breakpoints.add(new Breakpoint(addr, fileMap, lineNumMap));
        }

        /**
         * Remove a Breakpoint specified by the address
         *
         * @param addr The address of the Breakpoint to remove
         * @return True if the Breakpoint exists, false otherwise
         */
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

        /**
         * Clear breakpoints
         */
        public void clear() {
            breakpoints = new ArrayList<Breakpoint>();
        }

        /**
         * Return whether the simulation core has any Breakpoint installed
         *
         * @return True if the simulation core has breakpoints, false otherwise
         */
        public boolean hasBreakpoint() {
            return breakpoints.size() > 0 ? true : false;
        }

        /**
         * Check if the specified address is a Breakpoint
         *
         * @param addr Address to check
         * @return True if the address is a Breakpoint, false otherwise
         */
        public boolean isBreakpoint(long addr) {
            for(int i = 0; i < breakpoints.size(); i++) {
                if((long) breakpoints.get(i).getAddr() == addr) {
                    return true;
                }
            }

            return false;
        }

        /**
         * Check if the specified file and its line number is a Breakpoint
         *
         * @param fIndex The file mapper index for the file in question
         * @param lineNum The line number of the code
         * @return True if the location is a Breakpoint, false otherwise
         */
        public boolean isBreakpoint(int fIndex, int lineNum) {
            for(int i = 0; i < breakpoints.size(); i++) {
                if((breakpoints.get(i).getFileMap() == fIndex) &&
                   (breakpoints.get(i).getLineNumMap() == lineNum)) {
                    return true;
                }
            }

            return false;
        }

        /**
         * Return the number of breakpoints installed on this simulation core
         *
         * @return Number of breakpoints
         */
        public int size() {
            return breakpoints.size();
        }

        /**
         * Get the breakpoint specified by index
         *
         * @param index Index of the breakpoint
         * @return Breakpoint reference, or null if the index is out of bounds
         */
        public long getBreakpointAddress(int index) {
            if(index < 0 || index > breakpoints.size())
                return Constants.PLP_NUMBER_ERROR;

            return breakpoints.get(index).getAddr();
        }

        /**
         * Remove breakpoint specified by index
         *
         * @param index Index of breakpoint to be removed
         * @return boolean True if successful, false otherwise
         */
        public boolean removeBreakpoint(int index) {
            if(index < 0 || index > breakpoints.size())
                return false;

            breakpoints.remove(index);
            return true;
        }
    }

    class Breakpoint {
        private long addr;
        private int fileMap;
        private int lineNumMap;

        public Breakpoint(long addr, int fileMap, int lineNumMap) {
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
