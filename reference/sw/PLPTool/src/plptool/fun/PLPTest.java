/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.fun;

import plptool.mips.SimCore;
import plptool.mips.Asm;
//import plptool.PLPSimMods;
import plptool.Msg;

/**
 *
 * @author wira
 */
public class PLPTest {
    public static void main(String[] args) {
        Asm asm = new Asm("start:\n" +
                                "li $3,0x80004004\n" +
                                "li $4,0xbeef\n" +
                                "nop\n" +
                                "nop\n" +
                                "sw $4,0($3)\n" +
                                "j start\n" +
                                "nop"
                                , "in-line");
        if(asm.preprocess(0) == plptool.Constants.PLP_OK) {
            if(asm.assemble() != plptool.Constants.PLP_OK)
                System.exit(-1);
        }
        else
            System.exit(-1);

        
        plptool.mips.SimCore sim = new plptool.mips.SimCore(asm, 0);
        sim.bus.add(new plptool.mods.MemModule(0, 0x1000, true));
        sim.bus.add(new plptool.mods.LEDArray(0x80004004L));
        sim.bus.enableAllModules();
        sim.bus.eval();
        sim.reset();
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.bus.eval();
        sim.regfile.print(3);
        sim.regfile.print(4);



    }
}
