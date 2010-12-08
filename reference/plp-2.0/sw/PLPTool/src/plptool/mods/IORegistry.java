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
import plptool.PLPSimMemModule;
import plptool.PLPSimCore;
import plptool.PLPSimBusModule;
import plptool.PLPMsg;
import java.util.ArrayList;
import javax.swing.JInternalFrame;

/**
 * This class connects a module to its display frame. Module developers
 * should add module information and initialization here.
 *
 * @author wira
 */
public class IORegistry {
    private ArrayList<PLPSimBusModule> modules;
    private ArrayList<JInternalFrame> moduleFrames;

    /**********************************************************************/
    private int NUMBER_OF_MODULES = 2;
    /**********************************************************************/

    private Object[][] mods = new Object[NUMBER_OF_MODULES][4];

    // Register your I/O here
    // Field 0: Name
    // Field 1: Variable size?
    // Field 2: Number of registers (doesn't matter if field 1 is false)
    // Field 3: Information string

    public IORegistry() {
        modules = new ArrayList<PLPSimBusModule>();
        moduleFrames = new ArrayList<JInternalFrame>();
        
        // PLPSimMemModule *****************************************************
        mods[0][0] = "PLPSimMemModule";
        mods[0][1] = true;
        mods[0][2] = 0;
        mods[0][3] =  "PLP general purpose memory module.";

        // LEDArray ************************************************************
        mods[1][0] = "PLP LED Array";
        mods[1][1] = false;
        mods[1][2] = 1;
        mods[1][3] = "Simulated LED array. Will light up proper LEDs when "
                   + "input data is written to its register. This module only "
                   + "has one register.";
    }

    public int attachModuleToBus(int index, long addr, long size, PLPSimCore sim, PLPToolView mainWindow) {

        switch(index) {
            
            // PLPSimMemModule is summoned
            case 0:             
                PLPSimMemModule simMemModule = new PLPSimMemModule(addr, size, true);

                moduleFrames.add(null);
                modules.add(simMemModule);

                simMemModule.enable();

                sim.bus.add(simMemModule);

                return PLPMsg.PLP_OK;

            // LEDArray is summoned
            case 1:             
                LEDArrayFrame ledArrayFrame = new LEDArrayFrame();
                LEDArray  ledArray = new LEDArray(addr);

                moduleFrames.add(ledArrayFrame);
                modules.add(ledArray);

                ledArray.enable();

                sim.bus.add(ledArray);
                mainWindow.summonFrame(ledArrayFrame);

                return PLPMsg.PLP_OK;
        }

        return PLPMsg.PLP_OK;
    }

    public int getNumOfModsAttached() {
        return modules.size();
    }

    public int getNumOfMods() {
        return NUMBER_OF_MODULES;
    }

    public void removeAllModules() {
        while(!modules.isEmpty()) {
            moduleFrames.get(0).dispose();
            moduleFrames.remove(0);
            modules.remove(0);
        }
    }

    public void gui_eval() {
        for(int index = 0; index < getNumOfModsAttached(); index++)
            modules.get(index).gui_eval(moduleFrames.get(index));
    }
    
    public Object[][] getAvailableModulesInformation() {
        return mods;
    }
}
