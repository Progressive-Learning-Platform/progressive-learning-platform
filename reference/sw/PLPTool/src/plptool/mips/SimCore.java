/*
    Copyright 2010-2014 David Fritz, Brian Gordon, Wira Mulia

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

import plptool.mods.MemModule;
import plptool.Config;
import plptool.Msg;
import plptool.Constants;
import plptool.PLPSimBus;
import plptool.PLPSimCore;
import plptool.PLPSimRegModule;

/**
 * mips.SimCore is the PLP CPU Architecture Simulator. This class
 * contains core functionality of the simulator: the CPU itself, memory
 * elements, pipeline registers, and I/O devices. Users can interact with the
 * simulation core by using typical procedures such as stepping and I/O. It's
 * up to the command line interface or the GUI to present these features to the
 * user.
 *
 * @author wira
 */
public class SimCore extends PLPSimCore {

    /**
     * The core's program counter.
     */
    public PLPSimRegModule pc;

    /**
     * Register file.
     */
    public MemModule regfile;


    /**
     * ID stage module.
     */
    public id   id_stage;

    /**
     * EX stage module.
     */
    public ex   ex_stage;

    /**
     * MEM stage module.
     */
    public mem  mem_stage;

    /**
     * WB stage module.
     */
    public wb   wb_stage;

    /**
     * Object code from PLPAsm
     *
     * @see plptool.PLPAsm
     */
    long[] objCode;

    /**
     * Address table from PLPAsm
     *
     * @see plptool.PLPAsm
     */
    long[] addrTable;

    /**
     * IF stage Staller
     */
    boolean if_stall;

    /**
     * EX stage Staller
     */
    boolean ex_stall;

    boolean ex_continue;

    /**
     * The assembler object passed to the simulator.
     *
     * @see plptool.PLPAsm
     */
    private Asm asm;

    /**
     * Interrupt request state machine variable
     */
    public int int_state;

    /**
     * Interrupt return address
     */
    private long irq_ret;

    /**
     * Interrupt acknowledge
     */
    public long IRQAck;

    /**
     * Flags from previous step
     */
    private long previous_flags;

    /**
     * Forwarding flags
     */
    public boolean mem_mem = true;
    public boolean mem_ex = true;
    public boolean mem_ex_lw = true;
    public boolean ex_ex = true;

     // Simulator flags
    public static final long PLP_SIM_FWD_NO_EVENTS               = 0xF000FFFF;
    public static final long PLP_SIM_FWD_EX_EX_RT                = 0x00010000;
    public static final long PLP_SIM_FWD_EX_EX_RS                = 0x00020000;
    public static final long PLP_SIM_FWD_MEM_MEM                 = 0x04000000;
    public static final long PLP_SIM_FWD_MEM_EX_RT               = 0x00100000;
    public static final long PLP_SIM_FWD_MEM_EX_RS               = 0x00200000;
    public static final long PLP_SIM_FWD_MEM_EX_LW_RT            = 0x01000000;
    public static final long PLP_SIM_FWD_MEM_EX_LW_RS            = 0x02000000;
    public static final long PLP_SIM_IF_STALL_SET                = 0x00000100;
    public static final long PLP_SIM_ID_STALL_SET                = 0x00000200;
    public static final long PLP_SIM_EX_STALL_SET                = 0x00000400;
    public static final long PLP_SIM_MEM_STALL_SET               = 0x00000800;
    public static final long PLP_SIM_IRQ                         = 0x10000000;
    public static final long PLP_SIM_IRQ_SERVICED                = 0x20000000;

    /**
     * Simulator plp constructor.
     *
     * @param RAMsize the size of main memory attached to this core
     * @param startAddr the starting address of the main memory
     * @param asm assembler object passed on to this simulator
     * @see plptool.PLPAsm
     */
    public SimCore(Asm asm, long startAddr) {
        super();
        this.startAddr = startAddr;

        regfile = new MemModule(0, 32, false);
        pc = new PLPSimRegModule(0); // pc=startAddr on reset

        this.asm = asm;
        this.objCode = asm.getObjectCode();
        this.addrTable = asm.getAddrTable();

        sim_flags = (long) 0;

        // Instantiate stages
        wb_stage = new wb(regfile);
        mem_stage = new mem(wb_stage, bus);
        ex_stage = new ex(mem_stage, new alu());
        id_stage = new id(ex_stage, regfile);

        regfile.enable();
    }

    /**
     * Reset the core: clears RAM and zeroes out register file,
     * reloads the program to memory, resets program counter, flushes the
     * pipeline, clears flags, and resets statistics.
     *
     * @return Returns 0 on successful completion. Should not fail.
     * @see softreset()
     */
    public int reset() {
        int i;

        // issue a reset to all modules
        bus.reset();

        // init regfile
        for(i = 0; i < 32; i++)
            regfile.write(i, new Long(0), false);

        loadProgram(asm);

        IRQ = 0;
        IRQAck = 0;
        pc.reset(startAddr);
        visibleAddr = startAddr;
        instructionCount = 0;
        sim_flags = (long) 0;
        previous_flags = 0;
        ex_continue = false;
        ex_stall = false;
        if_stall = false;
        int_state = 0;
        id_stage.if_count = 0;
        id_stage.id_count = 0;
        ex_stage.count = 0;
        mem_stage.count = 0;
        wb_stage.count = 0;
        branch = false;
        flushpipeline();

        Msg.P("core: reset");

        return Constants.PLP_OK;
    }

    /**
     * Reset the program counter and flushes the pipeline.
     *
     * @return Returns 0 on completion.
     * @see reset()
     */
    public int softreset() {
        pc.reset(startAddr);
        flushpipeline();

        ex_continue = false;
        ex_stall = false;
        if_stall = false;

        Msg.P("core: soft reset");

        return Constants.PLP_OK;
    }

    /**
     * Part of the PLPSimCore specification, this function implements a
     * program loading routine for the simulation core.
     *
     * @param x assembler object containing the program code
     * @return PLP_OK on completion, error code otherwise
     */
    public int loadProgram(plptool.PLPAsm x) {
        int ret = Constants.PLP_OK;

        // load program to bus
        for(int i = 0; i < objCode.length; i++) {
            if (asm.isInstruction(i) == 0)
                ret = bus.write(((Asm)x).getAddrTable()[i],
                            ((Asm)x).getObjectCode()[i], true);
            else
                ret = bus.write(((Asm)x).getAddrTable()[i],
                            ((Asm)x).getObjectCode()[i], false);
        }

        if(ret != Constants.PLP_OK)
            return Msg.E("Unable to write program to memory",
                            Constants.PLP_SIM_UNABLE_TO_LOAD_PROGRAM, this);

        return ret;
    }

    /**
     * Advance the simulation by one cycle.
     *
     * @return Returns 0 on successful completion. Error code otherwise.
     */
    public int step () {
        previous_flags = sim_flags;
        sim_flags = (long) 0;
        instructionCount++;
        wb_stage.instr_retired = false;
        visibleAddr = -1;
        int ret = 0;
        long old_pc = pc.eval();

        if(Config.simFunctional)
            return stepFunctional();

        else {

            /****************** RISING EDGE OF THE CLOCK **********************/

            // Propagate values
            // move i_* values to the output side of the pipeline registers
            if(wb_stage.hot)  wb_stage.clock();
            if(mem_stage.hot) mem_stage.clock();
            if(ex_stage.hot)  ex_stage.clock();
            if(id_stage.hot)  id_stage.clock();

            // clock pc for next instruction
            if(!if_stall)
                pc.clock();

            /****************** FALLING EDGE OF THE CLOCK *********************/

            // Evaluate stages
            // produce i_* values for the input side of the pipeline registers
            // that will be used in the next cycle
            ret += wb_stage.eval();
            ret += mem_stage.eval();
            ret += ex_stage.eval();
            ret += id_stage.eval();

            // Program counter update logic (input side IF)
            if(ex_stage.hot && ex_stage.instrAddr != -1 && ex_stage.ctl_pcsrc == 1) {
                pc.write(ex_stage.ctl_branchtarget);
            } else if (ex_stage.hot && ex_stage.instrAddr != -1 && ex_stage.ctl_jump == 1) {
                pc.write(ex_stage.ctl_jumptarget);
            } else if (!if_stall) {
                pc.write(pc.eval() + 4);
            }

            // Evaluate modules attached to the bus
            ret += bus.eval();
            // Evalulate interrupt controller again to see if anything raised an IRQ
            // (PLPSimBus evaluates modules from index 0 upwards)
            ret += bus.eval(0);

            if(ret != 0) {
                Msg.E("Evaluation failed. This simulation is stale.",
                      Constants.PLP_SIM_EVALUATION_FAILED, this);

                if(Config.simDumpTraceOnFailedEvaluation) this.registersDump();
            }

            /*
             * STALL ROUTINES
             *
             * By default, the CPU here will just get the next instruction as
             * determined by the current value of PC. This is done by updating
             * the input side of the IF/ID pipeline register (id_stage.i_* values)
             * by calling the fetch() function.
             *
             * There are three conditions where this is not true:
             *
             * 1. IF stall due to jumps (id_stage.i_* values will take a bubble and
             *    no new instruction will be fetched, i.e. fetch() function will not
             *    be called in this case).
             *
             * 2. Stall on EX stage due to load-use hazard. Insert bubble for EX
             *    in the next cycle by making sure ex_stage.i_* values will not
             *    change the CPU state (no write to memory and register, no branch
             *    and jump). Then, the IF/ID pipeline is turned off by making
             *    id_stage.hot = false. This will keep the instruction being decoded
             *    to stay in that stage. fetch() will be called to fill the IF stage
             *    or the id_stage.i_* values. Finally we rewrite PC so we don't
             *    skip an instruction since PC is already clocked due to the
             *    procedural nature of our simulation engine. In a real machine,
             *    the PC would have held its value.
             *
             * 3. An interrupt service is requested. The interrupt service is a
             *    3-step process. First, when a request is detected in the end of
             *    a clock cycle, the CPU checks whether a jump or a branch has been
             *    executed in the EX stage. If yes, the CPU will wait another cycle
             *    before servicing (the next instruction is fetched in the IF stage
             *    as usual). If a jump or branch is not in the EX stage, the CPU
             *    will go ahead and flush the IF, ID, and EX stages for the next
             *    cycle.and record the address of the instruction that was in the
             *    EX stage. In the next cycle, a jalr $ir, $iv instruction is
             *    injected in the IF stage (id_stage.i_*) with the return value
             *    being the recorded address minus 4 to offset the plus 4 of the
             *    PC logic. During the third cycle, the CPU injects a bubble for the
             *    jump and resumes normal operation in the ISR space.
             */

            // We're stalled in the NEXT cycle, do not fetch new instruction
            if(if_stall && !ex_continue) {
                if_stall = false;
                id_stage.i_instruction = 0;
                id_stage.i_instrAddr = pc.input();
                id_stage.hot = true;
                id_stage.i_bubble = true;

                return Constants.PLP_OK;

            } else if(ex_stall) { // ex_stall, clear id/ex register
                ex_stall = false;
                ex_continue = true;
                // Insert bubble for EX stage in the next cycle
                ex_stage.i_instruction = 0;
                ex_stage.i_instrAddr = -1;
                ex_stage.i_fwd_ctl_memwrite = 0;
                ex_stage.i_fwd_ctl_regwrite = 0;
                ex_stage.i_ctl_branch = 0;
                ex_stage.i_ctl_jump = 0;
                ex_stage.hot = true;
                ex_stage.i_bubble = true;
                ret = fetch(); // make sure IF stage is filled
                id_stage.hot = false;
                pc.write(old_pc + 4);

                return ret;

            } else if(ex_continue) { // resume from ex_stall, turn on id/ex register
                ex_stage.hot = true;
                ex_continue = false;

                return fetch();

            } else if (int_state == 2) {
                Msg.D("IRQ service, int_inject 2->1", 3, this);
                id_stage.i_instruction = 0x0380f009L; // jalr $ir, $iv
                id_stage.i_instrAddr = 0;
                id_stage.i_ctl_pcplus4 = irq_ret - 4;
                id_stage.hot = true;

                int_state--;
                return Constants.PLP_OK;

            } else if (int_state == 1) {
                Msg.D("IRQ service, int_inject 1->0", 3, this);
                id_stage.i_instruction = 0;
                id_stage.i_instrAddr = -1;
                id_stage.hot = true;

                int_state--;
                IRQAck = 0;
                return Constants.PLP_OK;

            // Interrupt request
            } else if(int_state == 3) {
                Msg.D("IRQ Triggered.", 3, this);
                long diff = pc.input() - ex_stage.i_instrAddr;
                Msg.D("instrAddr diff: " + diff, 3, this);
                sim_flags |= PLP_SIM_IRQ;

                if(diff == 8) {
                    sim_flags |= PLP_SIM_IRQ_SERVICED;
                    irq_ret = mem_stage.i_instrAddr; // address to return to
                    int_state--;
                    IRQAck = 1;
                    Msg.D("IRQ service started, int_inject = 2, irq_ret = " + String.format("0x%02x", irq_ret), 3, this);

                    // flush 3 stages
                    id_stage.i_instruction = 0;
                    ex_stage.i_instruction = 0;
                    ex_stage.i_fwd_ctl_regwrite = 0;
                    ex_stage.i_fwd_ctl_memwrite = 0;
                    ex_stage.i_ctl_branch = 0;
                    ex_stage.i_ctl_jump = 0;
                    mem_stage.i_instruction = 0;
                    mem_stage.i_fwd_ctl_regwrite = 0;
                    mem_stage.i_ctl_memwrite = 0;
                    id_stage.i_instrAddr = -1;
                    ex_stage.i_instrAddr = -1;
                    mem_stage.i_instrAddr = -1;
                    id_stage.hot = true;
                    ex_stage.hot = true;
                    mem_stage.hot = true;

                    return Constants.PLP_OK;

                // can't service yet due to jump/branch
                } else
                    return fetch();

            } else
                return fetch();
        }
    }

    /**
     * Perform an instruction fetch and warm up the decode stage. This function
     * represents the instruction fetch phase of the PLP CPU core. The only
     * memory element in this stage is the program counter (and the instruction
     * memory, but that is external).
     *
     * @return Returns 0 on successful completion. Error code otherwise.
     */
    private int fetch()  {
        long addr = pc.eval();

        // fetch instruction / frontend stage
        if(!bus.isMapped(addr) || addr < 0) {
            if(Config.simDumpTraceOnFailedEvaluation) this.registersDump();
            return Msg.E("fetch(): PC points to unmapped address. Halt."
                         + " pc=" + String.format("0x%08x", addr),
                         Constants.PLP_SIM_INSTRUCTION_FETCH_FAILED, this);
        }

        Long ret = (Long) bus.read(addr);

        if(ret == null) {
            if(Config.simDumpTraceOnFailedEvaluation) this.registersDump();
            return Msg.E("fetch(): Unable to fetch next instruction from the bus."
                         + " pc=" + String.format("0x%08x", addr),
                         Constants.PLP_SIM_INSTRUCTION_FETCH_FAILED, this);
        }

        if(!bus.isInstr(addr) && !Config.simAllowExecutionOfArbitraryMem)
            return Msg.E("fetch(): Attempted to fetch non-executable memory: " +
                            "pc=" + String.format("%08x", addr),
                            Constants.PLP_SIM_NO_EXECUTE_VIOLATION, this);

        id_stage.i_instruction = ret;
        id_stage.i_instrAddr = addr;
        id_stage.i_ctl_pcplus4 = addr + 4;

        id_stage.hot = true;
        id_stage.i_bubble = false;
        visibleAddr = addr;
        id_stage.if_count++;

        Msg.D("fetch(): PC input side: "  + String.format("0x%08x", pc.input())
                  + " - PC output side: " + String.format("0x%08x", pc.eval()),
                     5, this);

        return Constants.PLP_OK;
    }

    private boolean branch;
    private long branch_destination = 0;
    /**
     * Non-cycle accurate stepping. One instruction is fetched and executed
     * every cycle. Branch delay slots are simulated.
     *
     * @return Returns 0 on successful completion. Error code otherwise.
     */
    public int stepFunctional() {
        int ret = 0;

        pc.clock();
        ret += fetch(); // get the instruction
        
        long instr = id_stage.i_instruction;

        // fill a nop for our interrupt's jalr branch delay slot
        if(int_state == 1) {
            instr = 0;
            int_state = 0;
            IRQAck = 0; // we are ready to handle interrupt requests again
        }

        long pcplus4 = pc.eval()+4;

        // control flow handler

        // pcplus4, default execution
        if(!branch && int_state != 3)
            pc.write(pcplus4);
        
        // are we interrupted
        else if(!branch && int_state == 3) {
            Msg.D("INT REQ - ret_addr(pcplus4-4) = " + plptool.PLPToolbox.format32Hex(pcplus4-4), 4, this);
            sim_flags |= PLP_SIM_IRQ;
            // rewrite instruction to jalr $iv, $ir
            instr = 0x0380f009L; 
            id_stage.i_instrAddr = 0;
            pcplus4 -= 8; // replay the discarded instruction after return from IRQ (jalr adds 4)
            int_state = 1;
        }

        // we are branching / jumping - simulate a brand delay slot
        else {
            Msg.D("DELAY SLOT - branching to = " + plptool.PLPToolbox.format32Hex(branch_destination), 4, this);
            pc.write(branch_destination);
            branch = false;
        }

        // decode instruction
        int opcode = MIPSInstr.opcode(instr);
        byte rs = MIPSInstr.rs(instr);
        byte rd = MIPSInstr.rd(instr);
        byte rt = MIPSInstr.rt(instr);
        byte funct = MIPSInstr.funct(instr);
        long imm = MIPSInstr.imm(instr);
        long jaddr = MIPSInstr.jaddr(instr);

        long s = regfile.read(rs);
        long t = regfile.read(rt);
        long s_imm =  (short) imm & 0xffffffffL;
        long alu_result;

        // execute
        if(opcode == 0) {            
            if(funct == 0x08 || funct == 0x09) {        // jr
                branch = true;
                branch_destination = s;

                if(funct == 0x09) {                     // jalr
                    regfile.write(rd, pcplus4+4, false);
                }
            } else {
                alu_result = ex_stage.exAlu.eval(s, t, instr);
                alu_result &= 0xffffffffL;
                regfile.write(rd, alu_result, false);
            }
        } else if (opcode == 0x04) {                    // beq
            if(s == t) {
                branch = true;
                branch_destination = (pcplus4 + (s_imm<<2)) & 0xffffffffL;
            }
        } else if (opcode == 0x05) {                    // bne
            if(s != t) {
                branch = true;
                branch_destination = (pcplus4 + (s_imm<<2)) & 0xffffffffL;
            }
        } else if (opcode == 0x23) {                    // lw
            Long data = (Long) bus.read(s + s_imm);
            if(data == null)
                return Msg.E("Bus read error.", Constants.PLP_SIM_BUS_ERROR, this);
            regfile.write(rt, data, false);

        } else if (opcode == 0x2B) {                    // sw
            ret = bus.write(s + s_imm, regfile.read(rt), false);
            if(ret > 0) {
                return Msg.E("Bus write error.", Constants.PLP_SIM_BUS_ERROR, this);
            }
        } else if (opcode == 0x02 || opcode == 0x03) {  // j
            branch = true;
            branch_destination = jaddr<<2 | (pcplus4 & 0xf0000000L);

            if (opcode == 0x03) {                       // jal
                regfile.write(31, pcplus4+4, false);
            }
        } else if(opcode == 0x0C || opcode == 0x0D) {   // ori, andi
            alu_result = ex_stage.exAlu.eval(s, imm, instr) & 0xffffffffL;
            regfile.write(rt, alu_result, false);

        } else {                                        // other i-type
            alu_result = ex_stage.exAlu.eval(s, s_imm, instr);
            if(alu_result == -1) {
                return Msg.E("Unhandled instruction: invalid op-code",
                        Constants.PLP_SIM_UNHANDLED_INSTRUCTION_TYPE, this);
            }
            alu_result &= 0xffffffffL;
            regfile.write(rt, alu_result, false);
        }

        // Evaluate modules attached to the bus
        ret += bus.eval();
        // Evalulate interrupt controller again to see if anything raised an IRQ
        // (PLPSimBus evaluates modules from index 0 upwards)
        ret += bus.eval(0);

        // we have an IRQ waiting, set ack so the controller won't set another
        // request while we process this one
        if(int_state == 3) {
            IRQAck = 1;
        }

        return ret;
    }

    /**
     * Clear all pipeline stages, in-flight instructions are discarded.
     *
     * @return Returns 0 on completion.
     */
    public int flushpipeline() {
        // Zero out everything
        id_stage.i_instruction = 0;
        id_stage.i_instrAddr = -1;
        id_stage.clock();
        id_stage.eval();
        ex_stage.clock();
        ex_stage.eval();
        mem_stage.clock();
        mem_stage.eval();
        wb_stage.clock();
        wb_stage.eval();

        wb_stage.instr_retired = false;

        return Constants.PLP_OK;
    }

    /**
     * Return whether the CPU is stalled
     *
     * @return boolean that indicates whether the CPU is stalled
     */
    public boolean isStalled() {
        return if_stall;
    }

    /**
     * Return flags from previous cycle
     *
     * @return Flags in long type
     */
    public long getPreviousFlags() {
        return previous_flags;
    }

    /**
     * Print front end states.
     */
    public void printfrontend() {
        Msg.p("if:   " + ((id_stage.i_instrAddr == -1 || id_stage.i_bubble) ? "--------" : String.format("%08x", id_stage.i_instrAddr)) +
            " instr: " + String.format("%08x", id_stage.i_instruction) +
            " : " + MIPSInstr.format(id_stage.i_instruction) +
            "\ni_pc: "  + String.format("%08x", pc.input()));
    }

    @Override public String toString() {
        return "SimCore";
    }

    /**
     * Print the loaded program.
     *
     * @param highlight Memory location to highlight, probably the PC value
     */
    public void printProgram(int memoryPosition, long highlight) {
        if(memoryPosition < 0 || memoryPosition >= bus.getNumOfMods()) {
            Msg.E("Invalid index", Constants.PLP_SIM_WRONG_MODULE_TYPE, this);
            return;
        }


        if(!(bus.getRefMod(memoryPosition) instanceof MemModule)) {
            Msg.E("The specified module is not a memory module.",
                  Constants.PLP_SIM_WRONG_MODULE_TYPE, this);
            return;
        }
        Msg.p("pc\taddress\t\thex\t\tDisassembly");
        Msg.p("--\t-------\t\t---\t\t-----------");
        Object[][] values = ((MemModule) bus.getRefMod(memoryPosition)).getValueSet();
        for(int i = 0; i < values.length; i++) {
                if((Long) values[i][0] == highlight)
                    Msg.pn(">>>");
                if((Boolean) values[i][2])
                    Msg.p(String.format("\t%08x\t%08x\t",
                                       (Long) values[i][0], (Long) values[i][1]) +
                                       MIPSInstr.format((Long) values[i][1]));
        }
        Msg.P();
    }

    /**
     * Print out registers contents, mapped back to the source files
     */
    public void registersDump() {
        Msg.I("Pipeline registers dump:", this);
        java.util.ArrayList<plptool.PLPAsmSource> asms = asm.getAsmList();

        int wb_i = asm.lookupAddrIndex(wb_stage.instrAddr);
        if(wb_i > -1) {
            int wb_index = asm.getFileMapper()[wb_i];
            int wb_line = asm.getLineNumMapper()[wb_i];
            Msg.M(" wb:  " + asms.get(wb_index).getAsmFilePath() +
                       ":" + wb_line + " " + asms.get(wb_index).getAsmLine(wb_line));
        }
        else if(wb_i < 0)
            Msg.M(" wb:  -- bubble --");


        int mem_i = asm.lookupAddrIndex(mem_stage.instrAddr);
        if(mem_i > -1) {
            int mem_index = asm.getFileMapper()[mem_i];
            int mem_line = asm.getLineNumMapper()[mem_i];
            Msg.M(" mem: " + asms.get(mem_index).getAsmFilePath() +
                       ":" + mem_line + " " + asms.get(mem_index).getAsmLine(mem_line));
        }
        else if(mem_i < 0)
            Msg.M(" mem: -- bubble --");

        int ex_i = asm.lookupAddrIndex(ex_stage.instrAddr);
        if(ex_i > -1) {
            int ex_index = asm.getFileMapper()[ex_i];
            int ex_line  = asm.getLineNumMapper()[ex_i];
            Msg.M(" ex:  " + asms.get(ex_index).getAsmFilePath() +
                       ":" + ex_line + " " + asms.get(ex_index).getAsmLine(ex_line));
        }
        else if(ex_i < 0)
            Msg.M(" ex:  -- bubble --");

        int id_i = asm.lookupAddrIndex(id_stage.instrAddr);
        if(id_i > - 1) {
            int id_index = asm.getFileMapper()[id_i];
            int id_line  = asm.getLineNumMapper()[id_i];
            Msg.M(" id:  " + asms.get(id_index).getAsmFilePath() +
                   ":" + id_line + " " + asms.get(id_index).getAsmLine(id_line));
        }
        else if(id_i < 0)
            Msg.M(" id:  -- bubble --");

        int pc_i = asm.lookupAddrIndex(pc.eval());
        if(pc_i > -1) {
            int pc_index = asm.getFileMapper()[pc_i];
            int pc_line  = asm.getLineNumMapper()[pc_i];

            Msg.M(" if:  " + asms.get(pc_index).getAsmFilePath() +
                   ":" + pc_line + " " + asms.get(pc_index).getAsmLine(pc_line));
        }
        else if(pc_i < 0)
            Msg.M(" if:  -- bubble --");
    }

    /**
     * This is the instruction decode (ID) / register file (RF) stage of the
     * PLP CPU core. The execute module and the register file are attached to
     * this class.
     */
    public class id {
        public int if_count = 0;
        public int id_count = 0;
        boolean hot = false;
        public boolean bubble = false;
        public boolean i_bubble = false;
        public long instruction;
        public long instrAddr;

        // ID stage pipeline registers
        public long i_instruction;
        public long i_instrAddr;
        public long i_ctl_pcplus4;

        public long ctl_pcplus4;

        private ex   ex_reg;
        private MemModule regfile;

        public id(ex ex_reg, MemModule regfile) {
            this.ex_reg = ex_reg;
            this.regfile = regfile;
        }

        public void printvars() {
            Msg.p("ID vars");
            Msg.p("\tinstruction:             " + String.format("%08x", instruction) + " " + MIPSInstr.format(instruction));
            Msg.p("\tinstrAddr:               " + ((instrAddr == -1) ? "--------" : String.format("%08x", instrAddr)));
            Msg.p("\tctl_pcplus4:             " + String.format("%08x", ctl_pcplus4));
            Msg.P();
        }

        public void printnextvars() {
            Msg.p("ID next vars");
            Msg.p("\ti_instruction:           " + String.format("%08x", i_instruction) + " " + MIPSInstr.format(i_instruction));
            Msg.p("\ti_instrAddr:             " + ((i_instrAddr == -1) ? "--------" : String.format("%08x", i_instrAddr)));
            Msg.p("\ti_ctl_pcplus4:           " + String.format("%08x", i_ctl_pcplus4));
            Msg.P();
        }

        public String printinstr() {
            return "id:   " + ((instrAddr == -1 || bubble) ? "--------" : String.format("%08x", instrAddr)) +
                     " instr: " + String.format("%08x", instruction) +
                     " : " + MIPSInstr.format(instruction);
        }

        private int eval() {
            try {

            ex_reg.i_bubble = bubble;
            ex_reg.i_instruction = instruction;
            ex_reg.i_instrAddr = instrAddr;

            if(this.hot) {
                this.hot = false;
                ex_reg.hot = true;
            }

            if(!bubble) id_count++;

            byte opcode      = (byte) MIPSInstr.opcode(instruction);
            byte funct       = (byte) MIPSInstr.funct(instruction);

            long addr_rt = MIPSInstr.rt(instruction); // rt
            long addr_rs = MIPSInstr.rs(instruction); // rs

            // Load-use hazard detection logic

            // The register being written to by load word
            long ex_rt =        MIPSInstr.rt(ex_stage.instruction);

            if(mem_stage.hot && mem_ex_lw) {
                if(ex_rt == addr_rt && addr_rt != 0 && ex_stage.fwd_ctl_memread == 1
                        && (MIPSInstr.opcode(instruction) != 0x2B)) { // lw->sw is mem->mem if it's rt
                    ex_stall = true;
                    sim_flags |= PLP_SIM_FWD_MEM_EX_LW_RT;
                }
                if(ex_rt == addr_rs && addr_rs != 0 && ex_stage.fwd_ctl_memread == 1) {
                    ex_stall = true;
                    sim_flags |= PLP_SIM_FWD_MEM_EX_LW_RS;
                }
            }

            // rt
            ex_reg.i_data_rt = (addr_rt == 0) ?
                                   0 : (Long) regfile.read(addr_rt);

            // rs
            ex_reg.i_data_rs = (addr_rs == 0) ?
                                   0 : (Long) regfile.read(addr_rs);

            long imm_field = MIPSInstr.imm(instruction);

            // sign extend on all instructions except for andi and ori
            if(opcode != 0x0C && opcode != 0x0D)
                // hurray for no unsigned types in java!
                ex_reg.i_data_imm_signExtended = (short) imm_field & ((long) 0xfffffff << 4 | 0xf);
            else
                ex_reg.i_data_imm_signExtended = imm_field;

            ex_reg.i_ctl_rd_addr = MIPSInstr.rd(instruction); // rd
            ex_reg.i_ctl_rt_addr = addr_rt;

            ex_reg.i_ctl_aluOp = instruction;

            ex_reg.i_fwd_ctl_linkaddr = ctl_pcplus4 + 4;

            // control logic
            ex_reg.i_fwd_ctl_memtoreg = 0;
            ex_reg.i_fwd_ctl_regwrite = 0;
            ex_reg.i_fwd_ctl_memwrite = 0;
            ex_reg.i_fwd_ctl_memread = 0;
            ex_reg.i_fwd_ctl_jal = 0;
            ex_reg.i_ctl_aluSrc = 0;
            ex_reg.i_ctl_regDst = 0;
            ex_reg.i_ctl_jump = 0;
            ex_reg.i_ctl_branch = 0;

            if(opcode != 0) {
                switch(Asm.lookupInstrType(Asm.lookupInstrOpcode(opcode))) {
                    case 3: // beq and bne
                        ex_reg.i_ctl_branch = 1;

                        break;

                    case 4: // i-types
                    case 5: // lui
                        ex_reg.i_fwd_ctl_regwrite = 1;
                        ex_reg.i_ctl_aluSrc = 1;

                        break;

                    case 6: // lw and sw
                        if(opcode == 0x23) {
                            ex_reg.i_fwd_ctl_memtoreg = 1;
                            ex_reg.i_fwd_ctl_regwrite = 1;
                            ex_reg.i_fwd_ctl_memread = 1;
                            ex_reg.i_ctl_aluSrc = 1;
                        } else if(opcode == 0x2B) {
                            ex_reg.i_fwd_ctl_memwrite = 1;
                            ex_reg.i_ctl_aluSrc = 1;
                        }

                        break;
                    
                    case 7: // j and jal
                        ex_reg.i_ctl_jump = 1;
                        if(Asm.lookupInstrOpcode(opcode).equals("jal")) {
                            ex_reg.i_fwd_ctl_regwrite = 1;
                            ex_reg.i_ctl_regDst = 1;
                            ex_reg.i_ctl_rd_addr = 31;
                            ex_reg.i_fwd_ctl_jal = 1;
                        }
                        
                        break;

                    default:
                        return Msg.E("Unhandled instruction type.",
                                        Constants.PLP_SIM_UNHANDLED_INSTRUCTION_TYPE,
                                        this);
                }
            } else {
                switch(Asm.lookupInstrType(Asm.lookupInstrFunct(funct))) {
                    case 0: // r-types
                    case 1: // shifts
                    case 8: // multiply
                        ex_reg.i_fwd_ctl_regwrite = 1;
                        ex_reg.i_ctl_regDst = 1;
                        break;

                    case 2: // jr
                        ex_reg.i_ctl_jump = 1;
                        break;
                        
                    case 9: // jalr
                        ex_reg.i_ctl_jump = 1;
                        ex_reg.i_fwd_ctl_regwrite = 1;
                        ex_reg.i_ctl_regDst = 1;
                        ex_reg.i_fwd_ctl_jal = 1;

                        break;

                    default:
                        return Msg.E("Unhandled instruction type.",
                                        Constants.PLP_SIM_UNHANDLED_INSTRUCTION_TYPE,
                                        this);
                }
            }

            ex_reg.i_ctl_branchtarget = ctl_pcplus4 +
                               ((short) ex_reg.i_data_imm_signExtended << 2);

            return Constants.PLP_OK;

            } catch(Exception e) {
                return Msg.E("I screwed up: " + e,
                                Constants.PLP_SIM_EVALUATION_FAILED, this);
            }
        }

        private void clock() {
            bubble = i_bubble;
            ctl_pcplus4 = i_ctl_pcplus4;
            instruction = i_instruction;
            instrAddr = i_instrAddr;
        }

        @Override public String toString() {
            return "mips.SimCore.id(addr:" + String.format("%08x", instrAddr)
                    + " instr: " + MIPSInstr.format(instruction) + ")";
        }
    }

    /**
     * This is the execute (EX) stage of the PLP CPU pipeline. The memory and
     * ALU modules are attached to this class.
     */
    public class ex {
        public int count = 0;

        boolean hot = false;
        public boolean bubble = false;
        public boolean i_bubble = false;

        public long instruction;
        public long instrAddr;

        // EX stage pipeline registers
        public long fwd_ctl_memtoreg;
        public long fwd_ctl_regwrite;

        public long fwd_ctl_memwrite;
        public long fwd_ctl_memread;
        public long fwd_ctl_jal;
        public long fwd_ctl_linkaddr;

        public long ctl_aluSrc;
        public long ctl_aluOp;
        public long ctl_regDst;

        public long ctl_jump;
        public long ctl_pcsrc;
        public long ctl_branch;
        public long ctl_branchtarget;
        public long ctl_jumptarget;

        public long ctl_forwardX;
        public long ctl_forwardY;

        public long data_rs;
        public long data_rt;

        public long data_x;
        public long data_eff_y;
        public long data_y;

        public long data_imm_signExtended;
        public long ctl_rt_addr;
        public long ctl_rd_addr;

        public long i_instruction;
        public long i_instrAddr;

        public long i_fwd_ctl_memtoreg;
        public long i_fwd_ctl_regwrite;

        public long i_fwd_ctl_memwrite;
        public long i_fwd_ctl_memread;
        public long i_fwd_ctl_jal;
        public long i_fwd_ctl_linkaddr;

        public long i_ctl_aluSrc;
        public long i_ctl_aluOp;
        public long i_ctl_regDst;

        public long i_ctl_jump;
        public long i_ctl_branch;
        public long i_ctl_branchtarget;

        public long i_data_rs;
        public long i_data_rt;

        public long i_data_imm_signExtended;
        public long i_ctl_rt_addr;
        public long i_ctl_rd_addr;

        long internal_alu_out;

        private mem  mem_reg;
        private alu  exAlu;

        public ex(mem mem_reg, alu exAlu) {
            this.mem_reg = mem_reg;
            this.exAlu = exAlu;
        }

        public void printvars() {
            String rt_forwarded = (sim_flags & (PLP_SIM_FWD_EX_EX_RT |
                                                     PLP_SIM_FWD_MEM_EX_RT))
                                  == 0 ? "" : " (forwarded)";
            String rs_forwarded = (sim_flags & (PLP_SIM_FWD_EX_EX_RS |
                                                     PLP_SIM_FWD_MEM_EX_RS))
                                  == 0 ? "" : " (forwarded)";

            Msg.p("EX vars");
            Msg.p("\tinstruction:             " + String.format("%08x", instruction) + " " + MIPSInstr.format(instruction));
            Msg.p("\tinstrAddr:               " + ((instrAddr == -1 || bubble) ? "--------" : String.format("%08x", instrAddr)));

            // EX stage pipeline registers
            Msg.p("\tfwd_ctl_memtoreg:        " + fwd_ctl_memtoreg);
            Msg.p("\tfwd_ctl_regwrite:        " + fwd_ctl_regwrite);
            Msg.p("\tfwd_ctl_memwrite:        " + fwd_ctl_memwrite);
            Msg.p("\tfwd_ctl_memread:         " + fwd_ctl_memread);
            Msg.p("\tfwd_ctl_linkaddr:        " + String.format("%08x",fwd_ctl_linkaddr));
            Msg.p("\tfwd_ctl_jal:             " + fwd_ctl_jal);

            Msg.p("\tctl_aluSrc:              " + ctl_aluSrc);
            Msg.p("\tctl_aluOp:               " + String.format("%08x",ctl_aluOp));
            Msg.p("\tctl_regDst:              " + ctl_regDst);
            Msg.p("\tctl_addr_rt:             " + ctl_rt_addr);
            Msg.p("\tctl_addr_rd:             " + ctl_rd_addr);

            Msg.p("\tctl_branchtarget:        " + String.format("%08x", ctl_branchtarget));
            Msg.p("\tctl_jump:                " + ctl_jump);
            Msg.p("\tctl_branch:              " + ctl_branch);
            Msg.p("\tctl_jumptarget*:         " + String.format("%08x", ctl_jumptarget));
            Msg.p("\tctl_pcsrc*:              " + ctl_pcsrc);
            Msg.p("\tctl_forwardX*:           " + ctl_forwardX);
            Msg.p("\tctl_forwardY*:           " + ctl_forwardY);

            Msg.p("\tdata_imm_signExtended:   " + String.format("%08x",data_imm_signExtended));
            Msg.p("\tdata_rs:                 " + String.format("%08x",data_rs));
            Msg.p("\tdata_rt:                 " + String.format("%08x",data_rt));
            Msg.p("\tdata_x (ALU0)*:          " + String.format("%08x",data_x) + rs_forwarded);
            Msg.p("\tdata_eff_y*:             " + String.format("%08x",data_eff_y) + rt_forwarded);
            Msg.p("\tdata_y (ALU1)*:          " + String.format("%08x",data_y));

            Msg.p("\tinternal_alu_out*:       " + String.format("%08x",internal_alu_out));
            Msg.P();
        }

        public void printnextvars() {
            Msg.p("EX next vars");
            Msg.p("\ti_instruction:           " + String.format("%08x", i_instruction) + " " + MIPSInstr.format(i_instruction));
            Msg.p("\ti_instrAddr:             " + ((i_instrAddr == -1) ? "--------" : String.format("%08x", i_instrAddr)));

            // EX stage pipeline registers
            Msg.p("\ti_fwd_ctl_memtoreg:      " + i_fwd_ctl_memtoreg);
            Msg.p("\ti_fwd_ctl_regwrite:      " + i_fwd_ctl_regwrite);
            Msg.p("\ti_fwd_ctl_memwrite:      " + i_fwd_ctl_memwrite);
            Msg.p("\ti_fwd_ctl_memread:       " + i_fwd_ctl_memread);
            Msg.p("\ti_fwd_ctl_linkaddr:      " + String.format("%08x",i_fwd_ctl_linkaddr));
            Msg.p("\ti_fwd_ctl_jal:           " + i_fwd_ctl_jal);

            Msg.p("\ti_ctl_aluSrc:            " + i_ctl_aluSrc);
            Msg.p("\ti_ctl_aluOp:             " + String.format("%08x",i_ctl_aluOp));
            Msg.p("\ti_ctl_regDst:            " + i_ctl_regDst);
            Msg.p("\ti_ctl_addr_rt:           " + i_ctl_rt_addr);
            Msg.p("\ti_ctl_addr_rd:           " + i_ctl_rd_addr);

            Msg.p("\ti_ctl_branchtarget:      " + String.format("%08x", i_ctl_branchtarget));
            Msg.p("\ti_ctl_jump:              " + i_ctl_jump);
            Msg.p("\ti_ctl_branch:            " + i_ctl_branch);

            Msg.p("\ti_data_imm_signExtended: " + String.format("%08x",i_data_imm_signExtended));
            Msg.p("\ti_data_rs:               " + String.format("%08x",i_data_rs));
            Msg.p("\ti_data_rt:               " + String.format("%08x",i_data_rt));
            Msg.P();
        }

        public String printinstr() {
            return "ex:   " + ((instrAddr == -1 || bubble) ? "--------" : String.format("%08x", instrAddr)) +
                     " instr: " + String.format("%08x", instruction) +
                     " : " + MIPSInstr.format(instruction);
        }

        private int eval() {
            try {

            mem_reg.i_bubble = bubble;
            mem_reg.i_instruction = instruction;
            mem_reg.i_instrAddr = instrAddr;

            if(this.hot) {
                this.hot = false;
                mem_reg.hot = true;
            }

            if(!bubble) count++;
            
            boolean wb_ctl_regwrite = (wb_stage.ctl_regwrite == 1);
            boolean mem_ctl_regwrite = (mem_stage.fwd_ctl_regwrite == 1);

            long ex_rs = MIPSInstr.rs(instruction);
            long ex_rt = MIPSInstr.rt(instruction);

            // Forward logic for rs source, 1 for EX->EX, 2 for MEM->EX
            ctl_forwardX = (ex_ex && mem_ctl_regwrite && mem_stage.fwd_ctl_dest_reg_addr == ex_rs && ex_rs != 0) ? 1 :
                           (mem_ex && wb_ctl_regwrite && wb_stage.ctl_dest_reg_addr == ex_rs && ex_rs != 0)      ? 2 :
                           0;

            sim_flags |= ((ctl_forwardX == 1) ? PLP_SIM_FWD_EX_EX_RS :
                          (ctl_forwardX == 2) ? PLP_SIM_FWD_MEM_EX_RS : 0);

            // Forward logic for rt source, 1 for EX->EX, 2 for MEM->EX
            ctl_forwardY = (ex_ex && mem_ctl_regwrite && mem_stage.fwd_ctl_dest_reg_addr == ex_rt && ex_rt != 0) ? 1 :
                           (mem_ex && wb_ctl_regwrite && wb_stage.ctl_dest_reg_addr == ex_rt && ex_rt != 0)      ? 2 :
                           0;

            sim_flags |= ((ctl_forwardY == 1) ? PLP_SIM_FWD_EX_EX_RT :
                          (ctl_forwardY == 2) ? PLP_SIM_FWD_MEM_EX_RT : 0);

            data_x = (ctl_forwardX == 0) ? data_rs :
                     (ctl_forwardX == 1) ? mem_stage.fwd_data_alu_result :
                     (ctl_forwardX == 2) ? wb_stage.data_regwrite : 0;

            data_eff_y = (ctl_forwardY == 0) ? data_rt :
                         (ctl_forwardY == 1) ? mem_stage.fwd_data_alu_result :
                         (ctl_forwardY == 2) ? wb_stage.data_regwrite : 0;

            data_y = (ctl_aluSrc == 1) ? data_imm_signExtended : data_eff_y;

            internal_alu_out =
                exAlu.eval(data_x, data_y,
                           ctl_aluOp) & (((long) 0xfffffff << 4) | 0xf);

            mem_reg.i_fwd_data_alu_result = internal_alu_out;

            mem_reg.i_fwd_ctl_memtoreg = fwd_ctl_memtoreg;
            mem_reg.i_fwd_ctl_regwrite = fwd_ctl_regwrite;
            mem_reg.i_fwd_ctl_dest_reg_addr = (ctl_regDst == 1) ? ctl_rd_addr : ctl_rt_addr;

            mem_reg.i_ctl_memwrite = fwd_ctl_memwrite;
            mem_reg.i_ctl_memread = fwd_ctl_memread;
            mem_reg.i_fwd_ctl_linkaddr = fwd_ctl_linkaddr;

            mem_reg.i_fwd_ctl_jal = fwd_ctl_jal;

            mem_reg.i_data_memwritedata = data_eff_y;
           
            ctl_pcsrc = (internal_alu_out == 1) ? 1 : 0;
            ctl_pcsrc &= ctl_branch;

            byte opcode = MIPSInstr.opcode(instruction);
            int jtype = Asm.lookupInstrType(Asm.lookupInstrOpcode(opcode));

            ctl_jumptarget = (jtype == 7) ? (instrAddr & 0xF0000000) |
                                            (MIPSInstr.jaddr(instruction) << 2)
                                            : data_rs;

            // Jump / branch taken, clear next IF stage / create a bubble
            if(ctl_jump == 1 || ctl_pcsrc == 1 && !ex_stall) {
                if_stall = true;
                sim_flags |= PLP_SIM_IF_STALL_SET;
            }

            return Constants.PLP_OK;

            } catch(Exception e) {
                return Msg.E("I screwed up: " + e,
                                Constants.PLP_SIM_EVALUATION_FAILED, this);
            }
        }

        private void clock() {
            bubble = i_bubble;
            instruction = i_instruction;
            instrAddr = i_instrAddr;

            ctl_branch = i_ctl_branch;
            ctl_jump = i_ctl_jump;
            ctl_branchtarget = i_ctl_branchtarget;
            
            fwd_ctl_memtoreg = i_fwd_ctl_memtoreg;
            fwd_ctl_regwrite = i_fwd_ctl_regwrite;

            fwd_ctl_memwrite = i_fwd_ctl_memwrite;
            fwd_ctl_memread = i_fwd_ctl_memread;

            fwd_ctl_jal = i_fwd_ctl_jal;
            fwd_ctl_linkaddr = i_fwd_ctl_linkaddr;

            ctl_aluSrc = i_ctl_aluSrc;
            ctl_aluOp = i_ctl_aluOp;
            ctl_regDst = i_ctl_regDst;

            data_rs = i_data_rs;
            data_rt = i_data_rt;

            data_imm_signExtended = i_data_imm_signExtended;
            ctl_rt_addr = i_ctl_rt_addr;
            ctl_rd_addr = i_ctl_rd_addr;
        }

        @Override public String toString() {
            return "mips.SimCore.ex(addr:" + String.format("%08x", instrAddr)
                    + " instr: " + MIPSInstr.format(instruction) + ")";
        }
    }

    /**
     * This is the memory (MEM) stage of the PLP CPU pipeline. Writeback module
     * is attached to this class. The bus module is also attached to this
     * class, and it is the only place where the PLP CPU simulation core interacts
     * with the outside world aside from fetching instructions in IF stage.
     */
    public class mem {
        public int count = 0;

        boolean hot = false;
        public boolean bubble = false;
        public boolean i_bubble = false;

        public long instruction;
        public long instrAddr;

        // MEM stage pipeline registers
        public long fwd_ctl_memtoreg;
        public long fwd_ctl_regwrite;
        public long fwd_ctl_dest_reg_addr;
        public long fwd_ctl_linkaddr;
        public long fwd_ctl_jal;
        public long fwd_data_alu_result;

        public long ctl_memwrite;
        public long ctl_memread;
        public long ctl_fwd_mem_mem;

        public long data_memwritedata;
        public Long data_mem_load;
        public long data_mem_store;

        public long i_instruction;
        public long i_instrAddr;

        public long i_fwd_ctl_memtoreg;
        public long i_fwd_ctl_regwrite;
        public long i_fwd_ctl_dest_reg_addr;
        public long i_fwd_ctl_linkaddr;
        public long i_fwd_ctl_jal;
        public long i_fwd_data_alu_result;

        public long i_ctl_memwrite;
        public long i_ctl_memread;

        public long i_data_memwritedata;

        private wb   wb_reg;
        private PLPSimBus bus;

        public mem(wb wb_reg, PLPSimBus bus) {
            this.wb_reg = wb_reg;
            this.bus = bus;
        }

        public void printvars() {
            String writedata_forwarded = (sim_flags & (PLP_SIM_FWD_MEM_MEM))
                      == 0 ? "" : " (forwarded)";
            
            Msg.p("MEM vars");
            Msg.p("\tinstruction:             " + String.format("%08x", instruction) + " " + MIPSInstr.format(instruction));
            Msg.p("\tinstrAddr: :             " + ((instrAddr == -1 || bubble) ? "--------" : String.format("%08x", instrAddr)));

            // MEM stage pipeline registers
            Msg.p("\tfwd_ctl_memtoreg:        " + fwd_ctl_memtoreg);
            Msg.p("\tfwd_ctl_regwrite:        " + fwd_ctl_regwrite);
            Msg.p("\tfwd_ctl_dest_reg_addr:   " + fwd_ctl_dest_reg_addr);
            Msg.p("\tfwd_ctl_linkaddr:        " + String.format("%08x",fwd_ctl_linkaddr));
            Msg.p("\tfwd_ctl_jal:             " + fwd_ctl_jal);
            Msg.p("\tfwd_data_alu_result:     " + String.format("%08x",fwd_data_alu_result));

            Msg.p("\tctl_memwrite:            " + ctl_memwrite);
            Msg.p("\tctl_memread:             " + ctl_memread);
            Msg.p("\tctl_fwd_mem_mem*:        " + ctl_fwd_mem_mem);

            Msg.p("\tdata_memwritedata:       " + String.format("%08x",data_memwritedata));
            Msg.p("\tdata_mem_load*:          " + String.format("%08x",data_mem_load));
            Msg.p("\tdata_mem_store*:         " + String.format("%08x",data_mem_store) + writedata_forwarded);
            Msg.P();
        }

        public void printnextvars() {
            Msg.p("MEM next vars");
            Msg.p("\ti_instruction:           " + String.format("%08x", i_instruction) + " " + MIPSInstr.format(i_instruction));
            Msg.p("\ti_instrAddr:             " + ((i_instrAddr == -1) ? "--------" : String.format("%08x", i_instrAddr)));

            // MEM stage pipeline registers
            Msg.p("\ti_fwd_ctl_memtoreg:      " + i_fwd_ctl_memtoreg);
            Msg.p("\ti_fwd_ctl_regwrite:      " + i_fwd_ctl_regwrite);
            Msg.p("\ti_fwd_ctl_dest_reg_addr: " + i_fwd_ctl_dest_reg_addr);
            Msg.p("\ti_fwd_ctl_linkaddr:      " + String.format("%08x",i_fwd_ctl_linkaddr));
            Msg.p("\ti_fwd_ctl_jal:           " + i_fwd_ctl_jal);
            Msg.p("\ti_fwd_data_alu_result:   " + String.format("%08x",i_fwd_data_alu_result));

            Msg.p("\ti_ctl_memwrite:          " + i_ctl_memwrite);
            Msg.p("\ti_ctl_memread:           " + i_ctl_memread);

            Msg.p("\ti_data_memwritedata:     " + String.format("%08x",i_data_memwritedata));
            Msg.P();
        }

        public String printinstr() {
            return "mem:  " + ((instrAddr == -1 || bubble) ? "--------" : String.format("%08x", instrAddr)) +
                     " instr: " + String.format("%08x", instruction) +
                     " : " + MIPSInstr.format(instruction);
        }

        private int eval() {
            try {

            wb_reg.i_bubble = bubble;
            wb_reg.i_instruction = instruction;
            wb_reg.i_instrAddr = instrAddr;

            if(this.hot) {
                this.hot = false;
                wb_reg.hot = true;
            }

            if(!bubble) count++;

            // Check for MEM->MEM data dependency
            ctl_fwd_mem_mem = (wb_stage.ctl_memtoreg == 1 && ctl_memwrite == 1 &&
                    wb_stage.ctl_dest_reg_addr == MIPSInstr.rt(instruction) &&
                    MIPSInstr.rt(instruction) != 0 && mem_mem) ? 1 : 0;
            sim_flags |= (ctl_fwd_mem_mem == 1 ? PLP_SIM_FWD_MEM_MEM : 0);
            data_mem_store = (ctl_fwd_mem_mem == 1) ? wb_stage.data_regwrite : data_memwritedata;

            wb_reg.i_instruction = instruction;
            wb_reg.i_instrAddr = instrAddr;

            wb_reg.i_ctl_memtoreg = fwd_ctl_memtoreg;
            wb_reg.i_ctl_regwrite = fwd_ctl_regwrite;
            wb_reg.i_ctl_dest_reg_addr = fwd_ctl_dest_reg_addr;
            wb_reg.i_ctl_jal = fwd_ctl_jal;
            wb_reg.i_ctl_linkaddr = fwd_ctl_linkaddr;
        
            wb_reg.i_data_alu_result = fwd_data_alu_result;

            data_mem_load = (ctl_memread == 1) ? (Long) bus.read(fwd_data_alu_result) : 0;
            if(data_mem_load == null)
                return Msg.E("The bus returned no data, check previous error.",
                                Constants.PLP_SIM_BUS_ERROR, this);
            wb_reg.i_data_memreaddata = data_mem_load;

            if(ctl_memwrite == 1)
                if(bus.write(fwd_data_alu_result, data_mem_store, false) != Constants.PLP_OK)
                    return Msg.E("Write failed, check previous error.",
                                    Constants.PLP_SIM_BUS_ERROR, this);

            return Constants.PLP_OK;

            } catch(Exception e) {
                return Msg.E("I screwed up: " + e,
                                Constants.PLP_SIM_EVALUATION_FAILED, this);
            }
        }

        private void clock() {
            bubble = i_bubble;
            instruction = i_instruction;
            instrAddr = i_instrAddr;
            
            fwd_ctl_memtoreg = i_fwd_ctl_memtoreg;
            fwd_ctl_regwrite = i_fwd_ctl_regwrite;
            fwd_ctl_dest_reg_addr = i_fwd_ctl_dest_reg_addr;
            fwd_data_alu_result = i_fwd_data_alu_result;
            fwd_ctl_linkaddr = i_fwd_ctl_linkaddr;
            fwd_ctl_jal = i_fwd_ctl_jal;

            ctl_memwrite = i_ctl_memwrite;
            ctl_memread = i_ctl_memread;
            data_memwritedata = i_data_memwritedata;
        }

        @Override public String toString() {
            return "mips.SimCore.mem(addr:" + String.format("%08x", instrAddr)
                    + " instr: " + MIPSInstr.format(instruction) + ")";
        }
    }

    /**
     * This is the writeback (WB) and final stage of the PLP CPU pipeline.
     * Instructions are retired and register file is written to in this
     * stage.
     */
    public class wb {
        public int count = 0;

        boolean hot = false;
        public boolean instr_retired = false;
        public boolean bubble = false;
        public boolean i_bubble = false;

        public long instruction;
        public long instrAddr;

        // WB stage pipeline registers
        public long ctl_memtoreg;
        public long ctl_regwrite;
        public long ctl_dest_reg_addr;
        public long ctl_linkaddr;
        public long ctl_jal;

        public long data_memreaddata;
        public long data_alu_result;
        public long data_regwrite;

        public long i_instruction;
        public long i_instrAddr;
        
        public long i_ctl_memtoreg;
        public long i_ctl_regwrite;
        public long i_ctl_dest_reg_addr;
        public long i_ctl_linkaddr;
        public long i_ctl_jal;

        public long i_data_memreaddata;
        public long i_data_alu_result;

        private MemModule regfile;

        public wb(MemModule regfile) {
            this.regfile = regfile;
        }

        public void printvars() {
            Msg.p("WB vars");
            Msg.p("\tinstruction:             " + String.format("%08x", instruction) + " " + MIPSInstr.format(instruction));
            Msg.p("\tinstrAddr:               " + ((instrAddr == -1 || bubble) ? "--------" : String.format("%08x", instrAddr)));

            // WB stage pipeline registers
            Msg.p("\tctl_memtoreg:            " + ctl_memtoreg);
            Msg.p("\tctl_regwrite:            " + ctl_regwrite);
            Msg.p("\tctl_dest_reg_addr:       " + ctl_dest_reg_addr);
            Msg.p("\tctl_linkaddr:            " + String.format("%08x",ctl_linkaddr));
            Msg.p("\tctl_jal:                 " + ctl_jal);

            Msg.p("\tdata_memreaddata:        " + String.format("%08x",data_memreaddata));
            Msg.p("\tdata_alu_result:         " + String.format("%08x",data_alu_result));
            Msg.p("\tdata_regwrite*:          " + String.format("%08x",data_regwrite));
            Msg.P();
        }

        public void printnextvars() {
            Msg.p("WB next vars");
            Msg.p("\ti_instruction:           " + String.format("%08x", i_instruction) + " " + MIPSInstr.format(i_instruction));
            Msg.p("\ti_instrAddr:             " + ((i_instrAddr == -1) ? "--------" : String.format("%08x", i_instrAddr)));

            // WB stage pipeline registers
            Msg.p("\ti_ctl_memtoreg:          " + i_ctl_memtoreg);
            Msg.p("\ti_ctl_regwrite:          " + i_ctl_regwrite);
            Msg.p("\ti_ctl_dest_reg_addr:     " + i_ctl_dest_reg_addr);
            Msg.p("\ti_ctl_linkaddr:          " + String.format("%08x",i_ctl_linkaddr));
            Msg.p("\ti_ctl_jal:               " + i_ctl_jal);

            Msg.p("\ti_data_memreaddata:      " + String.format("%08x",i_data_memreaddata));
            Msg.p("\ti_data_alu_result:       " + String.format("%08x",i_data_alu_result));
            Msg.P();
        }

        public String printinstr() {
            return "wb:   " + ((instrAddr == -1 || bubble) ? "--------" : String.format("%08x", instrAddr)) +
                     " instr: " + String.format("%08x", instruction) +
                     " : " + MIPSInstr.format(instruction);
        }
    
        private int eval() {
            try {

            if(this.hot) {
                this.hot = false;
                instr_retired = true;
            }

            if(!bubble) count++;

            long internal_2x1 = (ctl_jal == 0) ?
                                 data_alu_result : ctl_linkaddr;

            data_regwrite = (ctl_memtoreg == 0) ? internal_2x1 : data_memreaddata;

            if(ctl_regwrite == 1 && ctl_dest_reg_addr != 0)
                regfile.write(ctl_dest_reg_addr, data_regwrite, false);

            return Constants.PLP_OK;

            } catch(Exception e) {
                return Msg.E("I screwed up: " + e,
                                Constants.PLP_SIM_EVALUATION_FAILED, this);
            }
        }

        private void clock() {
            bubble = i_bubble;
            instruction = i_instruction;
            instrAddr = i_instrAddr;
            
            ctl_memtoreg = i_ctl_memtoreg;
            ctl_regwrite = i_ctl_regwrite;
            ctl_dest_reg_addr = i_ctl_dest_reg_addr;
            ctl_linkaddr = i_ctl_linkaddr;
            ctl_jal = i_ctl_jal;

            data_memreaddata = i_data_memreaddata;
            data_alu_result = i_data_alu_result;
        }

        @Override public String toString() {
            return "mips.SimCore.wb(addr:" + String.format("%08x", instrAddr)
                    + " instr: " + MIPSInstr.format(instruction) + ")";
        }
    }

    /**
     * The ALU.
     */
    public class alu {

        public alu() {
            
        }

        private long eval(long a, long b, long instr) {

            switch(MIPSInstr.opcode(instr)) {

                // R-types
                case 0:
                    switch(MIPSInstr.funct(instr)) {
                        case 0x24: return a & b;
                        case 0x25: return a | b;
                        case 0x27: return ~(a | b);
                        case 0x21: return a + b;
                        case 0x23: return a - b;
                        case 0x2A:
                            int a_signed = (int) a;
                            int b_signed = (int) b;
                            return (a_signed < b_signed) ? 1 : 0;
                        case 0x2B:
                            return (a < b) ? 1 : 0;
                        case 0x00: return b << MIPSInstr.sa(instr);
                        case 0x02: return b >>> MIPSInstr.sa(instr);
			case 0x10: return ((long)(int) a * (long)(int)b) & 0xffffffffL;
			case 0x11: return (((long)(int) a * (long)(int) b) & 0xffffffff00000000L) >> 32;
                        case 0x01: return a << b;
                        case 0x03: return a >> b;
                    }

                case 0x04: return (a - b == 0) ? 1 : 0;
                case 0x05: return (a - b == 0) ? 0 : 1;
                case 0x0c: return a & b;
                case 0x0d: return a | b;
                case 0x0f: return b << 16;
                case 0x0A:
                    int a_signed = (int) a;
                    int b_signed = (int) b;
                    return (a_signed < b_signed) ? 1 : 0;
                case 0x0B:
                    return (a < b) ? 1 : 0;
                case 0x08:
                case 0x09:
                case 0x23:
                case 0x2B: return a + b;
            }

            return -1;
        }
    }
}


