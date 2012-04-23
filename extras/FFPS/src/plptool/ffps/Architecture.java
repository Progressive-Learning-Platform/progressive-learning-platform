/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.ffps;

import plptool.*;
import plptool.gui.PLPToolApp;

/**
 *
 * @author wira
 */
public class Architecture extends PLPArchitecture {

    private boolean override_modules = false;

    public Architecture() {
        super(400, "Fast Functional PLP Simulation", null);
        Msg.M("FFPS: PLP CPU ISA implementation with Fast Functional PLP Simulation");
        hasAssembler = true;
        hasSimCore = true;
        override_modules = PLPToolApp.getAttributes().containsKey("ffps_override_modules");
        informationString = "PLP CPU ISA implementation with Fast Functional PLP Simulation (FFPS)";
    }

    public PLPAsm createAssembler() {
        return new plptool.mips.Asm(plp.getAsms());
    }

    public PLPSerialProgrammer createProgrammer() {
        return null;
    }

    public PLPSimCore createSimCore() {
        return new SimCore();
    }

    public PLPSimCoreGUI createSimCoreGUI() {
        return null;
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
