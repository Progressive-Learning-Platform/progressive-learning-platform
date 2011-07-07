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
     * Set interrupt flag
     */
    public void setIRQ(long IRQ) {
        this.IRQ = IRQ;
    }
}
