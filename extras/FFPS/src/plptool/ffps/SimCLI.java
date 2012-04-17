/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.ffps;

import plptool.*;
import plptool.mips.MIPSInstr;
import plptool.gui.ProjectDriver;

/**
 *
 * @author wira
 */
public class SimCLI {
    public static void simCL(ProjectDriver plp) {
        //plp.sim.loadProgram(plp.asm);
        plp.ioreg.loadPredefinedPreset(2);
        String in = "start";
        while(!in.equals("q")) {
            System.out.println("pc:" + PLPToolbox.format32Hex(plp.sim.visibleAddr));
            System.out.println("instr:" + MIPSInstr.format(plp.asm.getObjectCode()[plp.asm.lookupAddrIndex(plp.sim.visibleAddr)]));
            System.out.print("> ");
            in = PLPToolbox.readLine();

            if(in.equals("r")) {
                plp.sim.reset();
            } else if(in.startsWith("s")) {
                String tokens[] = in.split(" ");
                if(tokens.length == 1)
                    plp.sim.step();
                else {
                    int steps = Integer.parseInt(tokens[1]);
                    long begin = System.currentTimeMillis();
                    for(int i = 0; i < steps; i++)
                        plp.sim.step();
                    long runtime = System.currentTimeMillis() - begin;
                    System.out.println("That took " + runtime + " milliseconds.");
                }
            } else if(in.equals("preg")) {
                for(int i = 0; i < 32; i++)
                    System.out.println(i + "\t:\t" + PLPToolbox.format32Hex(((SimCore) plp.sim).regfile[i]));
            } else if(in.startsWith("rbus")) {
                String tokens[] = in.split(" ");
                if(tokens.length > 1)
                    System.out.println(PLPToolbox.format32Hex((Long) plp.sim.bus.read(PLPToolbox.parseNum(tokens[1]))));
            }

        }
    }
}
