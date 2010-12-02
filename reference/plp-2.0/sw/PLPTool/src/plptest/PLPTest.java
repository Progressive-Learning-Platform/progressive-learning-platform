/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptest;

import plptool.PLPMIPSSim;
import plptool.PLPAsm;

/**
 *
 * @author wira
 */
public class PLPTest {
    public static void main(String[] args) {
        PLPAsm asm = new PLPAsm("start:\n" +
                                "li $3,0xdeadbeef\n" +
                                "li $4,0xfeedbeef\n" +
                                "j start\n" +
                                "nop"
                                , "in-line", 0);
        asm.preprocess(0);
        System.out.println(asm.assemble());
        PLPMIPSSim sim = new PLPMIPSSim(asm, 8000);
        sim.reset();
        sim.memory.printProgram(-1);
        sim.memory.printAll(-1);
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.step();
        sim.regfile.print(3);
        sim.regfile.print(4);
    }
}
