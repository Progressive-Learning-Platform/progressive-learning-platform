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
    static boolean prompt = true;
    static SimCore core;
    static Asm asm;
    static plptool.mods.IORegistry ioReg;
    static String input;
    static String[] tokens;

    public static void simCLCommand(String cmd, plptool.gui.ProjectDriver plp) {
        if(plp != null && plp.g())
            plp.g_err.clearError();

        core = (SimCore) plp.sim;
        asm = (Asm) plp.asm;
        ioReg = plp.ioreg;
        input = cmd.trim();

        tokens = input.split(" ");
        if(input.equals("version")) {
            Msg.P(plptool.Text.versionString);
        }
        else if(input.equals("i")) {
        }
        else if(input.equals("s"))                  cmd_s();
        else if(tokens[0].equals("s"))              cmd_s_arg();
        else if(input.equals("r"))                  cmd_r();
        else if(input.equals("pram"))               cmd_pram();
        else if(tokens[0].equals("pram"))           cmd_pram_arg();
        else if(input.equals("preg"))               cmd_preg();
        else if(tokens[0].equals("preg"))           cmd_preg_arg();
        else if(input.equals("pfd"))                cmd_pfd();
        else if(tokens[0].equals("pprg"))           cmd_pprg();
        else if(input.equals("pasm"))               cmd_pasm();
        else if(input.equals("pinstr"))             cmd_pinstr();
        else if(tokens[0].equals("wpc"))            cmd_wpc();
        else if(tokens[0].equals("w"))              cmd_w();
        else if(tokens[0].equals("wbus"))           cmd_wbus();
        else if(tokens[0].equals("rbus"))           cmd_rbus();
        else if(input.equals("listio"))             cmd_listio();
        else if(input.equals("enableio"))           cmd_enableio();
        else if(tokens[0].equals("enableio"))       cmd_enableio_arg();
        else if(input.equals("evalio"))             cmd_evalio();
        else if(tokens[0].equals("evalio"))         cmd_evalio_arg();
        else if(input.equals("disableio"))          cmd_disableio();
        else if(tokens[0].equals("disableio"))      cmd_disableio_arg();
        else if(tokens[0].equals("cleario"))        cmd_cleario();
        else if(input.equals("listmods"))           cmd_listmods();
        else if(input.equals("listdmods"))          cmd_listdmods();
        else if(input.equals("attachedmods"))       cmd_attachedmods();
        else if(input.equals("listpresets"))        cmd_listpresets();
        else if(tokens[0].equals("loadpreset"))     cmd_loadpreset();
        else if(tokens[0].equals("addmod"))         cmd_addmod();
        else if(tokens[0].equals("adddmod"))        cmd_adddmod();
        else if(tokens[0].equals("rmmod"))          cmd_rmmod();
        else if(tokens[0].equals("j"))              cmd_j();
        else if(tokens[0].equals("asm"))            cmd_asm();
        else if(input.equals("pvars"))              cmd_pvars();
        else if(input.equals("pnextvars"))          cmd_pnextvars();
        else if(input.equals("silent"))             cmd_silent();
        else if(input.equals("cycleaccurate"))      cmd_cycleaccurate();
        else if(tokens[0].equals("bp"))             cmd_bp();
        else if(tokens[0].equals("assert"))         cmd_assert();
        else if(tokens[0].equals("assertreg"))      cmd_assertreg();
        else if(tokens[0].equals("echo"))           cmd_echo();
        else if(input.equals("flags"))              cmd_flags();
        else if(input.equals("pflags"))             cmd_pflags();
        else if(input.equals("fwd"))                cmd_fwd();
        else if(input.equals("jvm"))                cmd_jvm();
        else if(input.equals("help"))               simCLHelp(0);
        else if(input.equals("help sim"))           simCLHelp(1);
        else if(input.equals("help print"))         simCLHelp(2);
        else if(input.equals("help bus"))           simCLHelp(3);
        else if(input.equals("help mods"))          simCLHelp(4);
        else if(input.equals("help misc"))          simCLHelp(5);
        else if(input.equals("help bp"))            simCLHelp(6);
        else if(input.toLowerCase().equals("wira sucks"))   Msg.P("No, he doesn't.");
        else Msg.M("Unknown command: " + input);
        
        if(Msg.lastError != 0 && plp.g())
            plp.g_err.setError(Msg.lastError);        
    }

    public static void cmd_s() {
        if(core.stepW() != Constants.PLP_OK)
             Msg.E("Simulation is stale. Please reset.",
                      Constants.PLP_SIM_STALE, null);
        else if(!silent) {
            Msg.P("");
            Msg.p(core.wb_stage.printinstr());
            Msg.p(core.mem_stage.printinstr());
            Msg.p(core.ex_stage.printinstr());
            Msg.p(core.id_stage.printinstr());
            core.printfrontend();
            Msg.P();
            Msg.P("-------------------------------------");
        }
    }

    public static void cmd_s_arg() {
        if(tokens.length != 2) {
            Msg.P("Usage: s <number of instructions>");
        }
        else {
            Msg.lastError = 0;
            boolean breakpoint = false;
            long startCount = 0;
            int steps = PLPToolbox.parseNumInt(tokens[1]);
            long time = 0;
            if(steps > Constants.PLP_LONG_SIM) {
                if(!silent) {
                    Msg.P("This might take a while, turning on silent mode.");
                    silent = true;
                }
                startCount = core.getInstrCount();
                time = System.currentTimeMillis();
            }
            for(int i = 0; i < steps && Msg.lastError == 0 && !breakpoint; i++) {
                if(core.stepW() != Constants.PLP_OK)
                    Msg.E("Simulation is stale. Please reset.",
                             Constants.PLP_SIM_STALE, null);
                else if(!silent) {
                    Msg.P("");
                    Msg.p(core.wb_stage.printinstr());
                    Msg.p(core.mem_stage.printinstr());
                    Msg.p(core.ex_stage.printinstr());
                    Msg.p(core.id_stage.printinstr());
                    core.printfrontend();
                    Msg.P();
                    Msg.P("-------------------------------------");
                }
                breakpoint = core.breakpoints.isBreakpoint(core.visibleAddr);
                if(core.breakpoints.isBreakpoint(core.visibleAddr))
                    Msg.P("--- stopping at breakpoint: " + PLPToolbox.format32Hex(core.visibleAddr));
            }
            if(steps > Constants.PLP_LONG_SIM) {
                startCount = core.getInstrCount() - startCount;
                Msg.P("--- executed " + startCount + " instructions in " +
                      (System.currentTimeMillis() - time) + " milliseconds.");
            }
        }
    }

    public static void cmd_r() {
        core.reset();
        core.printfrontend();
        Msg.P();
    }

    public static void cmd_wpc() {
        if(tokens.length != 2) {
            Msg.P("Usage: wpc <address>");
        } else {
            core.softreset();
            core.pc.write(PLPToolbox.parseNum(tokens[1]));
            core.printfrontend();
            Msg.P();
        }
    }

    public static void cmd_j() {
        if(tokens.length != 2) {
            Msg.P("Usage: j <address>");
        }
        else {
            core.pc.write(PLPToolbox.parseNum(tokens[1]));
            core.printfrontend();
            Msg.P();
        }
    }

    public static void cmd_w() {
        if(tokens.length != 3) {
            Msg.P("Usage: w <address> <data>");
        } else {
            core.bus.write(PLPToolbox.parseNum(tokens[1]),
                              PLPToolbox.parseNum(tokens[2]), false);
        }
    }

    public static void cmd_flags() {
        long f = core.getFlags();
        if((f & SimCore.PLP_SIM_FWD_EX_EX_RT) == SimCore.PLP_SIM_FWD_EX_EX_RT)
            Msg.P("PLP_SIM_FWD_EX_EX_RT");
        if((f & SimCore.PLP_SIM_FWD_EX_EX_RS) == SimCore.PLP_SIM_FWD_EX_EX_RS)
            Msg.P("PLP_SIM_FWD_EX_EX_RS");
        if((f & SimCore.PLP_SIM_FWD_MEM_EX_RT) == SimCore.PLP_SIM_FWD_MEM_EX_RT)
            Msg.P("PLP_SIM_FWD_MEM_EX_RT");
        if((f & SimCore.PLP_SIM_FWD_MEM_EX_RS) == SimCore.PLP_SIM_FWD_MEM_EX_RS)
            Msg.P("PLP_SIM_FWD_MEM_EX_RS");
        if((f & SimCore.PLP_SIM_FWD_MEM_EX_LW_RT) == SimCore.PLP_SIM_FWD_MEM_EX_LW_RT)
            Msg.P("PLP_SIM_FWD_MEM_EX_LW_RT");
        if((f & SimCore.PLP_SIM_FWD_MEM_EX_LW_RS) == SimCore.PLP_SIM_FWD_MEM_EX_LW_RS)
            Msg.P("PLP_SIM_FWD_MEM_EX_LW_RS");
        if((f & SimCore.PLP_SIM_FWD_MEM_MEM) == SimCore.PLP_SIM_FWD_MEM_MEM)
            Msg.P("PLP_SIM_FWD_MEM_MEM");
        if((f & SimCore.PLP_SIM_IF_STALL_SET) == SimCore.PLP_SIM_IF_STALL_SET)
            Msg.P("PLP_SIM_IF_STALL_SET");
        if((f & SimCore.PLP_SIM_ID_STALL_SET) == SimCore.PLP_SIM_ID_STALL_SET)
            Msg.P("PLP_SIM_ID_STALL_SET");
        if((f & SimCore.PLP_SIM_EX_STALL_SET) == SimCore.PLP_SIM_EX_STALL_SET)
            Msg.P("PLP_SIM_EX_STALL_SET");
        if((f & SimCore.PLP_SIM_MEM_STALL_SET) == SimCore.PLP_SIM_MEM_STALL_SET)
            Msg.P("PLP_SIM_MEM_STALL_SET");
    }

    public static void cmd_pflags() {
        long f = core.getPreviousFlags();
        if((f & SimCore.PLP_SIM_FWD_EX_EX_RT) == SimCore.PLP_SIM_FWD_EX_EX_RT)
            Msg.P("PLP_SIM_FWD_EX_EX_RT");
        if((f & SimCore.PLP_SIM_FWD_EX_EX_RS) == SimCore.PLP_SIM_FWD_EX_EX_RS)
            Msg.P("PLP_SIM_FWD_EX_EX_RS");
        if((f & SimCore.PLP_SIM_FWD_MEM_EX_RT) == SimCore.PLP_SIM_FWD_MEM_EX_RT)
            Msg.P("PLP_SIM_FWD_MEM_EX_RT");
        if((f & SimCore.PLP_SIM_FWD_MEM_EX_RS) == SimCore.PLP_SIM_FWD_MEM_EX_RS)
            Msg.P("PLP_SIM_FWD_MEM_EX_RS");
        if((f & SimCore.PLP_SIM_FWD_MEM_EX_LW_RT) == SimCore.PLP_SIM_FWD_MEM_EX_LW_RT)
            Msg.P("PLP_SIM_FWD_MEM_EX_LW_RT");
        if((f & SimCore.PLP_SIM_FWD_MEM_EX_LW_RS) == SimCore.PLP_SIM_FWD_MEM_EX_LW_RS)
            Msg.P("PLP_SIM_FWD_MEM_EX_LW_RS");
        if((f & SimCore.PLP_SIM_FWD_MEM_MEM) == SimCore.PLP_SIM_FWD_MEM_MEM)
            Msg.P("PLP_SIM_FWD_MEM_MEM");
        if((f & SimCore.PLP_SIM_IF_STALL_SET) == SimCore.PLP_SIM_IF_STALL_SET)
            Msg.P("PLP_SIM_IF_STALL_SET");
        if((f & SimCore.PLP_SIM_ID_STALL_SET) == SimCore.PLP_SIM_ID_STALL_SET)
            Msg.P("PLP_SIM_ID_STALL_SET");
        if((f & SimCore.PLP_SIM_EX_STALL_SET) == SimCore.PLP_SIM_EX_STALL_SET)
            Msg.P("PLP_SIM_EX_STALL_SET");
        if((f & SimCore.PLP_SIM_MEM_STALL_SET) == SimCore.PLP_SIM_MEM_STALL_SET)
            Msg.P("PLP_SIM_MEM_STALL_SET");
    }

    public static void cmd_fwd() {
        Msg.P("EX->EX: " + core.ex_ex);
        Msg.P("MEM->EX: " + core.mem_ex);
        Msg.P("MEM->EX LW-use: " + core.mem_ex_lw);
    }

    public static void cmd_pinstr() {
        Msg.P("\nIn-flight instructions");
        Msg.P("======================");
        Msg.p(core.wb_stage.printinstr());
        Msg.p(core.mem_stage.printinstr());
        Msg.p(core.ex_stage.printinstr());
        Msg.p(core.id_stage.printinstr());
        core.printfrontend();
        Msg.P();
    }

    public static void cmd_pvars() {
        Msg.P("\nOutput side values of pipeline stages");
        Msg.P("=====================================");
        core.wb_stage.printvars();
        core.mem_stage.printvars();
        core.ex_stage.printvars();
        core.id_stage.printvars();
    }

    public static void cmd_pnextvars() {
        Msg.P("\nInput side values of pipeline registers");
        Msg.P("=======================================");
        core.wb_stage.printnextvars();
        core.mem_stage.printnextvars();
        core.ex_stage.printnextvars();
        core.id_stage.printnextvars();
    }

    public static void cmd_pram() {
        Msg.P("\nMain memory listing");
        Msg.P("===================");
        for(int i = 0; i < ioReg.getNumOfModsAttached(); i++) {
            if(ioReg.getType(i) == 0) {
                Msg.P("Attached memory in bus position " + ioReg.getPositionInBus(i));
                ((plptool.mods.MemModule) ioReg.getModule(i)).printAll(core.pc.eval());
            }
        }
    }

    public static void cmd_pram_arg() {
        if(tokens.length != 2) {
            Msg.P("Usage: pram <address>");
        } else {
            for(int i = 0; i < ioReg.getNumOfModsAttached(); i++) {
                if(ioReg.getType(i) == 0) {
                    Msg.P("Memory attached in bus position " + ioReg.getPositionInBus(i));
                    ((plptool.mods.MemModule) ioReg.getModule(i)).print(PLPToolbox.parseNum(tokens[1]));
                }
            }
        }
    }

    public static void cmd_preg() {
        long data;
        Msg.p("\nRegisters listing");
        Msg.p("=================");
        for(int j = 0; j < 32; j++) {
            data = (Long) core.regfile.read(j);
            Msg.p(j + "\t" +
                               String.format("%08x", data) + "\t" +
                               PLPToolbox.asciiWord(data));
        }
        Msg.P();
    }

    public static void cmd_preg_arg() {
        if(tokens.length != 2) {
            Msg.P("Usage: preg <address>");
        } else {
            long addr = PLPToolbox.parseNum(tokens[1]);
            core.regfile.print(addr);
        }
    }

    public static void cmd_pprg() {
        if(tokens.length != 2) {
            Msg.P("Usage: pprg <index of memory module on the BUS>");
        } else {
            Msg.P("\nProgram Listing");
            Msg.P("===============");
            core.printProgram(PLPToolbox.parseNumInt(tokens[1]), core.pc.eval());
        }
    }

    public static void cmd_pasm() {
        Formatter.prettyPrint(asm);
    }

    public static void cmd_pfd() {
        Msg.P("\nFrontend / fetch stage state");
        Msg.P("============================");
        core.printfrontend();
        Msg.P();
    }

    public static void cmd_wbus() {
        if(tokens.length != 3) {
            Msg.P("Usage: wbus <address> <data>");
        } else {
            long addr = PLPToolbox.tryResolveLabel(tokens[1], asm);
            if(addr > -1)
                core.bus.write(addr, PLPToolbox.parseNum(tokens[2]), false);
            else
                Msg.E("Invalid address.", Constants.PLP_NUMBER_ERROR, null);
        }
    }

    public static void cmd_rbus() {
        if(tokens.length != 2) {
            Msg.P("Usage: rbus <address>");
        } else {
            long addr = PLPToolbox.tryResolveLabel(tokens[1], asm);
            if(addr > -1) {
                Object ret = core.bus.read(addr);
                if(ret != null) {
                    long value = (Long) ret;
                    Msg.P(String.format("0x%08x=", addr) +
                                       String.format("0x%08x", value));
                }
            } else
                Msg.E("Invalid address.", Constants.PLP_NUMBER_ERROR, null);
        }
    }

    public static void cmd_enableio() {
        core.bus.enableAllModules();
    }

    public static void cmd_enableio_arg() {
        if(tokens.length != 2) {
            Msg.P("Usage: enableio <index>");
        } else {
            core.bus.enableMod((int) PLPToolbox.parseNum(tokens[1]));
        }
    }

    public static void cmd_disableio() {
        core.bus.disableAllModules();
    }

    public static void cmd_disableio_arg() {
        if(tokens.length != 2) {
            Msg.P("Usage: disableio <index>");
        } else {
            core.bus.disableMod((int) PLPToolbox.parseNum(tokens[1]));
        }
    }

    public static void cmd_listio() {
        for(int i = 0; i < core.bus.getNumOfMods(); i++) {
            long start = core.bus.getModStartAddress(i);
            long end = core.bus.getModEndAddress(i);
            Msg.P(i + ":\t" +
                               ((start < 0) ? " Unmapped " : String.format("0x%08x", start)) + "-" +
                               ((end   < 0) ? " Unmapped " : String.format("0x%08x", end)) + "\t" +
                               core.bus.introduceMod(i) +
                               (core.bus.getEnabled(i) ? " (enabled)" : " (disabled)"));
        }
    }

    public static void cmd_evalio() {
        core.bus.eval();
    }

    public static void cmd_evalio_arg() {
        if(tokens.length != 2) {
            Msg.P("Usage: evalio <index>");
        } else {
            core.bus.eval((int) PLPToolbox.parseNum(tokens[1]));
        }
    }

    public static void cmd_cleario() {
        if(tokens.length != 2) {
            Msg.P("Usage: cleario <index>");
        } else {
            core.bus.clearModRegisters((int) PLPToolbox.parseNum(tokens[1]));
        }
    }

    public static void cmd_listmods() {
        Msg.P("Registered modules:");
        Object modInfo[][] = ioReg.getAvailableModulesInformation();
        for(int i = 0; i < modInfo.length; i++) {
            Msg.P(i + ": " + modInfo[i][0] + " - " + modInfo[i][3]);
        }
    }

    public static void cmd_addmod() {
        if(tokens.length != 4) {
            Msg.P("Usage: addmod <mod ID> <address> <register file size>");
        } else {
            ioReg.attachModuleToBus((int) PLPToolbox.parseNum(tokens[1]),
                                    PLPToolbox.parseNum(tokens[2]),
                                    PLPToolbox.parseNum(tokens[3]));
        }
    }

    public static void cmd_listdmods() {
        Msg.P("Registered dynamic modules:");
        for(int i = 0; i < plptool.dmf.DynamicModuleFramework.getNumberOfClasses(); i++) {
            Class c = plptool.dmf.DynamicModuleFramework.getDynamicModuleClass(i);
            Class sc = c.getSuperclass();
            Msg.P(i + ":\t" + c.getName());
            Msg.P((sc != null) ? " extends " + sc.getName() : "");

        }
    }

    public static void cmd_adddmod() {
        if(tokens.length != 4) {
            Msg.P("Usage: adddmod <dynamic mod ID> <start address> <end address>");
        } else {
            int index = PLPToolbox.parseNumInt(tokens[1]);
            long startAddr = PLPToolbox.parseNum(tokens[2]);
            long endAddr = PLPToolbox.parseNum(tokens[3]) + startAddr;
            ioReg.attachDynamicModule(index, startAddr, endAddr, true, null);
        }
    }

    public static void cmd_rmmod() {
        if(tokens.length != 2) {
            Msg.P("Usage: rmmod <mod index in the REGISTRY>");
        } else {
            ioReg.removeModule((int) PLPToolbox.parseNum(tokens[1]));
        }
    }

    public static void cmd_attachedmods() {
        Msg.P("Attached modules:");
        Object mods[] = ioReg.getAttachedModules();
        for(int i = 0; i < mods.length; i++) {
            Msg.P(i + ": "
                    + ((plptool.PLPSimBusModule)mods[i]).introduce() +
                    " - position in bus: " + ioReg.getPositionInBus(i));
        }
    }

    public static void cmd_listpresets() {
        Msg.P("Registered presets:");
        Object[][] presets = plptool.mods.Preset.presets;
        for(int i = 0; i < presets.length; i++) {
            Msg.P(i + ": " + presets[i][0]);
        }
    }

    public static void cmd_loadpreset() {
        if(tokens.length != 2) {
            Msg.P("Usage: loadpreset <index>");
        }
        else {
            ioReg.loadPredefinedPreset(PLPToolbox.parseNumInt(tokens[1]));
        }
    }

    public static void cmd_asm() {
        if(tokens.length < 3) {
            Msg.P("Usage: asm <address> <in-line assembly>");
        } else {
            String iAsm = "";
            long addr;
            long base = PLPToolbox.tryResolveLabel(tokens[1], asm);
            for(int j = 2; j < tokens.length; j++)
                iAsm += tokens[j] + " ";
            Asm inlineAsm = new Asm(iAsm, "PLPSimCL inline asm");
            if(inlineAsm.preprocess(0) == Constants.PLP_OK) {
                inlineAsm.setSymTable(asm.getSymTable());
                inlineAsm.assemble();
            }
            if(inlineAsm.isAssembled() && base > -1) {
                Msg.P("\nCode injected:");
                Msg.P("==============");
                for(int j = 0; j < inlineAsm.getObjectCode().length; j++) {
                    addr = base + 4 * j;
                    core.bus.write(addr, inlineAsm.getObjectCode()[j], (inlineAsm.isInstruction(j) == 0) ? true : false);
                    Msg.P(String.format("%08x", addr) +
                                       "   " + PLPToolbox.asciiWord(inlineAsm.getObjectCode()[j]) +
                                       "  " + MIPSInstr.format(inlineAsm.getObjectCode()[j]));
                }
            } else
                Msg.E("Code injection failed.", Constants.PLP_GENERIC_ERROR, null);
        }
    }

    public static void cmd_silent() {
        if(silent) {
            silent = false;
            Msg.P("Silent mode off.");
        } else {
            silent = true;
            Msg.P("Silent mode on.");
        }
    }

    public static void cmd_cycleaccurate() {
        if(Config.simFunctional) {
            Config.simFunctional = false;
            Msg.P("Functional simulation mode off.");
        } else {
            Config.simFunctional = true;
            Msg.P("Functional simulation mode on.");
        }
    }

    public static void cmd_assert() {
        if(tokens.length == 4) {
            int index = PLPToolbox.parseNumInt(tokens[1]);
            long addr = PLPToolbox.tryResolveLabel(tokens[2], asm);
            long assertvalue = PLPToolbox.parseNum(tokens[3]);
            Long value;
            if(index > -1 && addr > -1 && assertvalue > -1) {
                value = (Long) core.bus.getRefMod(index).read(addr);
                if(value != null && (long) value == assertvalue)
                    Msg.P("True");
                else
                    Msg.P("False");
            } else
                Msg.P("False");
        } else
            Msg.P("Usage: assert <module index in the BUS> <address/label> <value>");
    }

    public static void cmd_assertreg() {
        if(tokens.length == 3) {
            long addr = PLPToolbox.parseNum(tokens[1]);
            long assertvalue = PLPToolbox.parseNum(tokens[2]);
            Long value;
            if(addr > -1 && assertvalue > -1) {
                value = (Long) core.regfile.read(addr);
                if(value != null && (long) value == assertvalue)
                    Msg.P("True");
                else
                    Msg.P("False");
            } else
                Msg.P("False");
        } else
            Msg.P("Usage: assertreg <register address> <value>");
    }

    public static void cmd_echo() {
        if(tokens.length == 1)
            Msg.P("");
        else
            Msg.P(input.substring(5, input.length()));
    }

    public static void cmd_bp() {
        if(tokens.length < 2) {
            simCLHelp(6);
        } else if(tokens[1].equals("set") && tokens.length == 3) {
            // check if label instead of an address is used
            long addrLabel = asm.resolveAddress(tokens[2]);
            long addr = (addrLabel == -1) ? PLPToolbox.parseNumSilent(tokens[2]) : addrLabel;
            if(addr > -1) {
                core.breakpoints.add(addr, asm.getFileIndex(addr), asm.getLineNum(addr));
                Msg.P("Breakpoint set at " + PLPToolbox.format32Hex(addr) + (addrLabel != -1 ? " (" + tokens[2] + ")" : ""));
            } else
                Msg.E("'" + tokens[2] + "' is not a valid number or label.",
                        Constants.PLP_GENERIC_ERROR, null);
        } else if(tokens[1].equals("list")) {
            Msg.P("Breakpoints:");
            long addr;
            String label;
            for(int i = 0; i < core.breakpoints.size(); i++) {
                addr = core.breakpoints.getBreakpointAddress(i);
                label = asm.lookupLabel(addr);
                Msg.P(i + "\t" + PLPToolbox.format32Hex(addr) + (label != null ? " " + label : ""));
            }
        } else if(tokens[1].equals("clear")) {
            core.breakpoints.clear();
        } else if(tokens[1].equals("remove") && tokens.length == 3) {
            core.breakpoints.removeBreakpoint(PLPToolbox.parseNumInt(tokens[2]));
        } else
            simCLHelp(6);
    }

    public static void cmd_jvm() {
        Runtime runtime = Runtime.getRuntime();
        Msg.P("Free JVM memory:     " + runtime.freeMemory());
        Msg.P("Total JVM memory:    " + runtime.totalMemory());
        Msg.P("Total - Free (Used): "  + (runtime.totalMemory() -  runtime.freeMemory()));
    }

    public static void simCL(plptool.gui.ProjectDriver plp) {
        try {

        BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
        Msg.P("Welcome to PLP CPU Simulator Command Line Interface");
        Msg.P("Reset vector: " + String.format("0x%08x", plp.asm.getEntryPoint()));
        Msg.m(String.format("\n%08x", plp.sim.getFlags()) +
                             " " + plp.sim.getInstrCount() +
                             " sim > ");

        while(!(input = stdIn.readLine().trim()).equals("q")) {
            simCLCommand(input, plp);
           
            if(prompt) {
                Msg.P("");
                Msg.m(String.format("%08x", plp.sim.getFlags()) +
                                    " " + plp.sim.getInstrCount() +
                                    " sim > ");
            }
        }

        plp.ioreg.removeAllModules();

        } catch(Exception e) {
            System.err.println(e);
        }
    }

    public static void simCLHelp(int commandGroup) {
        switch(commandGroup) {
            case 0:
                Msg.P("\nPLP CPU Simulator Command Line Interface Help.");
                Msg.P("\n help sim\n\tList general simulator commands.");
                Msg.P("\n help print\n\tList commands to print various simulator information to screen.");
                Msg.P("\n help bus\n\tList I/O bus commands.");
                Msg.P("\n help bp\n\tList breakpoint commands.");
                Msg.P("\n help mods\n\tList PLP modules commands.");
                Msg.P("\n help misc\n\tMiscellaneous commands.");

                break;

            case 1:
                Msg.P("\nGeneral Simulation Control.");
                Msg.P("\n s <steps> ..or.. s\n\tAdvance <steps> number of cycles. Step 1 cycle if no argument is given.");
                Msg.P("\n r\n\tReset simulated CPU (clears memory elements and reloads program).");
                Msg.P("\n wpc <address>\n\tOverwrite program counter with <address>.");
                Msg.P("\n j <address>\n\tJump to <address>.");
                Msg.P("\n w <address> <value>\n\tWrite <value> to memory at <address>.");
                Msg.P("\n flags\n\tPrint out simulation flags that are set.");
                Msg.P("\n pflags\n\tPrint out simulation flags that were set in the previous step.");

                break;

            case 2:
                Msg.P("\nPrint information to screen.");
                Msg.P("\n pinstr\n\tPrint instructions currently in-flight.");
                Msg.P("\n pvars\n\tPrint pipeline registers' values.");
                Msg.P("\n pnextvars\n\tPrint pipeline registers' input values.");
                Msg.P("\n pram <address> ..or.. pram\n\tPrint value of RAM at <address>. Print all if no argument is given.");
                Msg.P("\n preg <address> ..or.. preg\n\tPrint contents of a register or print contents of register file.");
                Msg.P("\n pprg <index>\n\tPrint disassembly of the contents of memory module with bus location specified by <index>");
                Msg.P("\n pasm\n\tPrint program object code.");
                Msg.P("\n pfd\n\tPrint CPU frontend states / IF stage input side values.");

                break;

            case 3:
                Msg.P("\nBus control.");
                Msg.P("\n wbus <address> <value>\n\tWrite <value> to FSB with <address>.");
                Msg.P("\n rbus <address>\n\tIssue read of <addr> to FSB.");
                Msg.P("\n enableio <index> ..or.. enableio\n\tEnable evaluation of I/O device <index>. Enable all if no argument is given.");
                Msg.P("\n disableio <index> ..or.. disableio\n\tDisable evaluation of I/O device <index>. Disable all if no argument is given.");
                Msg.P("\n listio\n\tList I/O modules loaded.");
                Msg.P("\n evalio <index> ..or.. evalio\n\tEvaluate I/O module <index>. Evaluate all if no argument is given.");

                break;

            case 4:
                Msg.P("\nModule management.");
                Msg.P("\n listmods\n\tList avaiable modules in the PLP I/O registry.");
                Msg.P("\n addmod <mod ID> <address> <regfile size>\n\tAttach module with <mod ID> starting at <address> with <regfile size> to the registry and the bus.");
                Msg.P("\n rmmod <index in the REGISTRY>\n\tRemove the module with <index in the REGISTRY> from the registry and the bus.");
                Msg.P("\n attachedmods\n\tList all modules attached to the simulation.");
                Msg.P("\n listpresets\n\tList all available module presets.");
                Msg.P("\n loadpreset <index>\n\tLoad a module preset as specified by <index>");

                break;

            case 5:
                Msg.P("\nMiscellaneous.");
                Msg.P("\n asm <address> <asm>\n\tAssemble <asm> and inject code starting at <address>.");
                Msg.P("\n silent\n\tToggle silent mode (default off).");
                Msg.P("\n cycleaccurate\n\tToggle cycle-accurate simulation mode (default off).");
                Msg.P("\n assert <module index in the BUS> <address/label> <value>\n\tAssert the contents of an address to be the specified value.");
                Msg.P("\n assertreg <register address> <value>\n\tAssert a register's contents to be the specified value.");

                break;

            case 6:
                Msg.P("\nBreakpoints.");
                Msg.P("\n bp set <address/label>\n\tSet a breakpoint at <address> or <label>.");
                Msg.P("\n bp list\n\tList breakpoints and their indices.");
                Msg.P("\n bp remove <index>\n\tRemove breakpoint specified by <index>, as listed in 'bp list' command.");
                Msg.P("\n bp clear\n\tClear all breakpoints.");

                break;
        }
    }
}
