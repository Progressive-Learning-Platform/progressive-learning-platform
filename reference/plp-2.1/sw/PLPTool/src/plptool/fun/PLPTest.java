/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.fun;

import plptool.mips.SimCore;
import plptool.mips.Asm;
import plptool.PLPSimMods;
import plptool.PLPMsg;

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

        
        SimCore sim = new SimCore(asm, 0, 8000);
        PLPSimMods mods = new PLPSimMods(sim);
        sim.bus.add(mods.io_leds);
        sim.bus.enableAllModules();
        //sim.bus.write((long) 0x8000400 << 4, 0xbeef, false);
        //System.out.println(String.format("%08x", sim.bus.read((long) 0x8000400 << 4)));
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
