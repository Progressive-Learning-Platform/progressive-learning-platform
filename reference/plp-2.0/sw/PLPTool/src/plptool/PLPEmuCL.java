package plptool;

import java.io.InputStreamReader;
import java.io.BufferedReader;

/**
 * PLPTool command line emulator interface
 *
 * @author wira
 */
public class PLPEmuCL {

    public static void emuCL(String asmFile) {
        BufferedReader stdIn = new BufferedReader(new InputStreamReader((System.in)));
        PLPAsm asm;
        PLPMIPSEmu core = null;
        String input = "";
        String tokens[];
        int ram_size;
        boolean init_core = false;

        try {

        System.out.println("PLPTool Command Line Emulator");
        System.out.println("Assembling " + asmFile);
        asm = new PLPAsm(null, asmFile, 0);
        if(asm.preprocess(0) == PLPMsg.PLP_OK)
            asm.assemble();

        if(!asm.isAssembled()) {
            PLPMsg.E("Assembly failed.", PLPMsg.PLP_ERROR_RETURN, asm);
            System.exit(PLPMsg.PLP_ERROR_RETURN);
        }


        System.out.print("emu > ");
        while(!(input = stdIn.readLine()).equals("q")) {
            tokens = input.split(" ");
            if(input.equals("version")) {
                System.out.println(PLPMsg.versionString);
            }
            else if(tokens[0].equals("i")) {
                if(tokens.length != 2) {
                    System.out.println("Usage: initcore <ram size in bytes>");
                }
                else {
                    ram_size = Integer.parseInt(tokens[1]);
                    if(ram_size % 4 != 0)
                        System.out.println("RAM size has to be in multiples of 4");
                    else {
                        ram_size /= 4;
                        core = new PLPMIPSEmu(asm, ram_size);
                        init_core = true;
                        core.coreMem.printMainMem();
                        core.printfrontend();
                    }
                }
            }
            else if(input.equals("s")) {
                if(!init_core)
                    System.out.println("Core is not initialized.");
                else {
                    core.step();
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
                    core.coreMem.printMainMem();
                }
            }
            else if(input.equals("preg")) {
                if(!init_core)
                    System.out.println("Core is not initialized.");
                else {
                    core.coreMem.printRegFile();
                }
            }
            else if(input.equals("pfd")) {
                if(!init_core)
                    System.out.println("Core is not initialized.");
                else {
                    core.printfrontend();
                }
            }
            else if(input.equals("pprg")) {
                if(!init_core)
                    System.out.println("Core is not initialized.");
                else {
                    core.printprogram();
                }
            }
            else if(tokens[0].equals("wpc")) {
                if(tokens.length != 2) {
                    System.out.println("Usage: wpc <address>");
                }
                else {
                    core.coreMem.pc = PLPAsm.sanitize32bits(tokens[1]);
                    if(core.fetch() != 0)
                        System.out.println("Simulation is stale. Please reset.");
                    core.printfrontend();
                }
            }
            else if(input.equals("pvars")) {
                if(!init_core)
                    System.out.println("Core is not initialized.");
                else {
                    core.wb_stage.printvars();
                    core.mem_stage.printvars();
                    core.ex_stage.printvars();
                    core.rf_stage.printvars();
                }
            }

            else {
                System.out.println("Unknown command");
            }

            System.out.print("emu > ");
        }

        } catch(Exception e) {
            System.err.println(e);
        }
    }
}
