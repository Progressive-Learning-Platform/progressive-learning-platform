/*
    Copyright 2010 David Fritz, Brian Gordon, Wira Mulia

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

package plptool;

import java.io.InputStreamReader;
import java.io.BufferedReader;

/**
 * PLPTool command line emulator interface
 *
 * @author wira
 */
public class PLPSimCL {

    public static void simCL(String asmFile) {
        BufferedReader stdIn = new BufferedReader(new InputStreamReader((System.in)));
        PLPAsm asm;
        PLPMIPSSim core = null;
        String input = "";
        String tokens[];
        int ram_size;
        boolean init_core = false;

        try {

        System.out.println("PLPTool Command Line Simulator");
        System.out.print("Assembling " + asmFile + " ...");
        asm = new PLPAsm(null, asmFile, 0);
        if(asm.preprocess(0) == PLPMsg.PLP_OK)
            asm.assemble();

        if(!asm.isAssembled()) {
            System.out.println();
            PLPMsg.E("Assembly failed.", PLPMsg.PLP_ERROR_RETURN, asm);
            System.exit(PLPMsg.PLP_ERROR_RETURN);
        }

        System.out.println(" OK");
        System.out.print("sim > ");
        while(!(input = stdIn.readLine().trim()).equals("q")) {
            tokens = input.split(" ");
            if(input.equals("version")) {
                System.out.println(PLPMsg.versionString);
            }
            else if(tokens[0].equals("i")) {
                if(tokens.length != 2) {
                    System.out.println("Usage: i <ram size in bytes>");
                }
                else {
                    try {
                    ram_size = Integer.parseInt(tokens[1]);
                   
                    if(ram_size % 4 != 0)
                        System.out.println("RAM size has to be in multiples of 4");
                    else {
                        ram_size /= 4;
                        core = new PLPMIPSSim(asm, ram_size);
                        core.reset();
                        init_core = true;
                        core.printfrontend();
                        System.out.println("Simulation core initialized.");
                    }
                    } catch(Exception e) {
                        System.out.println("Usage: i <ram size in bytes>");
                    }
                }
            }
            else if(input.equals("s")) {
                if(!init_core)
                    System.out.println("Core is not initialized.");
                else {
                    if(core.step() != PLPMsg.PLP_OK)
                        PLPMsg.E("Simulation is stale. Please reset.",
                                PLPMsg.PLP_SIM_STALE, null);
                    core.printfrontend();
                }
            }
            else if(tokens[0].equals("s")) {
                if(tokens.length != 2) {
                    System.out.println("Usage: s <number of instructions>");
                }
            else
                for(int i = 0; i < Integer.parseInt(tokens[1]); i++) {
                    if(core.step() != PLPMsg.PLP_OK)
                       PLPMsg.E("Simulation is stale. Please reset.",
                                PLPMsg.PLP_SIM_STALE, null);
                    core.printfrontend();
                }
            }
            else if(input.equals("r")) {
                if(!init_core)
                    System.out.println("Core is not initialized.");
                else {
                    core.reset();
                    core.printfrontend();
                }
            }
            else if(input.equals("pram")) {
                if(!init_core)
                    System.out.println("Core is not initialized.");
                else {
                    System.out.println("\nMain memory listing");
                    System.out.println("===================");
                    core.memory.printMain();
                }
            }
            else if(input.equals("preg")) {
                if(!init_core)
                    System.out.println("Core is not initialized.");
                else {
                    System.out.println("\nRegisters listing");
                    System.out.println("=================");
                    core.memory.printRegFile();
                }
            }
            else if(input.equals("pfd")) {
                if(!init_core)
                    System.out.println("Core is not initialized.");
                else {
                    System.out.println("\nFrontend / fetch stage state");
                    System.out.println("============================");
                    core.printfrontend();
                }
            }
            else if(input.equals("pprg")) {
                if(!init_core)
                    System.out.println("Core is not initialized.");
                else {
                    System.out.println("\nProgram Listing");
                    System.out.println("===============");
                    core.printprogram();
                }
            }
            else if(input.equals("pinstr")) {
                if(!init_core)
                    System.out.println("Core is not initialized.");
                else {
                    System.out.println("\nIn-flight instructions");
                    System.out.println("======================");
                    core.wb_stage.printinstr();
                    core.mem_stage.printinstr();
                    core.ex_stage.printinstr();
                    core.id_stage.printinstr();
                    core.printfrontend();
                }
            }
            else if(tokens[0].equals("wpc")) {
                if(tokens.length != 2) {
                    System.out.println("Usage: wpc <address>");
                }
                else {
                    core.softreset();
                    core.memory.i_pc = PLPAsm.sanitize32bits(tokens[1]);
                    core.printfrontend();
                }
            }
            else if(tokens[0].equals("j")) {
                if(tokens.length != 2) {
                    System.out.println("Usage: j <address>");
                }
                else {
                    core.memory.i_pc = PLPAsm.sanitize32bits(tokens[1]);
                    core.printfrontend();
                }
            }
            else if(input.equals("pvars")) {
                if(!init_core)
                    System.out.println("Core is not initialized.");
                else {
                    System.out.println("\nOutput side values of pipeline stages");
                    System.out.println("=====================================");
                    core.wb_stage.printvars();
                    core.mem_stage.printvars();
                    core.ex_stage.printvars();
                    core.id_stage.printvars();
                }
            }
            else if(input.equals("pnextvars")) {
                if(!init_core)
                    System.out.println("Core is not initialized.");
                else {
                    System.out.println("\nInput side values of pipeline registers");
                    System.out.println("=======================================");
                    core.wb_stage.printnextvars();
                    core.mem_stage.printnextvars();
                    core.ex_stage.printnextvars();
                    core.id_stage.printnextvars();
                }
            }
            else if(input.toLowerCase().equals("wira sucks")) {
                System.out.println("No, he doesn't.\n");
            }
            else {
                System.out.println("Unknown command\n");
            }

            System.out.print("sim > ");
        }
        System.out.println("See ya!");

        } catch(Exception e) {
            System.err.println(e);
        }
    }
}
