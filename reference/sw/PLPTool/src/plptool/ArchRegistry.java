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
import plptool.gui.ProjectDriver;

/*****************************************************************************
 * IMPORT YOUR ISA PACKAGE HERE
 *****************************************************************************/
import plptool.mips.*;

// ...add 3rd party packages... //

/**
 * This class associates an ISA implementation to its member classes.
 * Developers porting an ISA to PLP system will have to register their
 * implementation here. Starting with importing their package above.
 *
 * @author wira
 */
public class ArchRegistry {

    private static String[] archs = {
                                        "plpmips",

                                          // ... add your ISA name here ...

                                        "test-isa"
                                    };

    /**
     * This method returns a new instance of the ISA assembler when given an
     * ArrayList of source files.
     *
     * @param plp The current instance of the project driver backend
     * @param asms ArrayList of source objects
     * @return Asm instance of the ISA
     */
    public static PLPAsm createAssembler(final ProjectDriver plp, ArrayList<PLPAsmSource> asms) {
        String arch = plp.getArch();

        if(arch == null)
            return null;

        /**********************************************************************
         * plpmips Assembler generator
         **********************************************************************/
        else if(arch.equals("plpmips")) {
            return new plptool.mips.Asm(asms);
        }

        // ... add your assembler instantiation here ... //

        else
            return null;
    }

    /**
     * This method returns a new instance of the ISA assembler when given an
     * either the string for the source code or just the path.
     *
     * @param plp The current instance of the project driver
     * @param asmStr String of the source code, can be null
     * @param asmPath Path of the source code, if asmStr is null this file will
     * be loaded
     * @return Asm instance of the ISA
     */
    public static PLPAsm createAssembler(final ProjectDriver plp, String asmStr, String asmPath) {
        String arch = plp.getArch();

        if(arch == null)
            return null;

        /**********************************************************************
         * plpmips Assembler generator
         **********************************************************************/
        else if(arch.equals("plpmips")) {
            return new plptool.mips.Asm(asmStr, asmPath);
        }
        
        // ... add your assembler instantiation here ... //

        else
            return null;
    }

    /**
     * This method returns a new instance of the simulation core.
     *
     * @param plp The current instance of the project driver
     * @return SimCore instance of the ISA
     */
    public static PLPSimCore createSimCore(final ProjectDriver plp) {
        String arch = plp.getArch();

        if(arch == null)
            return null;

        /**********************************************************************
         * plpmips SimCore generator
         **********************************************************************/
        else if(arch.equals("plpmips")) {
            PLPSimCore sim = new plptool.mips.SimCore((plptool.mips.Asm) plp.asm,
                                    plp.asm.getAddrTable()[0]);
            sim.setStartAddr(plp.asm.getAddrTable()[0]);
            return sim;
        }

        // ... add your simulation core instantiation here ... //

        else
            return null;
    }

    /**
     * Additional simulation initialization code. Called by the ProjectDriver
     * AFTER the I/O registry has been initialized
     *
     * @param plp The current instance of the project driver
     */
    public static void simulatorInitialization(final ProjectDriver plp) {
        String arch = plp.getArch();

        if(arch == null)
            return;

        /**********************************************************************
         * plpmips SimCore initialization
         **********************************************************************/
        else if(arch.equals("plpmips")) {
            plp.sim.bus.add(new plptool.mods.InterruptController(0xf0700000L, plp.sim));
            plp.sim.bus.add(new plptool.mods.Button(8, 0xfffffff7L, plp.sim));
            plp.sim.bus.enableAllModules();

            if(plp.g()) {
                final javax.swing.JToggleButton btnInt = new javax.swing.JToggleButton("Button Interrupt");
                btnInt.addActionListener(new java.awt.event.ActionListener() {
                    public void actionPerformed(java.awt.event.ActionEvent e) {
                        ((plptool.mods.Button) plp.sim.bus.getRefMod(1)).setPressedState(btnInt.isSelected());
                    }
                });

                plp.g_dev.addButton(btnInt);
            }
        }

        // ... add your simulation core initialization here ... //
    }

    /**
     * Additional code after simulation is stopped. Called by the ProjectDriver
     * when the project exits simulation mode
     *
     * @param plp The current instance of the project driver
     */
    public static void simulatorStop(final ProjectDriver plp) {
        String arch = plp.getArch();

        if(arch == null)
            return;

        /**********************************************************************
         * plpmips SimCore post-simulation
         **********************************************************************/
        else if(arch.equals("plpmips")) {
            if(plp.g() && plp.g_dev != null)
                plp.g_dev.removeLastButton();
        }

        // ... add your simulation core initialization here ... //
    }

    /**
     * Launch a CLI for the simulator
     *
     * @param plp The current instance of the project driver
     */
    public static int launchCLISimulatorInterface(ProjectDriver plp) {
        String arch = plp.getArch();

        if(arch == null)
            return Constants.PLP_GENERIC_ERROR;

        /**********************************************************************
         * plpmips SimCore CLI interface
         **********************************************************************/
        else if(arch.equals("plpmips")) {
            plptool.mips.SimCLI.simCL(plp);
        }

        else
            return Msg.E("The ISA " + arch + " does not have a registered" +
                   " CLI for the simulator.", Constants.PLP_ISA_NO_SIM_CLI, null);

        // ... add your simulation core instantiation here ... //

        return Constants.PLP_OK;
    }

    /**
     * This method returns a new instance of the simulation core frame.
     *
     * @param plp The current instance of the project driver
     * @return SimCoreGUI instance of the ISA
     */
    public static PLPSimCoreGUI createSimCoreGUI(ProjectDriver plp) {
        String arch = plp.getArch();

        if(arch == null)
            return null;

        /**********************************************************************
         * plpmips SimCoreGUI generator
         **********************************************************************/
        else if(arch.equals("plpmips")) {
            return new plptool.mips.SimCoreGUI(plp);
        }

        // ... add your simulation core frame instantiation here ... //

        else
            return null;
    }

    /**
     * This method returns a new instance of the serial programmer
     *
     * @param plp The current instance of the project driver
     * @return SerialProgrammer instance of the ISA
     */
    public static PLPSerialProgrammer createProgrammer(ProjectDriver plp) {
        String arch = plp.getArch();

        if(arch == null)
            return null;

        /**********************************************************************
         * plpmips serial programmer generator
         **********************************************************************/
        else if(arch.equals("plpmips")) {
            return new plptool.mips.SerialProgrammer(plp);
        }

        // ... add your serial programmer instantiation here ... //

        else
            return null;
    }

    /**
     * Determine whether the specified ISA is registered
     *
     * @param arch The ISA to test
     * @return True if it is defined, false otherwise
     */
    public static boolean validateArch(String arch) {
        for(int i = 0; i < archs.length; i++) {
            if(archs[i].equals(arch))
                return true;
        }

        return false;
    }
}
