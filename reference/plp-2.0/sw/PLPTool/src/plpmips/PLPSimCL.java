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

package plpmips;

import java.io.InputStreamReader;
import java.io.BufferedReader;
import plptool.PLPMsg;
import plptool.PLPSimCore;
import plptool.PLPSimMods;
import plptool.PLPToolbox;

/**
 * PLPTool command line simulator interface for the MIPS simulation core
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

        PLPMsg.M("PLPTool Command Line Simulator");
        if(asmFile != null) {
            PLPMsg.m("Assembling " + asmFile + " ...");
            asm = new PLPAsm(null, asmFile, 0);
            if(asm.preprocess(0) == PLPMsg.PLP_OK)
                asm.assemble();
            
            if(!asm.isAssembled()) {
                PLPMsg.M("");
                PLPMsg.E("Assembly failed.", PLPMsg.PLP_ERROR_RETURN, asm);
                System.exit(PLPMsg.PLP_ERROR_RETURN);
            }
            PLPMsg.M(" OK");

        } else {
            asm = new PLPAsm("nop", "empty", 0);
            if(asm.preprocess(0) == PLPMsg.PLP_OK)
                asm.assemble();
        }
        
        PLPMsg.m("sim > ");
        
        while(!(input = stdIn.readLine().trim()).equals("q")) {
            tokens = input.split(" ");
            if(input.equals("version")) {
                PLPMsg.M(PLPMsg.versionString);
            }
            else if(tokens[0].equals("i")) {
                if(tokens.length == 1) {
                    core = new PLPMIPSSim(asm, -1);
                    core.reset();
                    init_core = true;
                    addMods(core);
                    core.printfrontend();
                    core.bus.enableiomods();
                    PLPMsg.M("Simulation core initialized with nigh-infinite RAM.");
                }
                else if(tokens.length != 2) {
                    PLPMsg.M("Usage: i <ram size in bytes>");
                }
                else {
                    try {
                    ram_size = PLPToolbox.parseNum(tokens[1]);
                   
                    if(ram_size % 4 != 0)
                        PLPMsg.M("RAM size has to be in multiples of 4");
                    else {
                        core = new PLPMIPSSim(asm, ram_size);
                        core.reset();
                        init_core = true;
                        addMods(core);
                        core.printfrontend();
                        core.bus.enableiomods();
                        PLPMsg.M("Simulation core initialized.");
                    }
                    } catch(Exception e) {
                        PLPMsg.M("" + e);
                        PLPMsg.M("Usage: i <ram size in bytes>");
                    }
                }
            }
            else if(input.equals("help") && !init_core) {
                PLPMsg.M("Run i to initialize simulation core with default RAM size.\n" +
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
                    PLPMsg.M("");
                    core.wb_stage.printinstr();
                    core.mem_stage.printinstr();
                    core.ex_stage.printinstr();
                    core.id_stage.printinstr();
                    core.printfrontend();
                    PLPMsg.M("-------------------------------------");
                }
            }
            
            else if(tokens[0].equals("s")) {
                if(tokens.length != 2) {
                    PLPMsg.M("Usage: s <number of instructions>");
                }
                else
                    for(int i = 0; i < Integer.parseInt(tokens[1]); i++) {
                        if(core.step() != PLPMsg.PLP_OK)
                        PLPMsg.E("Simulation is stale. Please reset.",
                                 PLPMsg.PLP_SIM_STALE, null);
                        else if(!silent) {
                            PLPMsg.M("");
                            core.wb_stage.printinstr();
                            core.mem_stage.printinstr();
                            core.ex_stage.printinstr();
                            core.id_stage.printinstr();
                            core.printfrontend();
                            PLPMsg.M("-------------------------------------");
                        }
                    }
            }
            else if(input.equals("r")) {
                core.reset();
                core.printfrontend();
            }
            else if(input.equals("pram")) {
                
                PLPMsg.M("\nMain memory listing");
                PLPMsg.M("===================");
                core.memory.printAll(core.pc.eval());
            }
            else if(tokens[0].equals("pram")) {
                if(tokens.length != 2) {
                    PLPMsg.M("Usage: pram <address>");
                }
                else {
                    core.memory.print(PLPToolbox.parseNum(tokens[1]));
                }
            }
            else if(input.equals("preg")) {
                long data;
                PLPMsg.M("\nRegisters listing");
                PLPMsg.M("=================");
                for(int j = 0; j < 32; j++) {
                    data = (Long) core.regfile.read(j);
                    PLPMsg.M(j + "\t" +
                                       String.format("%08x", data) + "\t" +
                                       PLPToolbox.asciiWord(data));
                }
            }
            else if(tokens[0].equals("preg")) {
                if(tokens.length != 2) {
                    PLPMsg.M("Usage: preg <address>");
                }
                else {
                    long addr = PLPToolbox.parseNum(tokens[1]);
                    core.regfile.print(addr);
                }
            }
            else if(input.equals("pfd")) {
                PLPMsg.M("\nFrontend / fetch stage state");
                PLPMsg.M("============================");
                core.printfrontend();
            }
            else if(input.equals("pprg")) {
                PLPMsg.M("\nProgram Listing");
                PLPMsg.M("===============");
                core.printProgram(core.pc.eval());
            }
            else if(input.equals("pasm")) {
                PLPAsmFormatter.prettyPrint(asm);
            }
            else if(input.equals("pinstr")) {
                PLPMsg.M("\nIn-flight instructions");
                PLPMsg.M("======================");
                core.wb_stage.printinstr();
                core.mem_stage.printinstr();
                core.ex_stage.printinstr();
                core.id_stage.printinstr();
                core.printfrontend();
            }
            else if(tokens[0].equals("wpc")) {
                if(tokens.length != 2) {
                    PLPMsg.M("Usage: wpc <address>");
                }
                else {
                    core.softreset();
                    core.pc.write(PLPAsm.sanitize32bits(tokens[1]));
                    core.printfrontend();
                }
            }
            else if(tokens[0].equals("w")) {
                if(tokens.length != 3) {
                    PLPMsg.M("Usage: w <address> <data>");
                }
                else {
                    if(core.memory.write(PLPAsm.sanitize32bits(tokens[1]),
                                      PLPAsm.sanitize32bits(tokens[2]), false) == PLPMsg.PLP_OK)
                      core.memory.print(PLPAsm.sanitize32bits(tokens[1]));
                }
            }
            else if(tokens[0].equals("wbus")) {
                if(tokens.length != 3) {
                    PLPMsg.M("Usage: wbus <address> <data>");
                }
                else {
                    core.bus.write(PLPAsm.sanitize32bits(tokens[1]),
                                      PLPAsm.sanitize32bits(tokens[2]), false);
                }
            }
            else if(tokens[0].equals("rbus")) {
                if(tokens.length != 2) {
                    PLPMsg.M("Usage: rbus <address>");
                }
                else {
                    long addr = PLPAsm.sanitize32bits(tokens[1]);
                    Object ret = core.bus.read(addr);
                    if(ret != null) {
                        long value = (Long) ret;
                        PLPMsg.M(String.format("0x%08x=", addr) +
                                           String.format("0x%08x", value));
                    }
                }
            }
            else if(input.equals("listio")) {
                for(int i = 0; i < core.bus.nummods(); i++)
                    PLPMsg.M(i + ": " +
                                       String.format("0x%08x", core.bus.iostartaddr(i)) + "-" +
                                       String.format("0x%08x", core.bus.ioendaddr(i)) + " " +
                                       core.bus.introduceio(i) +
                                       (core.bus.enabled(i) ? " (enabled)" : " (disabled)"));
            }
            else if(input.equals("enableio")) {
                core.bus.enableiomods();
            }
            else if(tokens[0].equals("enableio")) {
                if(tokens.length != 2) {
                    PLPMsg.M("Usage: enableio <index>");
                }
                else {
                    core.bus.enableio((int) PLPToolbox.parseNum(tokens[1]));
                }
            }
            else if(input.equals("evalio")) {
                core.bus.eval();
            }
            else if(tokens[0].equals("evalio")) {
                if(tokens.length != 2) {
                    PLPMsg.M("Usage: enableio <index>");
                }
                else {
                    core.bus.eval((int) PLPToolbox.parseNum(tokens[1]));
                }
            }
            else if(input.equals("disableio")) {
                core.bus.disableiomods();
            }
            else if(tokens[0].equals("disableio")) {
                if(tokens.length != 2) {
                    PLPMsg.M("Usage: disableio <index>");
                }
                else {
                    core.bus.disableio((int) PLPToolbox.parseNum(tokens[1]));
                }
            }
            else if(tokens[0].equals("cleario")) {
                if(tokens.length != 2) {
                    PLPMsg.M("Usage: cleario <index>");
                }
                else {
                    core.bus.cleario((int) PLPToolbox.parseNum(tokens[1]));
                }
            }
            else if(tokens[0].equals("j")) {
                if(tokens.length != 2) {
                    PLPMsg.M("Usage: j <address>");
                }
                else {
                    core.pc.write(PLPAsm.sanitize32bits(tokens[1]));
                    core.printfrontend();
                }
            }
            else if(tokens[0].equals("asm")) {
                if(tokens.length < 3) {
                    PLPMsg.M("Usage: asm <address> <in-line assembly>");
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
                        PLPMsg.M("\nCode injected:");
                        PLPMsg.M("==============");
                        for(int j = 0; j < inlineAsm.getObjectCode().length; j++) {
                            addr = PLPToolbox.parseNum(tokens[1]) + 4 * j;
                            core.memory.write(addr, inlineAsm.getObjectCode()[j], (inlineAsm.isInstruction(j) == 0) ? true : false);
                            PLPMsg.M(String.format("%08x", addr) +
                                               "   " + PLPToolbox.asciiWord(inlineAsm.getObjectCode()[j]) +
                                               "  " + MIPSInstr.format(inlineAsm.getObjectCode()[j]));
                        }
                    }
                }
            }
            else if(input.equals("pvars")) {
                PLPMsg.M("\nOutput side values of pipeline stages");
                PLPMsg.M("=====================================");
                core.wb_stage.printvars();
                core.mem_stage.printvars();
                core.ex_stage.printvars();
                core.id_stage.printvars();

            }
            else if(input.equals("pnextvars")) {
                PLPMsg.M("\nInput side values of pipeline registers");
                PLPMsg.M("=======================================");
                core.wb_stage.printnextvars();
                core.mem_stage.printnextvars();
                core.ex_stage.printnextvars();
                core.id_stage.printnextvars();
            }
            else if(input.equals("silent")) {
                if(silent) {
                    silent = false;
                    PLPMsg.M("Silent mode off.");
                } else {
                    silent = true;
                    PLPMsg.M("Silent mode on.");
                }
            }
            else if(input.equals("jvm")) {
                Runtime runtime = Runtime.getRuntime();
                PLPMsg.M("Free JVM memory:     " + runtime.freeMemory());
                PLPMsg.M("Total JVM memory:    " + runtime.totalMemory());
                PLPMsg.M("Total - Free (Used): "  + (runtime.totalMemory() -  runtime.freeMemory()));
            }
            else if(input.toLowerCase().equals("wira sucks")) {
                PLPMsg.M("No, he doesn't.");
            }
            else {
                simCLHelp();
            }

            PLPMsg.M("");

            
            } else {
                PLPMsg.M("Simulation core is not initialiazed. Try running the command 'i'.\n");
            }
        }
            if(!init_core)
                PLPMsg.m("sim > ");
            else
                PLPMsg.m(String.format("%08x", core.getFlags()) +
                                 " " + core.getinstrcount() +
                                 " sim > ");
        }
        PLPMsg.M("See ya!");

        } catch(Exception e) {
            System.err.println(e);
        }
    }

    public static void simCLHelp() {
        PLPMsg.M("Unknown command. Command list:");
        PLPMsg.M("\n i <RAM size in bytes> ..or.. i\n\tInit core with RAM size in bytes. Set RAM size to 2^62 if no argument is given.");
        PLPMsg.M("\n s <steps> ..or.. s\n\tAdvance <steps> number of cycles. Step 1 cycle if no argument is given.");
        PLPMsg.M("\n r\n\tReset simulated CPU (clears memory elements and reloads program).");
        PLPMsg.M("\n pinstr\n\tPrint instructions currently in-flight.");
        PLPMsg.M("\n pvars\n\tPrint pipeline registers' values.");
        PLPMsg.M("\n pnextvars\n\tPrint pipeline registers' input values.");
        PLPMsg.M("\n pram <address> ..or.. pram\n\tPrint value of RAM at <address>. Print all if no argument is given.");
        PLPMsg.M("\n preg <address> ..or.. preg\n\tPrint contents of a register or print contents of register file.");
        PLPMsg.M("\n pprg\n\tPrint disassembly of current program loaded in the CPU.");
        PLPMsg.M("\n pasm\n\tPrint program object code.");
        PLPMsg.M("\n pfd\n\tPrint CPU frontend states / IF stage input side values.");
        PLPMsg.M("\n wpc <address>\n\tOverwrite program counter with <address>.");
        PLPMsg.M("\n j <address>\n\tJump to <address>.");
        PLPMsg.M("\n w <address> <value>\n\tWrite <value> to memory at <address>.");
        PLPMsg.M("\n wbus <address> <value>\n\tWrite <value> to FSB with <address>.");
        PLPMsg.M("\n rbus <address>\n\tIssue read of <addr> to FSB.");
        PLPMsg.M("\n enableio <index> ..or.. enableio\n\tEnable evaluation of I/O device <index>. Enable all if no argument is given.");
        PLPMsg.M("\n disableio <index> ..or.. disableio\n\tDisable evaluation of I/O device <index>. Disable all if no argument is given.");
        PLPMsg.M("\n listio\n\tList I/O modules loaded.");
        PLPMsg.M("\n evalio <index> ..or.. evalio\n\tEvaluate I/O module <index>. Evaluate all if no argument is given.");
        PLPMsg.M("\n asm <address> <asm>\n\tAssemble <asm> and inject code starting at <address>.");
        PLPMsg.M("\n silent\n\tToggle silent mode (default off).");
    }

    public static void addMods(PLPSimCore core) {
        PLPSimMods mods = new PLPSimMods(core);
        core.bus.add(mods.io_leds);
        core.bus.add(mods.cache_hier);
    }
}
