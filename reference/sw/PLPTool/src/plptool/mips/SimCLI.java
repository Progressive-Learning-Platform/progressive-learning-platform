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

package plptool.mips;

import java.io.InputStreamReader;
import java.io.BufferedReader;
import plptool.Msg;
import plptool.PLPToolbox;
import plptool.Constants;
import plptool.Config;

/**
 * PLPTool command line simulator interface for the MIPS simulation core
 *
 * @author wira
 */
public class SimCLI {
    
    static boolean silent = false;

    public static void simCLCommand(String input, plptool.gui.ProjectDriver plp) {
        if(plp != null && plp.g())
            plp.g_err.clearError();

        SimCore core = (SimCore) plp.sim;
        Asm asm = (Asm) plp.asm;
        plptool.mods.IORegistry ioReg = plp.ioreg;

        String tokens[] = input.split(" ");
        if(input.equals("version")) {
            Msg.M(plptool.Constants.versionString);
        }
        if(input.equals("i")) {
        }
        else if(input.equals("s")) {
            if(core.step() != Constants.PLP_OK)
                 Msg.E("Simulation is stale. Please reset.",
                          Constants.PLP_SIM_STALE, null);
            else if(!silent) {
                Msg.M("");
                core.wb_stage.printinstr();
                core.mem_stage.printinstr();
                core.ex_stage.printinstr();
                core.id_stage.printinstr();
                core.printfrontend();
                Msg.M("-------------------------------------");
            }
        }

        else if(tokens[0].equals("s")) {
            if(tokens.length != 2) {
                Msg.M("Usage: s <number of instructions>");
            }
            else {
                Msg.lastError = 0;
                int steps = PLPToolbox.parseNumInt(tokens[1]);
                long time = 0;
                if(steps > Constants.PLP_LONG_SIM) {
                    if(!silent) {
                        Msg.M("This might take a while, turning on silent mode.");
                        silent = true;
                    }
                    time = System.currentTimeMillis();
                }
                for(int i = 0; i < steps && Msg.lastError == 0; i++) {
                    if(core.step() != Constants.PLP_OK)
                    Msg.E("Simulation is stale. Please reset.",
                             Constants.PLP_SIM_STALE, null);
                    else if(!silent) {
                        Msg.M("");
                        core.wb_stage.printinstr();
                        core.mem_stage.printinstr();
                        core.ex_stage.printinstr();
                        core.id_stage.printinstr();
                        core.printfrontend();
                        Msg.M("-------------------------------------");
                    }
                }
                if(steps > Constants.PLP_LONG_SIM)
                    Msg.M("That took " + (System.currentTimeMillis() - time) + " milliseconds.");
            }
        }
        else if(input.equals("r")) {
            core.reset();
            core.printfrontend();
        }
        else if(input.equals("pram")) {

            Msg.M("\nMain memory listing");
            Msg.M("===================");
            for(int i = 0; i < plp.ioreg.getNumOfModsAttached(); i++) {
                if(plp.ioreg.getType(i) == 0) {
                    Msg.M("Attached memory in bus position " + plp.ioreg.getPositionInBus(i));
                    ((plptool.mods.MemModule) plp.ioreg.getModule(i)).printAll(core.pc.eval());
                }
            }
        }
        else if(tokens[0].equals("pram")) {
            if(tokens.length != 2) {
                Msg.M("Usage: pram <address>");
            }
            else {
                for(int i = 0; i < plp.ioreg.getNumOfModsAttached(); i++) {
                    if(plp.ioreg.getType(i) == 0) {
                        Msg.M("Memory attached in bus position " + plp.ioreg.getPositionInBus(i));
                        ((plptool.mods.MemModule) plp.ioreg.getModule(i)).print(PLPToolbox.parseNum(tokens[1]));
                    }
                }
            }
        }
        else if(input.equals("preg")) {
            long data;
            Msg.M("\nRegisters listing");
            Msg.M("=================");
            for(int j = 0; j < 32; j++) {
                data = (Long) core.regfile.read(j);
                Msg.M(j + "\t" +
                                   String.format("%08x", data) + "\t" +
                                   PLPToolbox.asciiWord(data));
            }
        }
        else if(tokens[0].equals("preg")) {
            if(tokens.length != 2) {
                Msg.M("Usage: preg <address>");
            }
            else {
                long addr = PLPToolbox.parseNum(tokens[1]);
                core.regfile.print(addr);
            }
        }
        else if(input.equals("pfd")) {
            Msg.M("\nFrontend / fetch stage state");
            Msg.M("============================");
            core.printfrontend();
        }
        else if(tokens[0].equals("pprg")) {
            if(tokens.length != 2) {
                Msg.M("Usage: pprg <index of memory module on the BUS>");
            }
            else {
                Msg.M("\nProgram Listing");
                Msg.M("===============");
                core.printProgram(PLPToolbox.parseNumInt(tokens[1]), core.pc.eval());
            }
        }
        else if(input.equals("pasm")) {
            Formatter.prettyPrint(asm);
        }
        else if(input.equals("pinstr")) {
            Msg.M("\nIn-flight instructions");
            Msg.M("======================");
            core.wb_stage.printinstr();
            core.mem_stage.printinstr();
            core.ex_stage.printinstr();
            core.id_stage.printinstr();
            core.printfrontend();
        }
        else if(tokens[0].equals("wpc")) {
            if(tokens.length != 2) {
                Msg.M("Usage: wpc <address>");
            }
            else {
                core.softreset();
                core.pc.write(PLPToolbox.parseNum(tokens[1]));
                core.printfrontend();
            }
        }
        else if(tokens[0].equals("w")) {
            if(tokens.length != 3) {
                Msg.M("Usage: w <address> <data>");
            }
            else {
                core.bus.write(PLPToolbox.parseNum(tokens[1]),
                                  PLPToolbox.parseNum(tokens[2]), false);
            }
        }
        else if(tokens[0].equals("wbus")) {
            if(tokens.length != 3) {
                Msg.M("Usage: wbus <address> <data>");
            }
            else {
                core.bus.write(PLPToolbox.parseNum(tokens[1]),
                                  PLPToolbox.parseNum(tokens[2]), false);
            }
        }
        else if(tokens[0].equals("rbus")) {
            if(tokens.length != 2) {
                Msg.M("Usage: rbus <address>");
            }
            else {
                long addr = PLPToolbox.parseNum(tokens[1]);
                Object ret = core.bus.read(addr);
                if(ret != null) {
                    long value = (Long) ret;
                    Msg.M(String.format("0x%08x=", addr) +
                                       String.format("0x%08x", value));
                }
            }
        }
        else if(input.equals("listio")) {
            for(int i = 0; i < core.bus.getNumOfMods(); i++) {
                long start = core.bus.getModStartAddress(i);
                long end = core.bus.getModEndAddress(i);
                Msg.M(i + ": " +
                                   ((start < 0) ? "Unmapped" : String.format("0x%08x", start)) + "-" +
                                   ((end   < 0) ? "Unmapped" : String.format("0x%08x", end)) + " " +
                                   core.bus.introduceMod(i) +
                                   (core.bus.getEnabled(i) ? " (enabled)" : " (disabled)"));
            }
        }
        else if(input.equals("enableio")) {
            core.bus.enableAllModules();
        }
        else if(tokens[0].equals("enableio")) {
            if(tokens.length != 2) {
                Msg.M("Usage: enableio <index>");
            }
            else {
                core.bus.enableMod((int) PLPToolbox.parseNum(tokens[1]));
            }
        }
        else if(input.equals("evalio")) {
            core.bus.eval();
        }
        else if(tokens[0].equals("evalio")) {
            if(tokens.length != 2) {
                Msg.M("Usage: evalio <index>");
            }
            else {
                core.bus.eval((int) PLPToolbox.parseNum(tokens[1]));
            }
        }
        else if(input.equals("disableio")) {
            core.bus.disableAllModules();
        }
        else if(tokens[0].equals("disableio")) {
            if(tokens.length != 2) {
                Msg.M("Usage: disableio <index>");
            }
            else {
                core.bus.disableMod((int) PLPToolbox.parseNum(tokens[1]));
            }
        }
        else if(tokens[0].equals("cleario")) {
            if(tokens.length != 2) {
                Msg.M("Usage: cleario <index>");
            }
            else {
                core.bus.clearModRegisters((int) PLPToolbox.parseNum(tokens[1]));
            }
        }
        else if(input.equals("listmods")) {
            Msg.M("Registered modules:");
            Object modInfo[][] = ioReg.getAvailableModulesInformation();
            for(int i = 0; i < modInfo.length; i++) {
                Msg.M(i + ": " + modInfo[i][0] + " - " + modInfo[i][3]);
            }
        }
        else if(input.equals("attachedmods")) {
            Msg.M("Attached modules:");
            Object mods[] = ioReg.getAttachedModules();
            for(int i = 0; i < mods.length; i++) {
                Msg.M(i + ": "
                        + ((plptool.PLPSimBusModule)mods[i]).introduce() +
                        " - position in bus: " + ioReg.getPositionInBus(i));
            }
        }
        else if(input.equals("listpresets")) {
            Msg.M("Registered presets:");
            Object[][] presets = plptool.mods.Preset.presets;
            for(int i = 0; i < presets.length; i++) {
                Msg.M(i + ": " + presets[i][0]);
            }
        }
        else if(tokens[0].equals("loadpreset")) {
            if(tokens.length != 2) {
                Msg.M("Usage: loadpreset <index>");
            }
            else {
                plp.ioreg.loadPredefinedPreset(PLPToolbox.parseNumInt(tokens[1]));
            }
        }
        else if(tokens[0].equals("addmod")) {
            if(tokens.length != 4) {
                Msg.M("Usage: addmod <mod ID> <address> <register file size>");
            }
            else {
                ioReg.attachModuleToBus((int) PLPToolbox.parseNum(tokens[1]),
                                        PLPToolbox.parseNum(tokens[2]),
                                        PLPToolbox.parseNum(tokens[3]));
            }
        }
        else if(tokens[0].equals("rmmod")) {
            if(tokens.length != 2) {
                Msg.M("Usage: rmmod <mod index in the REGISTRY>");
            }
            else {
                ioReg.removeModule((int) PLPToolbox.parseNum(tokens[1]));
            }
        }

        else if(tokens[0].equals("j")) {
            if(tokens.length != 2) {
                Msg.M("Usage: j <address>");
            }
            else {
                core.pc.write(PLPToolbox.parseNum(tokens[1]));
                core.printfrontend();
            }
        }
        else if(tokens[0].equals("asm")) {
            if(tokens.length < 3) {
                Msg.M("Usage: asm <address> <in-line assembly>");
            }
            else {
                String iAsm = "";
                long addr;
                for(int j = 2; j < tokens.length; j++)
                    iAsm += tokens[j] + " ";
                Asm inlineAsm = new Asm(iAsm, "PLPSimCL inline asm");
                if(inlineAsm.preprocess(0) == Constants.PLP_OK)
                    inlineAsm.assemble();
                if(inlineAsm.isAssembled()) {
                    Msg.M("\nCode injected:");
                    Msg.M("==============");
                    for(int j = 0; j < inlineAsm.getObjectCode().length; j++) {
                        addr = PLPToolbox.parseNum(tokens[1]) + 4 * j;
                        core.bus.write(addr, inlineAsm.getObjectCode()[j], (inlineAsm.isInstruction(j) == 0) ? true : false);
                        Msg.M(String.format("%08x", addr) +
                                           "   " + PLPToolbox.asciiWord(inlineAsm.getObjectCode()[j]) +
                                           "  " + MIPSInstr.format(inlineAsm.getObjectCode()[j]));
                    }
                }
            }
        }
        else if(input.equals("pvars")) {
            Msg.M("\nOutput side values of pipeline stages");
            Msg.M("=====================================");
            core.wb_stage.printvars();
            core.mem_stage.printvars();
            core.ex_stage.printvars();
            core.id_stage.printvars();

        }
        else if(input.equals("pnextvars")) {
            Msg.M("\nInput side values of pipeline registers");
            Msg.M("=======================================");
            core.wb_stage.printnextvars();
            core.mem_stage.printnextvars();
            core.ex_stage.printnextvars();
            core.id_stage.printnextvars();
        }
        else if(input.equals("silent")) {
            if(silent) {
                silent = false;
                Msg.M("Silent mode off.");
            } else {
                silent = true;
                Msg.M("Silent mode on.");
            }
        }
        else if(input.equals("cycleaccurate")) {
            if(Config.simFunctional) {
                Config.simFunctional = false;
                Msg.M("Functional simulation mode off.");
            } else {
                Config.simFunctional = true;
                Msg.M("Functional simulation mode on.");
            }
        }
        else if(input.equals("mod_forwarding")) {
            Msg.M("EX->EX R-type: " + core.forwarding.ex_ex_rtype);
            Msg.M("EX->EX I-type: " + core.forwarding.ex_ex_itype);
            Msg.M("MEM->EX R-type: " + core.forwarding.mem_ex_rtype);
            Msg.M("MEM->EX I-type: " + core.forwarding.mem_ex_itype);
            Msg.M("MEM->EX LW-use: " + core.forwarding.mem_ex_lw);
        }
        else if(input.equals("flags")) {
            long f = core.getFlags();
            if((f & Constants.PLP_SIM_FWD_EX_EX_ITYPE) == Constants.PLP_SIM_FWD_EX_EX_ITYPE)
                Msg.M("PLP_SIM_FWD_EX_EX_ITYPE");
            if((f & Constants.PLP_SIM_FWD_EX_EX_RTYPE) == Constants.PLP_SIM_FWD_EX_EX_RTYPE)
                Msg.M("PLP_SIM_FWD_EX_EX_RTYPE");
            if((f & Constants.PLP_SIM_FWD_MEM_EX_RTYPE) == Constants.PLP_SIM_FWD_MEM_EX_RTYPE)
                Msg.M("PLP_SIM_FWD_MEM_EX_RTYPE");
            if((f & Constants.PLP_SIM_FWD_MEM_EX_ITYPE) == Constants.PLP_SIM_FWD_MEM_EX_ITYPE)
                Msg.M("PLP_SIM_FWD_MEM_EX_ITYPE");
            if((f & Constants.PLP_SIM_FWD_MEM_EX_LW) == Constants.PLP_SIM_FWD_MEM_EX_LW)
                Msg.M("PLP_SIM_FWD_MEM_EX_LW");
            if((f & Constants.PLP_SIM_FWD_MEM_MEM) == Constants.PLP_SIM_FWD_MEM_MEM)
                Msg.M("PLP_SIM_FWD_MEM_MEM");
            if((f & Constants.PLP_SIM_IF_STALL_SET) == Constants.PLP_SIM_IF_STALL_SET)
                Msg.M("PLP_SIM_IF_STALL_SET");
            if((f & Constants.PLP_SIM_ID_STALL_SET) == Constants.PLP_SIM_ID_STALL_SET)
                Msg.M("PLP_SIM_ID_STALL_SET");
            if((f & Constants.PLP_SIM_EX_STALL_SET) == Constants.PLP_SIM_EX_STALL_SET)
                Msg.M("PLP_SIM_EX_STALL_SET");
            if((f & Constants.PLP_SIM_MEM_STALL_SET) == Constants.PLP_SIM_MEM_STALL_SET)
                Msg.M("PLP_SIM_MEM_STALL_SET");

        }
        else if(input.equals("jvm")) {
            Runtime runtime = Runtime.getRuntime();
            Msg.M("Free JVM memory:     " + runtime.freeMemory());
            Msg.M("Total JVM memory:    " + runtime.totalMemory());
            Msg.M("Total - Free (Used): "  + (runtime.totalMemory() -  runtime.freeMemory()));
        }
        else if(input.toLowerCase().equals("wira sucks")) {
            Msg.M("No, he doesn't.");
        }
        else if(input.equals("help sim")) {
            simCLHelp(1);
        }
        else if(input.equals("help print")) {
            simCLHelp(2);
        }
        else if(input.equals("help bus")) {
            simCLHelp(3);
        }
        else if(input.equals("help mods")) {
            simCLHelp(4);
        }
        else if(input.equals("help misc")) {
            simCLHelp(5);
        }
        else {
            simCLHelp(0);
        }

        Msg.M("");

        if(Msg.lastError != 0 && plp.g())
            plp.g_err.setError(Msg.lastError);

        Msg.m(String.format("%08x", core.getFlags()) +
                             " " + core.getInstrCount() +
                             " sim > ");
    }

    public static void simCL(plptool.gui.ProjectDriver plp) {
        try {

        String input;
        BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
        Msg.M("Welcome to PLP MIPS Simulator Command Line Interface");
        Msg.M("Reset vector: " + String.format("0x%08x", plp.asm.getEntryPoint()));
        Msg.m(String.format("\n%08x", plp.sim.getFlags()) +
                             " " + plp.sim.getInstrCount() +
                             " sim > ");

        while(!(input = stdIn.readLine().trim()).equals("q"))
           simCLCommand(input, plp);

        plp.ioreg.removeAllModules();
        Msg.M("See ya!");

        } catch(Exception e) {
            System.err.println(e);
        }
    }

    public static void simCLHelp(int commandGroup) {
        switch(commandGroup) {
            case 0:
                Msg.M("\nPLPTool MIPS Command Line Interface Help.");
                Msg.M("\n help sim\n\tList general simulator commands.");
                Msg.M("\n help print\n\tList commands to print various simulator information to screen.");
                Msg.M("\n help bus\n\tList I/O bus commands.");
                Msg.M("\n help mods\n\tList PLP modules commands.");
                Msg.M("\n help misc\n\tMiscellaneous commands.");

                break;

            case 1:
                Msg.M("\nGeneral Simulation Control.");
                Msg.M("\n s <steps> ..or.. s\n\tAdvance <steps> number of cycles. Step 1 cycle if no argument is given.");
                Msg.M("\n r\n\tReset simulated CPU (clears memory elements and reloads program).");
                Msg.M("\n wpc <address>\n\tOverwrite program counter with <address>.");
                Msg.M("\n j <address>\n\tJump to <address>.");
                Msg.M("\n w <address> <value>\n\tWrite <value> to memory at <address>.");
                Msg.M("\n flags\n\tPrint out simulation flags that are set.");

                break;

            case 2:
                Msg.M("\nPrint information to screen.");
                Msg.M("\n pinstr\n\tPrint instructions currently in-flight.");
                Msg.M("\n pvars\n\tPrint pipeline registers' values.");
                Msg.M("\n pnextvars\n\tPrint pipeline registers' input values.");
                Msg.M("\n pram <address> ..or.. pram\n\tPrint value of RAM at <address>. Print all if no argument is given.");
                Msg.M("\n preg <address> ..or.. preg\n\tPrint contents of a register or print contents of register file.");
                Msg.M("\n pprg <index>\n\tPrint disassembly of the contents of memory module with bus location specified by <index>");
                Msg.M("\n pasm\n\tPrint program object code.");
                Msg.M("\n pfd\n\tPrint CPU frontend states / IF stage input side values.");

                break;

            case 3:
                Msg.M("\nBus control.");
                Msg.M("\n wbus <address> <value>\n\tWrite <value> to FSB with <address>.");
                Msg.M("\n rbus <address>\n\tIssue read of <addr> to FSB.");
                Msg.M("\n enableio <index> ..or.. enableio\n\tEnable evaluation of I/O device <index>. Enable all if no argument is given.");
                Msg.M("\n disableio <index> ..or.. disableio\n\tDisable evaluation of I/O device <index>. Disable all if no argument is given.");
                Msg.M("\n listio\n\tList I/O modules loaded.");
                Msg.M("\n evalio <index> ..or.. evalio\n\tEvaluate I/O module <index>. Evaluate all if no argument is given.");

                break;

            case 4:
                Msg.M("\nModule management.");
                Msg.M("\n listmods\n\tList avaiable modules in the PLP I/O registry.");
                Msg.M("\n addmod <mod ID> <address> <regfile size>\n\tAttach module with <mod ID> starting at <address> with <regfile size> to the registry and the bus.");
                Msg.M("\n rmmod <index in the REGISTRY>\n\tRemove the module with <index in the REGISTRY> from the registry and the bus.");
                Msg.M("\n attachedmods\n\tList all modules attached to the simulation.");

                break;

            case 5:
                Msg.M("\nMiscellaneous.");
                Msg.M("\n asm <address> <asm>\n\tAssemble <asm> and inject code starting at <address>.");
                Msg.M("\n silent\n\tToggle silent mode (default off).");
                Msg.M("\n cycleaccurate\n\tToggle cycle-accurate simulation mode (default off).");

                break;
        }
    }
}
