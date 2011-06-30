/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.mods;
import plptool.Constants;
import plptool.PLPSimBusModule;
import plptool.Msg;

/**
 * PLP Switches I/O module.
 *
 * @see PLPSimBusModule
 * @author wira
 */
public class Switches extends PLPSimBusModule {

    public Switches(long addr) {
        super(addr, addr, true);
    }

    public int eval() {
        // No need to eval every cycle
        return Constants.PLP_OK;
    }

    public int gui_eval(Object x) {
        return Constants.PLP_OK;
    }

    public String introduce() {
        return "Switches";
    }

    @Override
    public int write(long addr, Object data, boolean isInstr) {
        return Msg.E("write(): Write to read only register.",
                        Constants.PLP_SIM_WRITE_TO_READONLY_MODULE, this);
    }

    @Override
    public String toString() {
        return "Switches";
    }
}
