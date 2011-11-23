/*
    Copyright 2011 David Fritz, Brian Gordon, Wira Mulia

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

import plptool.gui.ProjectDriver;

/**
 * This abstract class serves as a skeleton for the ISA's metaclass. The
 * ProjectDriver class depends on the extension of this class to perform
 * ISA specific actions such as creating an assembler, simulation routines,
 * etc. See plptool.mips.Architecture for an example implementation. This
 * class hass access to the currently active ProjectDriver instance 'plp'
 *
 * @author wira
 */
public abstract class PLPArchitecture {
    private String identifier;
    protected ProjectDriver plp;

    protected int     archID;
    protected boolean hasAssembler = false;
    protected boolean hasSimCore = false;
    protected boolean hasSimCoreGUI = false;
    protected boolean hasProgrammer = false;

    public PLPArchitecture(int archID, String identifier, ProjectDriver plp) {
        this.identifier = identifier;
        this.plp = plp;
        this.archID = archID;
    }

    /**
     * Set a new ProjectDriver for this ISA
     *
     * @param plp New ProjectDriver instance to use
     */
    public void setProjectDriver(ProjectDriver plp) {
        this.plp = plp;
    }

    /**
     * Get ISA ID
     *
     * @return ISA ID specified by the registry
     */
    public int getID() {
        return archID;
    }

    /**
     * Does this ISA implement the assembler class
     *
     * @return boolean specifying whether the ISA implements the assembler
     */
    public boolean hasAssembler() {
        return hasAssembler;
    }

    /**
     * Does this ISA implement the simulation core
     *
     * @return boolean specifying whether the ISA implements the simulation core
     */
    public boolean hasSimCore() {
        return hasSimCore;
    }

    /**
     * Does this ISA implement the simulation core GUI frame
     *
     * @return boolean specifying whether the ISA implements the simulation core
     *         GUI frame
     */
    public boolean hasSimCoreGUI() {
        return hasSimCoreGUI;
    }

    /**
     * Does this ISA implement the programmer class
     *
     * @return boolean specifying whether the ISA implements the board
     *         programmer class
     */
    public boolean hasProgrammer() {
        return hasProgrammer;
    }

    /**
     * Return a new instance of the ISA assembler.
     *
     * @return Asm instance of the ISA
     */
    abstract public PLPAsm createAssembler();

    /**
     * Return a new instance of the simulation core.
     *
     * @return SimCore instance of the ISA
     */
    abstract public PLPSimCore createSimCore();

    /**
     * Additional simulation initialization code. Called by the ProjectDriver
     * AFTER the I/O registry has been initialized but before I/O module
     * presets are loaded to the simulation
     *
     */
    abstract public void simulatorInitialization();

    /**
     * Additional simulation initialization code called after all simulation
     * elements are initialized
     */
    abstract public void simulatorInitializationFinal();

    /**
     * Additional code after simulation is stopped. Called by the ProjectDriver
     * immediately after the project exits simulation mode
     */
    abstract public void simulatorStop();

    /**
     * Launch a CLI for the simulator (if one is implemented)
     */
    abstract public void launchSimulatorCLI();

    /**
     * Return a new instance of the simulation core frame.
     *
     * @return SimCoreGUI instance of the ISA
     */
    abstract public PLPSimCoreGUI createSimCoreGUI();

    /**
     * This method returns a new instance of the serial programmer
     *
     * @return SerialProgrammer instance of the ISA
     */
    abstract public PLPSerialProgrammer createProgrammer();

    /**
     * Return a QuickReference string for the IDE
     *
     * @return QuickReference string to display (HTML formatted)
     */
    abstract public String getQuickReferenceString();

    /**
     * Save architecture-specific simulation configuration to PLP file
     *
     * @return Additional configuration string to save to plp.simconfig
     */
    abstract public String saveArchSpecificSimStates();

    /**
     * Set architecture-specific simulation configuration to the current
     * open project driver. ProjectDriver.open() will pass an array of String
     * tokens split with the string "::" as the delimiter. For example,
     * the line in config file "foo::bar" will be passed on to this function
     * as {"foo", "bar"} in configStr
     *
     * @param configStr Configuration string saved in plp.simconfig
     */
    abstract public void restoreArchSpecificSimStates(String[] configStr);

    public boolean equals(String str) {
        return identifier.equals(str);
    }
}
