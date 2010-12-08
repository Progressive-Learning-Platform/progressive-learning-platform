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

import plp.PLPToolView;
import plptool.PLPSimCore;
import plptool.PLPSimBusModule;
import plptool.Constants;
import plptool.PLPMsg;
import java.util.ArrayList;
import java.util.LinkedList;
import javax.swing.JInternalFrame;

/**
 * This class connects a module to its display frame, and allows users to
 * dynamically load / remove modules during runtime. Module developers
 * need to add module information and initialization here. 
 *
 * @author wira
 */
public class IORegistry {
    private ArrayList<PLPSimBusModule> modules;
    private ArrayList<JInternalFrame> moduleFrames;
    private LinkedList<Integer> positionInBus;

    /**********************************************************************
     * Number of modules registered. This constant needs to be incremented
     * whenever new modules are added.
     */
    private final int NUMBER_OF_MODULES = 2;
    /**********************************************************************/

    private Object[][] mods = new Object[NUMBER_OF_MODULES][6];

    public IORegistry() {
        modules = new ArrayList<PLPSimBusModule>();
        moduleFrames = new ArrayList<JInternalFrame>();

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


        // ADDITIONAL MODULE INFO HERE 
    }

    public int attachModuleToBus(int index, long addr, long size, PLPSimCore sim, PLPToolView mainWindow) {

        PLPSimBusModule module = null;
        JInternalFrame moduleFrame = null;

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

        if(moduleFrame != null)
            mainWindow.summonFrame(moduleFrame);

        return Constants.PLP_OK;
    }

    public int getNumOfModsAttached() {
        return modules.size();
    }

    public int getNumOfMods() {
        return NUMBER_OF_MODULES;
    }

    public PLPSimBusModule getModule(int index) {
        return modules.get(index);
    }

    public int getPositionInBus(int index) {
        return positionInBus.get(index);
    }

    public JInternalFrame getModuleFrame(int index) {
        return moduleFrames.get(index);
    }

    public void removeAllModules(PLPSimCore sim) {
        while(!modules.isEmpty()) {
            if(moduleFrames.get(0) != null)
                moduleFrames.get(0).dispose();
            moduleFrames.remove(0);
            modules.remove(0);
            sim.bus.remove(positionInBus.remove(positionInBus.size() - 1));
        }
    }

    public void removeModule(int index, PLPSimCore sim) {
        sim.bus.remove(positionInBus.get(index));
        positionInBus.remove(index);
        modules.remove(index);
        if(moduleFrames.get(index) != null)
            moduleFrames.get(index).dispose();
        moduleFrames.remove(index);

        // we have to update bus positions
        for(int i = index; i < positionInBus.size(); i++) {
            Integer val = positionInBus.get(i);
            positionInBus.set(i, val - 1);
        }
    }

    public void gui_eval() {
        for(int index = 0; index < getNumOfModsAttached(); index++)
            modules.get(index).gui_eval(moduleFrames.get(index));
    }
    
    public Object[][] getAvailableModulesInformation() {
        return mods;
    }

    public Object[] getAttachedModules() {
        return modules.toArray();
    }
}
