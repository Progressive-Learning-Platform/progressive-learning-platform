/*
    Copyright 2011-2012 David Fritz, Brian Gordon, Wira Mulia

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
 * class has access to the currently active ProjectDriver instance 'plp'
 *
 * @author wira
 */
public abstract class PLPArchitecture {
    protected String identifier;
    protected ProjectDriver plp;

    protected int     archID;
    protected boolean hasAssembler = false;
    protected boolean hasSimCore = false;
    protected boolean hasSimCoreGUI = false;
    protected boolean hasProgrammer = false;
    protected boolean hasSyntaxHighlightSupport = false;
    protected String  informationString = "UNDEFINED";

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
     * Set new architecture ID and identifier
     *
     * @param archID Integer ISA ID
     * @param identifier String ISA ID
     */
    public void setParameters(int archID, String identifier) {
        this.archID = archID;
        this.identifier = identifier;
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
     * Get ISA String ID
     *
     * @return ISA string ID specified in the manifest
     */
    public String getStringID() {
        return identifier;
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
     * Does this ISA have syntax highlight support
     *
     * @return boolean specifying whether the ISA supports syntax highlighting
     */
    public boolean hasSyntaxHighlightSupport() {
        return hasSyntaxHighlightSupport;
    }

/*********************** FRAMEWORK INITIALIZATIONS ***************************/

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

/*********************** OVERRIDABLE METHODS *********************************/

    /**
     * Get an implementation of the ISA syntax highlighting support
     * 
     * @return Reference to the syntax highlight support object
     */
    public PLPSyntaxHighlightSupport getSyntaxHighlightSupport() {return null;}

    /**
     * Additional simulation initialization code. Called by the ProjectDriver
     * AFTER the I/O registry has been initialized but before I/O module
     * presets are loaded to the simulation
     *
     */
    public void simulatorInitialization() {}

    /**
     * Additional simulation initialization code called after all simulation
     * elements are initialized
     */
    public void simulatorInitializationFinal() {}

    /**
     * Additional code after simulation is stopped. Called by the ProjectDriver
     * immediately after the project exits simulation mode
     */
    public void simulatorStop() {}

    /**
     * Launch a CLI for the simulator (if one is implemented)
     */
    public void launchSimulatorCLI() {}

    /**
     * Return a QuickReference string for the IDE
     *
     * @return QuickReference string to display (HTML formatted)
     */
    public String getQuickReferenceString() {return null;}

    /**
     * Save architecture-specific simulation configuration to PLP file
     *
     * @return Additional configuration string to save to plp.simconfig
     */
    public String saveArchSpecificSimStates() {return "";}

    /**
     * Set architecture-specific simulation configuration to the current
     * open project driver. ProjectDriver.open() will pass an array of String
     * tokens split with the string "::" as the delimiter. For example,
     * the line in config file "foo::bar" will be passed on to this function
     * as {"foo", "bar"} in configStr
     *
     * @param configStr Configuration string saved in plp.simconfig
     */
    public void restoreArchSpecificSimStates(String[] configStr) {}

    /**
     * Called when the user requests for a program listing
     */
    public void listing() {}

    /**
     * Called when a new project with the specified ISA has been called. This
     * function is NOT called if the user provides a source file to import
     * while creating a new project, e.g. if the user uses the '-c' command
     * line from the terminal
     */
    public void newProject(ProjectDriver plp) {}

    /**
     * Init is called right after the architecture is instantiated
     */
    public void init() {}

    /**
     * Cleanup the mess the project might have done. This is called when the
     * user changes the ISA for the project, or the project is closed
     */
    public void cleanup() {}

    /**
     * CLI Simulation command, called when PLPTool is being run in scripted
     * simulation mode from command line. This function interprets a
     * simulation script line.
     * 
     * @param cmd
     */
    public void simCLICommand(String cmd) {}

    /**
     * Overridable developer-specified generic hook.
     *
     * @param param An object to pass to the hook
     * @return A reference to an object returned from the hook function
     */
    public Object hook(Object param) {return null;}

    /**
     * Get information string for the architecture
     *
     * @return
     */
    public String getInformationString() {
        return informationString;
    }

    public boolean equals(String str) {
        return identifier.equals(str);
    }
}
