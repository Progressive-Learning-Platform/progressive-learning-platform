/*
    Copyright 2012 PLP Contributors

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

package plptool.ffps;

import plptool.*;
import plptool.gui.PLPToolApp;
import plptool.mips.*;

/**
 *
 * @author wira
 */
public class Architecture extends PLPArchitecture {

    private boolean override_modules = false;
    private plptool.mips.SyntaxHighlightSupport syntaxHighlightSupport;

    public Architecture() {
        super(400, "Fast Functional PLP Simulation", null);
        Msg.M("FFPS: PLP CPU ISA implementation with Fast Functional PLP Simulation");
        hasAssembler = true;
        hasSimCore = true;
        hasProgrammer = true;
        hasSyntaxHighlightSupport = true;
        syntaxHighlightSupport = new SyntaxHighlightSupport();
        override_modules = PLPToolApp.getAttributes().containsKey("ffps_override_modules");
        informationString = "PLP CPU ISA implementation with Fast Functional PLP Simulation (FFPS)";
    }

    public PLPAsm createAssembler() {
        return new plptool.mips.Asm(plp.getAsms());
    }

    public PLPSerialProgrammer createProgrammer() {
        return new plptool.mips.SerialProgrammer(plp);
    }

    public PLPSimCore createSimCore() {
        return new plptool.ffps.SimCore();
    }

    public PLPSimCoreGUI createSimCoreGUI() {
        return null;
    }

    @Override
    public PLPSyntaxHighlightSupport getSyntaxHighlightSupport() {
        return syntaxHighlightSupport;
    }

    @Override
    public void launchSimulatorCLI() {
        SimCLI.simCL(plp);
    }

    /**
     * Perform simulator initialization
     */
    @Override
    public void simulatorInitializationFinal() {
        if(override_modules) {
            Msg.W("FFPS Module Override!", this);
            plp.ioreg.removeAllModules();
            plp.sim.bus.add(new FastMemory(0x10000000L, 0x800, "ROM"));
            plp.sim.bus.add(new FastMemory(0x10000000L, 0x1000000, "RAM"));
            plp.sim.bus.add(new Timer(0xf0600000L));
            plp.sim.bus.add(new FastMemory(0xf0a00000L, 4, "Seven segments"));
            plp.sim.bus.add(new FastMemory(0xf0500000L, 4, "PLPID1"));
            plp.sim.bus.add(new FastMemory(0xf0500004L, 4, "PLPID2"));
            plp.sim.bus.add(new FastMemory(0xf0100000L, 4, "Switches"));
            plp.sim.bus.add(new FastMemory(0xf0200000L, 4, "LEDs"));
        }
    }

    @Override
    public String toString() {
        return "FFPS";
    }
}
