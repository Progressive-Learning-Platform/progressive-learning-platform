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
    private final int NUMBER_OF_MODULES = 9;
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

        /* ********************************************************************/
        // PLPID

        mods[4][0] = "PLPID";
        mods[4][1] = false;
        mods[4][2] = 2;
        mods[4][3] = "This module always returns the board ID and frequency "
                   + "when the first and second registers are read, "
                   + "respectively.";
        mods[4][4] = true;
        mods[4][5] = true;
        
        /* ********************************************************************/
        // DummyMemory

        mods[5][0] = "Dummy Memory";
        mods[5][1] = true;
        mods[5][2] = 1;
        mods[5][3] = "This module always returns zero, no init required.";
        mods[5][4] = true;
        mods[5][5] = false;

        /* ********************************************************************/
        // VGA

        mods[6][0] = "VGA";
        mods[6][1] = false;
        mods[6][2] = 2;
        mods[6][3] = "640x480 VGA module. Consists of two registers, refer"
                   + " to the manual for usage.";
        mods[6][4] = true;
        mods[6][5] = true;

        /* ********************************************************************/
        // Timer

        mods[7][0] = "Timer";
        mods[7][1] = false;
        mods[7][2] = 1;
        mods[7][3] = "Timer module";
        mods[7][4] = true;
        mods[7][5] = false;

        /* ********************************************************************/
        // UART

        mods[8][0] = "UART";
        mods[8][1] = false;
        mods[8][2] = 4;
        mods[8][3] = "UART module";
        mods[8][4] = true;
        mods[8][5] = true;
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

            /******************************************************************/
            // PLPID is summoned
            case 4:
                module = new PLPID(addr);
                moduleFrame = new PLPIDFrame((PLPID) module);
                break;

            /******************************************************************/
            // DummyMemory is summoned
            case 5:
                module = new DummyMemory(addr, size, true);
                break;

            /******************************************************************/
            // VGA is summoned
            case 6:
                moduleFrame = new VGAFrame();
                module = new VGA(addr, sim.bus, (VGAFrame) moduleFrame);
                module.threaded = plptool.PLPCfg.threadedModEnabled;
                module.stop = false;
                if(module.threaded)
                    module.start();
                break;

            /******************************************************************/
            // Timer is summoned
            case 7:
                module = new Timer(addr);
                break;

            /******************************************************************/
            // UART is summoned
            case 8:
                module = new UART(addr);
                moduleFrame = new UARTFrame();
                ((UART)module).setFrame(moduleFrame);
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
        if(index >= modules.size() || index < 0) {
            PLPMsg.E("getModule: invalid index:" + index,
                     Constants.PLP_GENERIC_ERROR, null);
            return null;
        }

        return modules.get(index);
    }

    /**
     * @param index Index of the module.
     * @return The position of the specified module in the simulation bus.
     */
    public int getPositionInBus(int index) {
        if(index >= modules.size() || index < 0) {
            PLPMsg.E("getPositionInBus: invalid index: " + index,
                     Constants.PLP_GENERIC_ERROR, null);
            return -1;
        }

        return positionInBus.get(index);
    }

    /**
     * @param index Index of the module.
     * @return The frame object associated with the specified module.
     */
    public Object getModuleFrame(int index) {
        if(index >= modules.size() || index < 0) {
            PLPMsg.E("getModuleFrame: invalid index:" + index,
                     Constants.PLP_GENERIC_ERROR, null);
            return null;
        }

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
            
            if(modules.get(0).threaded && modules.get(0).isAlive()) {
                modules.get(0).stop = true;
        }
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
    public int removeModule(int index, PLPSimCore sim) {
        if(index >= modules.size() || index < 0) {
            return PLPMsg.E("removeModule: invalid index:" + index,
                       Constants.PLP_GENERIC_ERROR, null);
        }

        sim.bus.remove(positionInBus.get(index));
        positionInBus.remove(index);

        if(modules.get(index).threaded && modules.get(index).isAlive()) {
            modules.get(index).stop = true;
        }

        modules.remove(index);

        if(moduleFrames.get(index) != null && moduleFrames.get(index) instanceof JInternalFrame)
            ((JInternalFrame) moduleFrames.get(index)).dispose();

        moduleFrames.remove(index);

        // we have to update bus positions
        for(int i = index; i < positionInBus.size(); i++) {
            Integer val = positionInBus.get(i);
            positionInBus.set(i, val - 1);
        }

        return Constants.PLP_OK;
    }

    /**
     * Execute a GUI evaluation on all modules attached to this registry.
     */
    public void gui_eval() {
        for(int index = 0; index < getNumOfModsAttached(); index++)
            if(!modules.get(index).threaded)
                modules.get(index).gui_eval(moduleFrames.get(index));
            else if(modules.get(index).isAlive()) {
                try {
                modules.get(index).notify();
                } catch(Exception e) {}
            }
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

    /**
     * Loads a preset
     *
     * @param index Index of the preset
     */
    public static int loadPreset(int index, plptool.gui.ProjectDriver plp) {
        if(index >= Presets.presets.length || index < 0)
            return PLPMsg.E("loadPreset: invalid index: " + index,
                            Constants.PLP_GENERIC_ERROR, null);

        Integer[] modsType = (Integer[]) Presets.presets[index][1];
        Long[] startAddresses = (Long[]) Presets.presets[index][2];
        Long[] sizes = (Long[]) Presets.presets[index][3];
        for(int i = 0; i < modsType.length; i++) {
            if(plp.g())
                plp.ioreg.attachModuleToBus(modsType[i], startAddresses[i], sizes[i], plp.sim, plp.g_simsh.getSimDesktop());
            else
                plp.ioreg.attachModuleToBus(modsType[i], startAddresses[i], sizes[i], plp.sim, null);
        }

        return Constants.PLP_OK;
    }
}
