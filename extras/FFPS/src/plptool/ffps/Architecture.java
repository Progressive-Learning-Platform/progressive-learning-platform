/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.ffps;

import plptool.*;

/**
 *
 * @author wira
 */
public class Architecture extends PLPArchitecture {

    public Architecture() {
        super(400, "ffps", null);
        Msg.M("PLP CPU ISA implementation with fast functional PLP simulation");
        hasAssembler = true;
        hasSimCore = true;
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
}
