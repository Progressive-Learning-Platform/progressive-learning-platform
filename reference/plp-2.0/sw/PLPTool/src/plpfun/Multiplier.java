/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plpfun;

import plptool.PLPSimBusModule;
import plptool.PLPMsg;

/**
 *
 * @author wira
 */
public class Multiplier extends PLPSimBusModule{

    public Multiplier(long start) {
        super(start, start + 12, true);

        // enable self
        super.enable();

        // init regs
        super.write(start, (long) 0, false);       // input A
        super.write(start + 4, (long) 0, false);   // input B
        super.write(start + 8, (long) 0, false);   // high(A * B)
        super.write(start + 12, (long) 0, false);  // low(A * B)
    }

    public int eval() {
        if(!enabled)
            return PLPMsg.PLP_SIM_MODULE_DISABLED;

        Object[][] valueSet = super.getValueSet();

        long result = (Long) valueSet[0][1] * (Long) valueSet[1][1];
        super.write(super.startAddr + 8, result >> 32, false);
        super.write(super.startAddr + 12, result & 0xffffffff, false);

        return PLPMsg.PLP_OK;
    }

    public int gui_eval(Object x) {

        return PLPMsg.PLP_OK;
    }

    public String introduce() {

        return "plpfun.Multiplier";
    }

}
