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

package plptool.mods;

import plptool.PLPSimCore;
import plptool.PLPSimBusModule;
import plptool.Constants;
import plptool.PLPMsg;
import java.util.ArrayList;
import java.util.LinkedList;
import javax.swing.JInternalFrame;

/**
 * This class associates a module to its display frame, and allows users to
 * dynamically load / remove modules during runtime. Module developers
 * need to add module information and initialization here. 
 *
 * @see plptool.PLPSimBus
 * @see plptool.PLPSimBusModule
 * @see plp.PLPIORegistry
 * @author wira
 */
public class IORegistry {
    /**
     * This is the data structure storing all the modules attached to this
     * registry
     */
    private ArrayList<PLPSimBusModule> modules;

    /**
     * This list contains references to the GUI frames of attached
     * PLPSimBusModule objects.
     */
    private ArrayList<Object> moduleFrames;

    /**
     * Stores the position of the attached modules in the simulation bus.
     */
    private LinkedList<Integer> positionInBus;

    /**********************************************************************
     * Number of modules registered. This constant needs to be incremented
     * whenever new modules are added.
     */
    private final int NUMBER_OF_MODULES = 4;
    /**********************************************************************/

    private Object[][] mods = new Object[NUMBER_OF_MODULES][6];

    /**
     * IORegistry constructor populates the module information array.
     * Module developers will have to add their module's information here
     * to be listed in the PLPTool I/O window.
     */
    public IORegistry() {
        modules = new ArrayList<PLPSimBusModule>();
        moduleFrames = new ArrayList<Object>();

        // We save bus position because the simulation core may already
        // have built-in modules attached
        positionInBus = new LinkedList<Integer>();

        // Register your I/O here !!!

        // Field 0: Name
        // Field 1: Whether the module can have variable number of registers
        // Field 2: Number of registers (doesn't matter if field 1 is true)
        // Field 3: Information string
        // Field 4: Registers MUST be aligned
        // Field 5: Has GUI frame (gui_eval is implemented)


        /* ********************************************************************/
        // PLPSimMemModule

        mods[0][0] = "Memory Module";
        mods[0][1] = true;
        mods[0][2] = 0;
        mods[0][3] =  "PLP general purpose memory module. Register file size "
                   +  "is in BYTES.";
        mods[0][4] = false;
        mods[0][5] = false;


        /* ********************************************************************/
        // LEDArray

        mods[1][0] = "LED Array";
        mods[1][1] = false;
        mods[1][2] = 1;
        mods[1][3] = "Simulated LED array. Will light up proper LEDs when "
                   + "input data is written to its register. This module only "
                   + "has one register and the register address must be "
                   + "aligned.";
        mods[1][4] = true;
        mods[1][5] = true;


        /* ********************************************************************/
        // Switches

        mods[2][0] = "Switches";
        mods[2][1] = false;
        mods[2][2] = 1;
        mods[2][3] = "Input switches. Action on the GUI frame will update the "
                   + "I/O register. This is a read-only register, writes "
                   + "issued to this module will have no effect and return "
                   + "a write error.";
        mods[2][4] = true;
        mods[2][5] = true;


        /* ********************************************************************/
        // FTracer

        mods[3][0] = "FTracer";
        mods[3][1] = true;
        mods[3][2] = 1337;
        mods[3][3] = "Traces all bus activity to the console. This module "
                   + "requires the simulator to be reset to work properly.";
        mods[3][4] = true;
        mods[3][5] = false;

        // ADDITIONAL MODULE INFO HERE 
    }

    /**
     * Instantiates and attaches a PLPSimBusModule-based module to this I/O
     * registry and to the bus of the given simulation core. This method also
     * associates a module with its internal frame, if GUI representation is
     * implemented. Module developers will have to write their module's
     * initialization here.
     *
     * @param index Module type as registered in the constructor.
     * @param addr The starting address of the module's address space.
     * @param size The size of the module's address space.
     * @param sim Simulation core associated with the main application.
     * @param simDesktop Simulator desktop the module frames will be added to
     * (if applicable).
     * @return PLP_OK on completion, or PLP_SIM_INVALID_MODULE if invalid index
     * is specified.
     */
    public int attachModuleToBus(int index, long addr, long size,
                                 PLPSimCore sim, javax.swing.JDesktopPane simDesktop) {

        PLPSimBusModule module = null;
        Object moduleFrame = null;

        switch(index) {

            // This is where the modules are initialized when the simulator
            // requests an I/O module instance to be created.

            /******************************************************************/
            // PLPSimMemModule is summoned
            case 0:             
                module = new MemModule(addr, size, true);

                break;

            /******************************************************************/
            // LEDArray is summoned
            case 1:
                module = new LEDArray(addr);
                moduleFrame = new LEDArrayFrame();

                break;

            /******************************************************************/
            // Switches is summoned
            case 2:
                module = new Switches(addr);
                moduleFrame = new SwitchesFrame((Switches) module);

                break;

            /******************************************************************/
            // FTrace is requested, he doesn't respond to summons
            case 3:
                module = new FTracer(addr, size);
                break;

            // ADD YOUR MODULE INITIALIZATION HERE

                            // ...3rd party code...//

            /** DO NOT EDIT ANYTHING BELOW THIS **/

            default:
                return PLPMsg.E("attachModuleToBus(): invalid module ID.",
                                Constants.PLP_SIM_INVALID_MODULE, this);
        }

        moduleFrames.add(moduleFrame);
        modules.add(module);
        module.enable();
        positionInBus.add(sim.bus.add(module));

        if(moduleFrame != null && simDesktop != null && moduleFrame instanceof JInternalFrame) {
            simDesktop.add((JInternalFrame) moduleFrame);
            ((JInternalFrame) moduleFrame).setVisible(true);
        }

        return Constants.PLP_OK;
    }

    /**
     * @return The number of modules attached to this registry.
     */
    public int getNumOfModsAttached() {
        return modules.size();
    }

    /**
     * @return The number of modules defined in the registry.
     */
    public int getNumOfMods() {
        return NUMBER_OF_MODULES;
    }

    /**
     * @param index Index of the module to be retrieved.
     * @return The module at index attached to this registry.
     */
    public PLPSimBusModule getModule(int index) {
        return modules.get(index);
    }

    /**
     * @param index Index of the module.
     * @return The position of the specified module in the simulation bus.
     */
    public int getPositionInBus(int index) {
        return positionInBus.get(index);
    }

    /**
     * @param index Index of the module.
     * @return The frame object associated with the specified module.
     */
    public Object getModuleFrame(int index) {
        return moduleFrames.get(index);
    }

    /**
     * Removes all modules attached to this registry AND removes them
     * from the simulation bus, effectively destroying all modules that
     * were attached to the simulation.
     *
     * @param sim Simulation core that is being run.
     */
    public void removeAllModules(PLPSimCore sim) {
        while(!modules.isEmpty()) {
            if(moduleFrames.get(0) != null && moduleFrames.get(0) instanceof JInternalFrame)
                ((JInternalFrame) moduleFrames.get(0)).dispose();
            moduleFrames.remove(0);
            modules.remove(0);
            sim.bus.remove(positionInBus.remove(positionInBus.size() - 1));
        }
    }

    /**
     * Removes the specified module from the registry AND the simulation
     * bus.
     *
     * @param index Index of the module to be removed.
     * @param sim Simulation core that is being run.
     */
    public void removeModule(int index, PLPSimCore sim) {
        sim.bus.remove(positionInBus.get(index));
        positionInBus.remove(index);
        modules.remove(index);
        if(moduleFrames.get(index) != null && moduleFrames.get(index) instanceof JInternalFrame)
            ((JInternalFrame) moduleFrames.get(index)).dispose();
        moduleFrames.remove(index);

        // we have to update bus positions
        for(int i = index; i < positionInBus.size(); i++) {
            Integer val = positionInBus.get(i);
            positionInBus.set(i, val - 1);
        }
    }

    /**
     * Execute a GUI evaluation on all modules attached to this registry.
     */
    public void gui_eval() {
        for(int index = 0; index < getNumOfModsAttached(); index++)
            modules.get(index).gui_eval(moduleFrames.get(index));
    }

    /**
     * @return Module information in an object array. Used by the I/O window
     * of the main app to list available modules that are in this registry.
     */
    public Object[][] getAvailableModulesInformation() {
        return mods;
    }

    /**
     * @return All modules attached to this registry as an object array.
     */
    public Object[] getAttachedModules() {
        return modules.toArray();
    }
}
