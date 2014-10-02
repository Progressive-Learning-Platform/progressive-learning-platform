/*
    Copyright 2010-2014 David Fritz, Brian Gordon, Wira Mulia

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

import plptool.PLPSimBusModule;
import plptool.Constants;
import plptool.gui.ProjectDriver;
import plptool.Msg;
import plptool.dmf.*;
import java.util.ArrayList;
import java.util.LinkedList;
import javax.swing.JFrame;

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
     * This list contains the sizes of the module's register file
     */
    private ArrayList<Long> regSize;

    /**
     * This list contains the types of the modules
     */
    private ArrayList<Integer> type;

    /**
     * Stores the position of the attached modules in the simulation bus.
     */
    private LinkedList<Integer> positionInBus;

    /**
     * Reference to current open project
     */
    private ProjectDriver plp;

    /**********************************************************************
     * Number of modules registered. This constant needs to be incremented
     * whenever new modules are added.
     */
    private final int NUMBER_OF_MODULES = 13;
    /**********************************************************************/

    private Object[][] mods = new Object[NUMBER_OF_MODULES][6];

    /**
     * IORegistry constructor populates the module information array.
     * Module developers will have to add their module's information here
     * to be listed in the PLPTool I/O window.
     */
    public IORegistry(ProjectDriver plp) {
        modules = new ArrayList<PLPSimBusModule>();
        moduleFrames = new ArrayList<Object>();
        regSize = new ArrayList<Long>();
        type = new ArrayList<Integer>();
        this.plp = plp;

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

        /* ********************************************************************/
        // SevenSegments

        mods[9][0] = "Seven Segment LEDs";
        mods[9][1] = false;
        mods[9][2] = 1;
        mods[9][3] = "Simulated seven segments LED.";
        mods[9][4] = true;
        mods[9][5] = true;

        /* ********************************************************************/
        // PLP MIPS Interrupt Controller

        mods[10][0] = "PLP MIPS Interrupt Controller";
        mods[10][1] = false;
        mods[10][2] = 2;
        mods[10][3] = "This module implements the PLP MIPS interrupt controller.";
        mods[10][4] = true;
        mods[10][5] = false;

        /* ********************************************************************/
        // Bus Monitor

        mods[11][0] = "Bus Monitor";
        mods[11][1] = false;
        mods[11][2] = 1;
        mods[11][3] = "Monitors bus addresses and updates the timing diagram";
        mods[11][4] = true;
        mods[11][5] = false;

        /* ********************************************************************/
        // GPIO

        mods[12][0] = "General-purpose I/O";
        mods[12][1] = false;
        mods[12][2] = 3;
        mods[12][3] = "2x8 bit GPIO with tristate register";
        mods[12][4] = true;
        mods[12][5] = true;

        // ADDITIONAL MODULE INFO HERE 
    }

    /**
     * Instantiate and attach a PLPSimBusModule-based module to this I/O
     * registry and to the bus of the given simulation core. This method also
     * associates a module with its internal frame, if GUI representation is
     * implemented. Module developers will have to write their module's
     * initialization here.
     *
     * @param index Module type as registered in the constructor.
     * @param addr The starting address of the module's address space.
     * @param size The size of the module's address space.
     * @return PLP_OK on completion, or PLP_SIM_INVALID_MODULE if invalid index
     * is specified.
     */
    public int attachModuleToBus(int index, long addr, long size) {
        // we're not loading dynamic modules from here (index -1)
        if(index < 0)
            return Constants.PLP_OK;

        PLPSimBusModule module = null;
        Object moduleFrame = null;

        Msg.D("Loading " + mods[index][0], 3, this);

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
                if(plp.g()) {
                    moduleFrame = new LEDArrayFrame();
                    attachModuleFrameListeners((JFrame) moduleFrame, plp, Constants.PLP_TOOLFRAME_SIMLEDS);
                }

                break;

            /******************************************************************/
            // Switches is summoned
            case 2:
                module = new Switches(addr);
                if(plp.g()) {
                    moduleFrame = new SwitchesFrame((Switches) module);
                    attachModuleFrameListeners((JFrame) moduleFrame, plp, Constants.PLP_TOOLFRAME_SIMSWITCHES);
                }

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
                if(plp.g()) {
                    moduleFrame = new PLPIDFrame((PLPID) module);
                    attachModuleFrameListeners((JFrame) moduleFrame, plp, Constants.PLP_TOOLFRAME_SIMPLPID);
                }
                break;

            /******************************************************************/
            // DummyMemory is summoned
            case 5:
                module = new DummyMemory(addr, size, true);
                break;

            /******************************************************************/
            // VGA is summoned
            case 6:
                if(plp.g()) {
                    moduleFrame = new VGAFrame();
                    attachModuleFrameListeners((JFrame) moduleFrame, plp, Constants.PLP_TOOLFRAME_SIMVGA);
                }
                module = new VGA(addr, plp.sim.bus, (VGAFrame) moduleFrame);
                module.threaded = plptool.Config.threadedModEnabled;
                module.stop = false;
                if(module.threaded)
                    module.start();
                break;

            /******************************************************************/
            // Timer is summoned
            case 7:
                module = new Timer(addr, plp.sim);
                break;

            /******************************************************************/
            // UART is summoned
            case 8:
                if(plp.g()) {
                    moduleFrame = new UARTFrame();
                    attachModuleFrameListeners((JFrame) moduleFrame, plp, Constants.PLP_TOOLFRAME_SIMUART);
                }
                module = new UART(addr, (UARTFrame) moduleFrame, plp.sim);
                break;

            /******************************************************************/
            // Seven Segments is summoned
            case 9:
                module = new SevenSegments(addr);
                if(plp.g()) {
                    moduleFrame = new SevenSegmentsFrame();
                    attachModuleFrameListeners((JFrame) moduleFrame, plp,  Constants.PLP_TOOLFRAME_SIMSEVENSEGMENTS);
                }

                break;

            /******************************************************************/
            // Interrupt Controller is summoned
            case 10:
                module = new InterruptController(addr, plp.sim);

                break;

            /******************************************************************/
            // Bus monitor is summoned
            case 11:
                module = new BusMonitor(plp.sim);
                if(plp.g()) {
                    moduleFrame = new BusMonitorFrame((BusMonitor) module, null);
                }

                break;

            /******************************************************************/
            // GPIO is summoned
            case 12:
                module = new GPIO(addr);
                if(plp.g()) {
                    moduleFrame = new GPIOFrame((GPIO) module);
                    attachModuleFrameListeners((JFrame) moduleFrame, plp, Constants.PLP_TOOLFRAME_SIMGPIO);
                }

                break;

            // ADD YOUR MODULE INITIALIZATION HERE

                            // ...3rd party code...//

            /** DO NOT EDIT ANYTHING BELOW THIS **/

            default:
                return Msg.E("attachModuleToBus(): invalid module ID.",
                                Constants.PLP_SIM_INVALID_MODULE, this);
        }

        moduleFrames.add(moduleFrame);
        modules.add(module);
        type.add(index);
        regSize.add(size);
        module.enable();
        positionInBus.add(plp.sim.bus.add(module));

        //if(moduleFrame != null && moduleFrame instanceof JFrame) {
        //    attachModuleFrameListeners((JFrame) moduleFrame, plp);
            //simDesktop.add((JFrame) moduleFrame);
            //((JFrame) moduleFrame).setVisible(true);
        //}

        return Constants.PLP_OK;
    }

    /**
     * Instantiate and attach a dynamic PLPSimBusModule-based module to this I/O
     * registry and to the bus of the given simulation core.
     * PLPDynamicModuleFramework is called to create a new instance of the
     * module.
     *
     * @param index Index of the dynamic module CLASS
     * @param addr Starting address of the module's memory map
     * @param size Number of registers
     * @param isWordAligned Denote whether the module's address space is word
     * aligned
     * @param frame Frame object associated with this module
     * @return PLP_OK on completion, or PLP_DBUSMOD_INSTANTIATION_ERROR if
     * the module object failed to initialize
     */
    public int attachDynamicModule(int index, long startAddr, long endAddr, boolean isWordAligned, Object frame) {
        moduleFrames.add(frame);
        PLPSimBusModule module = DynamicModuleFramework.newBusModuleInstance(index);

        if(module == null)
            return Constants.PLP_DMOD_INSTANTIATION_ERROR;

        module.setNewParameters(startAddr, endAddr, isWordAligned);
        modules.add(module);
        type.add(-1); // -1 for dynamic module
        regSize.add(isWordAligned ? (endAddr-startAddr)/4+1 : endAddr-startAddr+1);
        module.enable();
        positionInBus.add(plp.sim.bus.add(module));

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
            Msg.E("getModule: invalid index:" + index,
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
            Msg.E("getPositionInBus: invalid index: " + index,
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
            Msg.E("getModuleFrame: invalid index:" + index,
                     Constants.PLP_GENERIC_ERROR, null);
            return null;
        }

        return moduleFrames.get(index);
    }

    /**
     * @param index Index of the module.
     * @return The register file size of the module
     */
    public Object getRegSize(int index) {
        if(index >= modules.size() || index < 0) {
            Msg.E("getRegSize: invalid index:" + index,
                     Constants.PLP_GENERIC_ERROR, null);
            return null;
        }

        return regSize.get(index);
    }

    public long getStartAddr(int index) {
        if(index >= modules.size() || index < 0) {
            Msg.E("getStartAddr: invalid index:" + index,
                     Constants.PLP_GENERIC_ERROR, null);
            return -1;
        }

        return modules.get(index).startAddr();
    }


    /**
     * @param index Index of the module.
     * @return The type of the module
     */
    public long getType(int index) {
        if(index >= modules.size() || index < 0) {
            Msg.E("getType: invalid index:" + index,
                     Constants.PLP_GENERIC_ERROR, null);
            return -1;
        }

        return type.get(index);
    }

    /**
     * Removes all modules attached to this registry AND removes them
     * from the simulation bus, effectively destroying all modules that
     * were attached to the simulation.
     *
     * @param sim Simulation core that is being run.
     */
    public void removeAllModules() {
        while(!modules.isEmpty()) {
            if(moduleFrames.get(0) != null && moduleFrames.get(0) instanceof JFrame)
                ((JFrame) moduleFrames.get(0)).dispose();
            moduleFrames.remove(0);
            
            if(modules.get(0).threaded && modules.get(0).isAlive()) {
                modules.get(0).stop = true;
            }
            modules.get(0).remove();
            modules.remove(0);
            regSize.remove(0);
            type.remove(0);
            plp.sim.bus.remove(positionInBus.remove(positionInBus.size() - 1));
        }
    }

    /**
     * Removes the specified module from the registry AND the simulation
     * bus.
     *
     * @param index Index of the module to be removed.
     * @param sim Simulation core that is being run.
     */
    public int removeModule(int index) {
        if(index >= modules.size() || index < 0) {
            return Msg.E("removeModule: invalid index:" + index,
                       Constants.PLP_GENERIC_ERROR, null);
        }

        plp.sim.bus.remove(positionInBus.get(index));
        positionInBus.remove(index);

        if(modules.get(index).threaded && modules.get(index).isAlive()) {
            modules.get(index).stop = true;
        }

        modules.get(index).remove();
        modules.remove(index);
        regSize.remove(index);
        type.remove(index);

        if(moduleFrames.get(index) != null && moduleFrames.get(index) instanceof JFrame)
            ((JFrame) moduleFrames.get(index)).dispose();

        moduleFrames.remove(index);

        // we have to update bus positions
        for(int i = index; i < positionInBus.size(); i++) {
            Integer val = positionInBus.get(i);
            positionInBus.set(i, val - 1);
        }

        return Constants.PLP_OK;
    }


    /**
     * Stop module threads
     */
    public void stopThreadedMods() {
        for(int i = 0; i < modules.size(); i++) {
            if(modules.get(i).threaded)
                modules.get(i).stop = true;
        }
    }

    public void startThreadedMods() {

    }

    /**
     * Execute a GUI evaluation on all modules attached to this registry.
     */
    public void gui_eval() {
        try {

        for(int index = 0; index < getNumOfModsAttached(); index++)
            if(!modules.get(index).threaded)
                modules.get(index).gui_eval(moduleFrames.get(index));
            else if(modules.get(index).isAlive()) {
                try {
                modules.get(index).notify();
                } catch(Exception e) {}
            }

        CallbackRegistry.callback(CallbackRegistry.BUS_GUI_EVAL, null);

        } catch(NullPointerException e) {
            // modules might have been removed when this is called, ignore
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
     * Loads a predefined preset
     *
     * @param index Index of the preset
     */
    public int loadPredefinedPreset(int index) {
        if(index >= Preset.presets.length || index < 0)
            return Msg.E("loadPredefinedPreset: invalid index: " + index,
                            Constants.PLP_GENERIC_ERROR, null);

        Integer[] modsType = (Integer[]) Preset.presets[index][1];
        Long[] startAddresses = (Long[]) Preset.presets[index][2];
        Long[] sizes = (Long[]) Preset.presets[index][3];
        for(int i = 0; i < modsType.length; i++) {
            if(plp.g())
                this.attachModuleToBus(modsType[i], startAddresses[i], sizes[i]);
            else
                this.attachModuleToBus(modsType[i], startAddresses[i], sizes[i]);
        }

        return Constants.PLP_OK;
    }

    /**
     * Loads a preset
     *
     * @param preset preset object to load
     */
    public int loadPreset(Preset preset) {
        Msg.D("loading preset, # of mods: " + preset.size(), 3, this);

        for(int i = 0; i < preset.size(); i++) {
            if(plp.g()) {
                this.attachModuleToBus(preset.getType(i), preset.getAddress(i), preset.getSize(i));
                if(preset.getHasFrame(i))
                    ((JFrame)moduleFrames.get(i)).setVisible(preset.getVisible(i));
            }
            else
                this.attachModuleToBus(preset.getType(i), preset.getAddress(i), preset.getSize(i));
        }

        return Constants.PLP_OK;
    }

    /**
     * Creates a preset off the currently attached modules
     */
    public Preset createPreset() {
        Msg.D("Adding " + modules.size() + " modules to preset.", 3, this);

        Preset preset = new Preset();

        for(int i = 0; i < modules.size(); i++) {
            preset.addModuleDefinition(type.get(i), modules.get(i).startAddr(), regSize.get(i), 
                    (moduleFrames.get(i) != null && (moduleFrames.get(i) instanceof JFrame)),
                    (moduleFrames.get(i) instanceof JFrame) ? ((JFrame)moduleFrames.get(i)).isVisible() : false);
        }

        return preset;
    }

    /**
     * Attach listeners to the specified module frame x
     *
     * @param x Module frame to attach the listener to
     * @param plp Current project driver instance
     */
    public static void attachModuleFrameListeners(final JFrame x, final plptool.gui.ProjectDriver plp, final int menuDesignation) {
        x.addWindowListener(new java.awt.event.WindowListener() {
            @Override
            public void windowClosing(java.awt.event.WindowEvent evt) {
                x.setVisible(false);
                plp.g_ioreg.refreshModulesTable();
                try {
                plp.g_dev.getToolCheckboxMenu(menuDesignation).setSelected(false);
                plp.g_dev.getToolToggleButton(menuDesignation).setSelected(false);
                } catch(NullPointerException ne) {
                    Msg.D("IORegistry.attachModuleFrameListeners: menu designation " + menuDesignation + " - no frame.", 3, this);
                    if(Constants.debugLevel >= 4)
                        ne.printStackTrace();
                }
            }

            @Override public void windowOpened(java.awt.event.WindowEvent evt) { }
            @Override public void windowDeactivated(java.awt.event.WindowEvent evt) { }
            @Override public void windowActivated(java.awt.event.WindowEvent evt) { }
            @Override public void windowDeiconified(java.awt.event.WindowEvent evt) { }
            @Override public void windowIconified(java.awt.event.WindowEvent evt) { }
            @Override public void windowClosed(java.awt.event.WindowEvent evt) { }
        });
    }

    @Override
    public String toString() {
        return "IORegistry";
    }
}
