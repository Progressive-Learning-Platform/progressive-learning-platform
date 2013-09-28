/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.extras.g02toolkit;

import plptool.*;
import plptool.dmf.*;
import plptool.mips.SerialProgrammer;
import plptool.gui.ProjectDriver;

/**
 *
 * @author wira
 */
public class PLPToolConnector implements ModuleInterface5 {
    private ProjectDriver plp;

    public String getName() {
        return "G02 Emulation Board Toolkit";
    }

    public String getDescription() {
        return "G02 Emulation Board-specific features enabler for PLPTool 5";
    }

    public int[] getVersion() {
        int[] v = {1, 0};
        return v;
    }

    public int[] getMinimumPLPToolVersion() {
        int[] v = {5, 0};
        return v;
    }
    
    public int initialize(ProjectDriver plp) {
        this.plp = plp;
        CallbackRegistry.register(new Callbacks(),
                CallbackRegistry.EVENT_ASSEMBLE_INIT,
                CallbackRegistry.EVENT_PROGRAM_INIT);

        return Constants.PLP_OK;
    }

    class Callbacks implements Callback {
        public boolean callback(int num, Object param) {
            if(plp.getArch().getID() != 0)
                return true;
            switch(num) {
                case CallbackRegistry.EVENT_ASSEMBLE_INIT:
                    PLPAsm asm = (PLPAsm) param;
                    Msg.M("G02 Toolkit: Adding G02 specific symbols");
                    asm.getSymTable().put("g02_i2c_read", 0x4000L);
                    asm.getSymTable().put("g02_i2c_write", 0x4004L);
                    asm.getSymTable().put("g02_uart2_read", 0x5000L);
                    asm.getSymTable().put("g02_uart2_write", 0x5004L);
                    asm.getSymTable().put("g02_adc_read", 0x6000L);
                    asm.getSymTable().put("g02_adc_write", 0x6004L);
                    asm.getSymTable().put("g02_gpio_a", 0x7000L);
                    asm.getSymTable().put("g02_gpio_b", 0x8000L);
                    asm.getSymTable().put("g02_gpio_c", 0x9000L);
                    break;
                case CallbackRegistry.EVENT_PROGRAM_INIT:
                    Msg.M("G02 Toolkit: Sending G02 specific preambles");
                    byte[] d = {'p', 0, 0, 0};
                    byte ack;
                    try {
                        plp.p_port.enableReceiveTimeout(Config.prgReadTimeout);
                        plp.p_port.getOutputStream().write(d);
                        ack = (byte) plp.p_port.getInputStream().read();
                        if(ack != 'f') {
                            Msg.M("G02 Toolkit: No/invalid preamble ack received");
                        }
                    } catch(Exception e) {
                        Msg.trace(e);
                    }

                    break;
            }
            return true;
        }
    }
}
