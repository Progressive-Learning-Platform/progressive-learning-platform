/*
    Copyright 2012 PLP Contributors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

package plptool.ffps;

import plptool.*;
import plptool.mods.MemModule;
import plptool.mips.MIPSInstr;
import plptool.gui.ProjectDriver;

/**
 *
 * @author wira
 */
public class SimCLI {
    public static void simCL(ProjectDriver plp) {
        plp.sim.reset();
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
                    int ret = 0;
                    int steps = Integer.parseInt(tokens[1]);
                    long begin = System.currentTimeMillis();
                    for(int i = 0; i < steps && ret == 0; i++)
                        ret = plp.sim.step();
                    long runtime = System.currentTimeMillis() - begin;
                    System.out.println("That took " + runtime + " milliseconds.");
                }
            } else if(in.equals("preg")) {
                for(int i = 0; i < 32; i++)
                    System.out.println(i + "\t:\t" +
                            PLPToolbox.format32Hex(((SimCore) plp.sim).regfile[i]));
            } else if(in.startsWith("rbus")) {
                String tokens[] = in.split(" ");
                if(tokens.length > 1)
                    System.out.println(PLPToolbox.format32Hex((Long) plp.sim.bus.read(PLPToolbox.parseNum(tokens[1]))));
            } else if(in.equals("pmmap")) {
                plptool.PLPSimBusModule module;
                for(int i = 0; i < plp.sim.bus.getNumOfMods(); i++) {
                    module = plp.sim.bus.getRefMod(i);
                    System.out.println(i +"\t" + PLPToolbox.format32Hex(module.startAddr()) +
                            ":" + PLPToolbox.format32Hex(module.endAddr()) + "\t" + plp.sim.bus.getRefMod(i));
                }
            } else if(in.equals("pbusevallatency")) {
                    System.out.println("Total bus eval latency: " + ((SimCore)plp.sim).total_bus_eval_latency);
            }
        }
    }
}