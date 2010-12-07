/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.mods;

import java.awt.Color;
import plptool.PLPSimBusModule;
import plptool.PLPMsg;

/**
 * An example of a bus module utilizing the gui_eval function. When evaluated,
 * this module lights up the correct LEDs on the LEDArrayFrame class.
 *
 * @author wira
 */
public class LEDArray extends PLPSimBusModule {

    public LEDArray(long addr) {
        super(addr, addr, true);
    }

    public int eval() {
        // No need to eval every cycle
        return PLPMsg.PLP_OK;
    }

    public int gui_eval(Object x) {
        if(!enabled)
            return PLPMsg.PLP_OK;

        // Get register value
        if(!isInitialized(startAddr))
            return PLPMsg.PLP_SIM_BUS_ERROR;
        
        long value = (Long) super.read(super.startAddr);

        ((LEDArrayFrame)x).setAddress(super.startAddr());
        ((LEDArrayFrame)x).setValue(value);

        // Combinational logic
        for(int i = 7; i >= 0; i--) {
            if((value & (long) Math.pow(2, i)) == (long) Math.pow(2, i))
                ((LEDArrayFrame)x).getLED(i).setBackground(Color.GREEN);
            else
                ((LEDArrayFrame)x).getLED(i).setBackground(Color.BLACK);
        }

        return PLPMsg.PLP_OK;
    }

    public String introduce() {
        return "PLP LED array";
    }

    @Override
    public String toString() {
        return "PLP LED Array";
    }
}
