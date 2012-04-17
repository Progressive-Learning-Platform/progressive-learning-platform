/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.hc11;

import plptool.*;

/**
 *
 * @author wira
 */
public class SimCore extends PLPSimCore {
    private Asm asm;

    public SimCore() {
        super();
    }

    public int loadProgram(PLPAsm asm) {
        this.asm = (Asm) asm;

        return Constants.PLP_OK;
    }

    public int step() {
        return Constants.PLP_OK;
    }

    public int reset() {
        return Constants.PLP_OK;
    }
}
