/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.hc11;

import plptool.*;
import plptool.gui.ProjectDriver;

/**
 *
 * @author wira
 */
public class Architecture extends PLPArchitecture {

    public Architecture() {
        super(6811, "hc11", null);
        Msg.M("PLP 6811 ISA Implementation is called.");
    }

    public PLPAsm createAssembler() {
        return null;
    }

    public PLPSerialProgrammer createProgrammer() {
        return null;
    }

    public PLPSimCore createSimCore() {
        return null;
    }

    public PLPSimCoreGUI createSimCoreGUI() {
        return null;
    }
}
