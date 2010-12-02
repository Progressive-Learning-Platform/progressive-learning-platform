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
        long ram_size;
        boolean init_core = false;
        boolean silent = false;

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
                if(tokens.length == 1) {
                    core = new PLPMIPSSim(asm, -1);
                    core.reset();
                    init_core = true;
                    core.printfrontend();
                    core.bus.enableiomods();
                    System.out.println("Simulation core initialized with nigh-infinite RAM.");
                }
                else if(tokens.length != 2) {
                    System.out.println("Usage: i <ram size in bytes>");
                }
                else {
                    try {
                    ram_size = PLPToolbox.parseNum(tokens[1]);
                   
                    if(ram_size % 4 != 0)
                        System.out.println("RAM size has to be in multiples of 4");
                    else {
                        ram_size /= 4;
                        core = new PLPMIPSSim(asm, ram_size);
                        core.reset();
                        init_core = true;
                        core.printfrontend();
                        core.bus.enableiomods();
                        System.out.println("Simulation core initialized.");
                    }
                    } catch(Exception e) {
                        System.out.println(e);
                        System.out.println("Usage: i <ram size in bytes>");
                    }
                }
            }
            else if(input.equals("help") && !init_core) {
                System.out.println("Run i to initialize simulation core with default RAM size.\n" +
                                   "Run i <ram size in bytes> to specify RAM size.\n" +
                                   "Run q to quit.\n");
            }
            else {
            if(init_core) {
            if(input.equals("s")) {
                if(core.step() != PLPMsg.PLP_OK)
                     PLPMsg.E("Simulation is stale. Please reset.",
                              PLPMsg.PLP_SIM_STALE, null);
                else if(!silent) {
                    System.out.println();
                    core.wb_stage.printinstr();
                    core.mem_stage.printinstr();
                    core.ex_stage.printinstr();
                    core.id_stage.printinstr();
                    core.printfrontend();
                    System.out.println("-------------------------------------");
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
                        else if(!silent) {
                            System.out.println();
                            core.wb_stage.printinstr();
                            core.mem_stage.printinstr();
                            core.ex_stage.printinstr();
                            core.id_stage.printinstr();
                            core.printfrontend();
                            System.out.println("-------------------------------------");
                        }
                    }
            }
            else if(input.equals("r")) {
                core.reset();
                core.printfrontend();
            }
            else if(input.equals("pram")) {
                
                System.out.println("\nMain memory listing");
                System.out.println("===================");
                core.memory.printAll(core.pc.eval());
            }
            else if(tokens[0].equals("pram")) {
                if(tokens.length != 2) {
                    System.out.println("Usage: pram <address>");
                }
                else {
                    core.memory.print(PLPToolbox.parseNum(tokens[1]));
                }
            }
            else if(input.equals("preg")) {
                long data;
                System.out.println("\nRegisters listing");
                System.out.println("=================");
                for(int j = 0; j < 32; j++) {
                    data = core.regfile.read(j);
                    System.out.println(j + "\t" +
                                       String.format("%08x", data) + "\t" +
                                       PLPAsmFormatter.asciiWord(data));
                }
            }
            else if(input.equals("pfd")) {
                System.out.println("\nFrontend / fetch stage state");
                System.out.println("============================");
                core.printfrontend();
            }
            else if(input.equals("pprg")) {
                System.out.println("\nProgram Listing");
                System.out.println("===============");
                core.memory.printProgram(core.pc.eval());
            }
            else if(input.equals("pasm")) {
                PLPAsmFormatter.prettyPrint(asm);
            }
            else if(input.equals("pinstr")) {
                System.out.println("\nIn-flight instructions");
                System.out.println("======================");
                core.wb_stage.printinstr();
                core.mem_stage.printinstr();
                core.ex_stage.printinstr();
                core.id_stage.printinstr();
                core.printfrontend();
            }
            else if(tokens[0].equals("wpc")) {
                if(tokens.length != 2) {
                    System.out.println("Usage: wpc <address>");
                }
                else {
                    core.softreset();
                    core.pc.write(PLPAsm.sanitize32bits(tokens[1]));
                    core.printfrontend();
                }
            }
            else if(tokens[0].equals("w")) {
                if(tokens.length != 3) {
                    System.out.println("Usage: w <address> <data>");
                }
                else {
                    core.memory.write(PLPAsm.sanitize32bits(tokens[1]),
                                      PLPAsm.sanitize32bits(tokens[2]), false);
                    core.memory.print(PLPAsm.sanitize32bits(tokens[1]));
                }
            }
            else if(tokens[0].equals("wbus")) {
                if(tokens.length != 3) {
                    System.out.println("Usage: w <address> <data>");
                }
                else {
                    core.bus.write(PLPAsm.sanitize32bits(tokens[1]),
                                      PLPAsm.sanitize32bits(tokens[2]), false);
                }
            }
            else if(input.equals("listio")) {
                for(int i = 0; i < core.bus.nummods(); i++)
                    System.out.println(i + ": " + core.bus.introduceio(i) +
                                       " enabled: " + core.bus.enabled(i));
            }
            else if(input.equals("enableio")) {
                core.bus.enableiomods();
            }
            else if(tokens[0].equals("enableio")) {
                if(tokens.length != 2) {
                    System.out.println("Usage: enableio <index>");
                }
                else {
                    core.bus.enableio((int) PLPToolbox.parseNum(tokens[1]));
                }
            }
            else if(input.equals("disableio")) {
                core.bus.disableiomods();
            }
            else if(tokens[0].equals("disableio")) {
                if(tokens.length != 2) {
                    System.out.println("Usage: disableio <index>");
                }
                else {
                    core.bus.disableio((int) PLPToolbox.parseNum(tokens[1]));
                }
            }
            else if(tokens[0].equals("j")) {
                if(tokens.length != 2) {
                    System.out.println("Usage: j <address>");
                }
                else {
                    core.pc.write(PLPAsm.sanitize32bits(tokens[1]));
                    core.printfrontend();
                }
            }
            else if(tokens[0].equals("asm")) {
                if(tokens.length < 3) {
                    System.out.println("Usage: asm <address> <in-line assembly>");
                }
                else {
                    String iAsm = "";
                    long addr;
                    for(int j = 2; j < tokens.length; j++)
                        iAsm += tokens[j] + " ";
                    PLPAsm inlineAsm = new PLPAsm(iAsm, "PLPSimCL inline asm", 0);
                    if(inlineAsm.preprocess(0) == PLPMsg.PLP_OK)
                        inlineAsm.assemble();
                    if(inlineAsm.isAssembled()) {
                        System.out.println("\nCode injected:");
                        System.out.println("==============");
                        for(int j = 0; j < inlineAsm.getObjectCode().length; j++) {
                            addr = PLPToolbox.parseNum(tokens[1]) + 4 * j;
                            core.memory.write(addr, inlineAsm.getObjectCode()[j], (inlineAsm.isInstruction(j) == 0) ? true : false);
                            System.out.println(String.format("%08x", addr) +
                                               "   " + PLPAsmFormatter.asciiWord(inlineAsm.getObjectCode()[j]) +
                                               "  " + MIPSInstr.format(inlineAsm.getObjectCode()[j]));
                        }
                    }
                }
            }
            else if(input.equals("pvars")) {
                System.out.println("\nOutput side values of pipeline stages");
                System.out.println("=====================================");
                core.wb_stage.printvars();
                core.mem_stage.printvars();
                core.ex_stage.printvars();
                core.id_stage.printvars();

            }
            else if(input.equals("pnextvars")) {
                System.out.println("\nInput side values of pipeline registers");
                System.out.println("=======================================");
                core.wb_stage.printnextvars();
                core.mem_stage.printnextvars();
                core.ex_stage.printnextvars();
                core.id_stage.printnextvars();
            }
            else if(input.equals("silent")) {
                if(silent) {
                    silent = false;
                    System.out.println("Silent mode off.");
                } else {
                    silent = true;
                    System.out.println("Silent mode on.");
                }
            }
            else if(input.toLowerCase().equals("wira sucks")) {
                System.out.println("No, he doesn't.");
            }
            else {
                simCLHelp();
            }

            System.out.println();

            
            } else {
                System.out.println("Simulation core is not initialiazed. Try running the command 'i'.\n");
            }
        }
            if(!init_core)
                System.out.print("sim > ");
            else
                System.out.print(String.format("%08x", core.getFlags()) +
                                 " " + core.getinstrcount() +
                                 " sim > ");
        }
        System.out.println("See ya!");

        } catch(Exception e) {
            System.err.println(e);
        }
    }

    public static void simCLHelp() {
        System.out.println("Unknown command. Command list:");
        System.out.println("\n i <RAM size in bytes> ..or.. i\n\tInit core with RAM size in bytes. Set RAM size to 2^62 if no argument is given.");
        System.out.println("\n s <steps> ..or.. s\n\tAdvance <steps> number of cycles. Step 1 cycle if no argument is given.");
        System.out.println("\n r\n\tReset simulated CPU (clears memory elements and reloads program).");
        System.out.println("\n pinstr\n\tPrint instructions currently in-flight.");
        System.out.println("\n pvars\n\tPrint pipeline registers' values.");
        System.out.println("\n pnextvars\n\tPrint pipeline registers' input values.");
        System.out.println("\n pram <address> ..or.. pram\n\tPrint value of RAM at <address>. Print all if no argument is given.");
        System.out.println("\n preg\n\tPrint contents of register file.");
        System.out.println("\n pprg\n\tPrint disassembly of current program loaded in the CPU.");
        System.out.println("\n pasm\n\tPrint program object code.");
        System.out.println("\n pfd\n\tPrint CPU frontend states / IF stage input side values.");
        System.out.println("\n wpc <address>\n\tOverwrite program counter with <address>.");
        System.out.println("\n w <address> <value>\n\tWrite <value> to memory at <address>.");
        System.out.println("\n wbus <address> <value>\n\tWrite <value> to FSB with <address>.");
        System.out.println("\n enableio <index> ..or.. enableio\n\tEnable evaluation of I/O device <index>. Enable all if no argument is given.");
        System.out.println("\n disableio <index> ..or.. disableio\n\tDisable evaluation of I/O device <index>. Disable all if no argument is given.");
        System.out.println("\n listio\n\tList I/O modules loaded.");
        System.out.println("\n asm <address> <asm>\n\tAssemble <asm> and inject code starting at <address>.");
        System.out.println("\n silent\n\tToggle silent mode (default off).");
    }
}
