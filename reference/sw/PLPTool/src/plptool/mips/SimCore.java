/*
    Copyright 2010-2011 David Fritz, Brian Gordon, Wira Mulia

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

import java.util.ArrayList;

/**
 * mips.SimCore is the PLP MIPS Architecture Simulator. This class
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
     * Forwarding unit.
     */
    public mod_forwarding forwarding;

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
    Asm asm;

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
      * Functional mode return address for interrupts
      */
     private long functional_ret;

    /**
     * Simulator plp constructor.
     *
     * @param RAMsize the size of main memory attached to this core
     * @param startAddr the starting address of the main memory
     * @param asm assembler object passed on to this simulator
     * @see plptool.PLPAsm
     */
    public SimCore(Asm asm, long startAddr) {
        this.startAddr = startAddr;

        regfile = new MemModule(0, 32, false);
        pc = new PLPSimRegModule(0); // pc=startAddr on reset

        this.asm = asm;
        this.objCode = asm.getObjectCode();
        this.addrTable = asm.getAddrTable();

        sim_flags = (long) 0;

        // core mods
        forwarding = new mod_forwarding();
        breakpoints = new mod_breakpoint();
        bus = new PLPSimBus();

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
        ex_continue = false;
        ex_stall = false;
        if_stall = false;
        int_state = 0;
        flushpipeline();

        Msg.M("core: reset");

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

        Msg.M("core: soft reset");

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
        sim_flags = (long) 0;
        instructionCount++;
        wb_stage.instr_retired = false;
        visibleAddr = -1;
        int ret = 0;
        long old_pc = pc.eval();


        if(Config.simFunctional)
            ret += stepFunctional();

        else {

            /****************** RISING EDGE OF THE CLOCK **********************/

            // Propagate values
            if(wb_stage.hot)  wb_stage.clock();
            if(mem_stage.hot) mem_stage.clock();
            if(ex_stage.hot)  ex_stage.clock();
            if(id_stage.hot)  id_stage.clock();

            // clock pc for next instruction
            if(!if_stall)
                pc.clock();

            /****************** FALLING EDGE OF THE CLOCK *********************/

            // Evaluate stages
            ret += wb_stage.eval();
            ret += mem_stage.eval();
            ret += ex_stage.eval();
            ret += id_stage.eval();

            // Engage forwarding unit
            if(Config.simForwardingUnit)
                forwarding.eval(id_stage, ex_stage, mem_stage, wb_stage);
        }

        // pc update logic (input side IF)
        if(ex_stage.hot && ex_stage.instrAddr != -1 && ex_stage.ctl_pcsrc == 1) {
            pc.write(ex_stage.ctl_branchtarget);
            functional_ret = pc.input();
        } else if (ex_stage.hot && ex_stage.instrAddr != -1 && ex_stage.ctl_jump == 1) {
            pc.write(ex_stage.ctl_jumptarget);
            functional_ret = pc.input();
        } else if (!if_stall) {
            pc.write(pc.eval() + 4);
            functional_ret = pc.eval();
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

        // We're stalled in this cycle, do not fetch new instruction
        if(if_stall && !ex_continue) {
            if_stall = false;
            id_stage.i_instruction = 0;
            id_stage.i_instrAddr = pc.input();
            id_stage.hot = true;

            return Constants.PLP_OK;

        } else if(ex_stall) { // ex_stall, clear id/ex register
            ex_stall = false;
            ex_continue = true;
            ex_stage.i_instruction = 0;
            ex_stage.i_instrAddr = -1;
            ex_stage.i_fwd_ctl_memwrite = 0;
            ex_stage.i_fwd_ctl_regwrite = 0;
            ex_stage.i_ctl_branch = 0;
            ex_stage.i_ctl_jump = 0;
            ex_stage.hot = true;
            ret = fetch();
            id_stage.hot = false;
            pc.write(old_pc + 4);

            return ret;

        } else if(ex_continue) { // resume from ex_stall, turn on id/ex register
            ex_stage.hot = true;
            ex_continue = false;

            return fetch();

        } else if (int_state == 2) {
            Msg.D("IRQ service, int_inject 2->1", 3, this);
            Asm x = new Asm("jalr $i1, $i0", "inline");
            x.preprocess(0);
            x.assemble();
            id_stage.i_instruction = x.getObjectCode()[0];
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
            sim_flags |= Constants.PLP_SIM_IRQ;
           
            if(diff == 8 || Config.simFunctional) {
                sim_flags |= Constants.PLP_SIM_IRQ_SERVICED;
                irq_ret = (Config.simFunctional ? functional_ret : mem_stage.i_instrAddr); // address to return to
                int_state--;
                IRQAck = 1;
                Msg.D("IRQ service started, int_inject = 2, irq_ret = " + String.format("0x%02x", irq_ret), 3, this);

                // flush 3 stages
                id_stage.i_instruction = 0;
                ex_stage.i_instruction = 0;
                mem_stage.i_instruction = 0;
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

    /**
     * Perform an instruction fetch and warm up the decode stage. This function
     * represents the instruction fetch phase of the MIPS core. The only
     * memory element in this stage is the program counter (and the instruction
     * memory, but that is external).
     *
     * @return Returns 0 on successful completion. Error code otherwise.
     */
    private int fetch()  {
        long addr = pc.eval();

        // fetch instruction / frontend stage
        if(!bus.isMapped(addr)) {
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
        visibleAddr = addr;

        Msg.D("fetch(): PC input side: "  + String.format("0x%08x", pc.input())
                  + " - PC output side: " + String.format("0x%08x", pc.eval()),
                     5, this);

        return Constants.PLP_OK;
    }

    /**
     * Non-cycle accurate stepping. Lets the instruction through the pipeline
     * before fetching the next one (functional mode)
     *
     * @return Returns 0 on successful completion. Error code otherwise.
     */
    public int stepFunctional() {
        int ret = 0;

        pc.clock();
        id_stage.clock();
        ret += id_stage.eval();
        ex_stage.clock();
        ret += ex_stage.eval();
        mem_stage.clock();
        ret += mem_stage.eval();
        wb_stage.clock();
        ret += wb_stage.eval();
        ex_stage.hot = true;
        if_stall = false;
        ex_stall = false;

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
     * Print front end states.
     */
    public void printfrontend() {
        Msg.M("if:   " + ((id_stage.i_instrAddr == -1) ? "--------" : String.format("%08x", id_stage.i_instrAddr)) +
            " instr: " + String.format("%08x", id_stage.i_instruction) +
            " : " + MIPSInstr.format(id_stage.i_instruction) +
            "\ni_pc: "  + String.format("%08x", pc.input()));
    }

    @Override public String toString() {
        return "mips.SimCore(asm: " + asm.toString() + ")";
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
        Msg.M("pc\taddress\t\thex\t\tDisassembly");
        Msg.M("--\t-------\t\t---\t\t-----------");
        Object[][] values = ((MemModule) bus.getRefMod(memoryPosition)).getValueSet();
        for(int i = 0; i < values.length; i++) {
                if((Long) values[i][0] == highlight)
                    Msg.m(">>>");
                if((Boolean) values[i][2])
                    Msg.M(String.format("\t%08x\t%08x\t",
                                       (Long) values[i][0], (Long) values[i][1]) +
                                       MIPSInstr.format((Long) values[i][1]));
        }
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
            Msg.I(" wb:  " + asms.get(wb_index).getAsmFilePath() +
                       ":" + wb_line + " " + asms.get(wb_index).getAsmLine(wb_line), null);
        }
        else if(wb_i < 0)
            Msg.I(" wb:  -- bubble --", null);


        int mem_i = asm.lookupAddrIndex(mem_stage.instrAddr);
        if(mem_i > -1) {
            int mem_index = asm.getFileMapper()[mem_i];
            int mem_line = asm.getLineNumMapper()[mem_i];
            Msg.I(" mem: " + asms.get(mem_index).getAsmFilePath() +
                       ":" + mem_line + " " + asms.get(mem_index).getAsmLine(mem_line), null);
        }
        else if(mem_i < 0)
            Msg.I(" mem: -- bubble --", null);

        int ex_i = asm.lookupAddrIndex(ex_stage.instrAddr);
        if(ex_i > -1) {
            int ex_index = asm.getFileMapper()[ex_i];
            int ex_line  = asm.getLineNumMapper()[ex_i];
            Msg.I(" ex:  " + asms.get(ex_index).getAsmFilePath() +
                       ":" + ex_line + " " + asms.get(ex_index).getAsmLine(ex_line), null);
        }
        else if(ex_i < 0)
            Msg.I(" ex:  -- bubble --", null);

        int id_i = asm.lookupAddrIndex(id_stage.instrAddr);
        if(id_i > - 1) {
            int id_index = asm.getFileMapper()[id_i];
            int id_line  = asm.getLineNumMapper()[id_i];
            Msg.I(" id:  " + asms.get(id_index).getAsmFilePath() +
                   ":" + id_line + " " + asms.get(id_index).getAsmLine(id_line), null);
        }
        else if(id_i < 0)
            Msg.I(" id:  -- bubble --", null);

        int pc_i = asm.lookupAddrIndex(pc.eval());
        if(pc_i > -1) {
            int pc_index = asm.getFileMapper()[pc_i];
            int pc_line  = asm.getLineNumMapper()[pc_i];

            Msg.I(" if:  " + asms.get(pc_index).getAsmFilePath() +
                   ":" + pc_line + " " + asms.get(pc_index).getAsmLine(pc_line), null);
        }
        else if(pc_i < 0)
            Msg.I(" if:  -- bubble --", null);
    }

    /**
     * This is the instruction decode (ID) / register file (RF) stage of the
     * MIPS core. The execute module and the register file are attached to
     * this class.
     */
    public class id {
        boolean hot = false;
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
            Msg.M("ID vars");
            Msg.M("\tinstruction: " + String.format("%08x", instruction) + " " + MIPSInstr.format(instruction));
            Msg.M("\tinstrAddr: " + ((instrAddr == -1) ? "--------" : String.format("%08x", instrAddr)));
            Msg.M("\tctl_pcplus4: " + String.format("%08x", ctl_pcplus4));
        }

        public void printnextvars() {
            Msg.M("ID next vars");
            Msg.M("\ti_instruction: " + String.format("%08x", i_instruction) + " " + MIPSInstr.format(i_instruction));
            Msg.M("\ti_instrAddr: " + ((i_instrAddr == -1) ? "--------" : String.format("%08x", i_instrAddr)));
            Msg.M("\ti_ctl_pcplus4: " + String.format("%08x", i_ctl_pcplus4));
        }

        public void printinstr() {
            Msg.M("id:   " + ((instrAddr == -1) ? "--------" : String.format("%08x", instrAddr)) +
                     " instr: " + String.format("%08x", instruction) +
                     " : " + MIPSInstr.format(instruction));
        }

        private int eval() {
            try {

            ex_reg.i_instruction = instruction;
            ex_reg.i_instrAddr = instrAddr;

            if(this.hot) {
                this.hot = false;
                ex_reg.hot = true;
            }

            if(instrAddr == -1) return Constants.PLP_OK; // cleared

            byte opcode      = (byte) MIPSInstr.opcode(instruction);
            byte funct       = (byte) MIPSInstr.funct(instruction);

            long addr_rt = MIPSInstr.rt(instruction); // rt
            long addr_rs = MIPSInstr.rs(instruction); // rs

            // rt
            ex_reg.i_data_rt     = (addr_rt == 0) ?
                                   0 : (Long) regfile.read(addr_rt);

            // rs
            ex_reg.i_data_alu_in = (addr_rs == 0) ?
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
     * This is the execute (EX) stage of the MIPS pipeline. The memory and
     * ALU modules are attached to this class.
     */
    public class ex {
        boolean hot = false;
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

        public long data_alu_in;
        public long data_rt;

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

        public long i_data_alu_in;
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
            Msg.M("EX vars");
            Msg.M("\tinstruction: " + String.format("%08x", instruction) + " " + MIPSInstr.format(instruction));
            Msg.M("\tinstrAddr: " + ((instrAddr == -1) ? "--------" : String.format("%08x", instrAddr)));

            // EX stage pipeline registers
            Msg.M("\tfwd_ctl_memtoreg: " + fwd_ctl_memtoreg);
            Msg.M("\tfwd_ctl_regwrite: " + fwd_ctl_regwrite);
            Msg.M("\tfwd_ctl_memwrite: " + fwd_ctl_memwrite);
            Msg.M("\tfwd_ctl_memread: " + fwd_ctl_memread);
            Msg.M("\tfwd_ctl_linkaddr: " + String.format("%08x",fwd_ctl_linkaddr));
            Msg.M("\tfwd_ctl_jal: " + fwd_ctl_jal);

            Msg.M("\tctl_aluSrc: " + ctl_aluSrc);
            Msg.M("\tctl_aluOp: " + String.format("%08x",ctl_aluOp));
            Msg.M("\tctl_regDst: " + ctl_regDst);

            Msg.M("\tctl_branchtarget: " + String.format("%08x", ctl_branchtarget));
            Msg.M("\tctl_jumptarget: " + String.format("%08x", ctl_jumptarget));
            Msg.M("\tctl_pcsrc: " + ctl_pcsrc);
            Msg.M("\tctl_jump: " + ctl_jump);
            Msg.M("\tctl_branch: " + ctl_branch);

            Msg.M("\tdata_imm_signExtended: " + String.format("%08x",data_imm_signExtended));
            Msg.M("\tdata_alu_in: " + String.format("%08x",data_alu_in));
            Msg.M("\tdata_rt: " + String.format("%08x",data_rt));

            Msg.M("\tinternal_alu_out: " + String.format("%08x",internal_alu_out));
        }

        public void printnextvars() {
            Msg.M("EX next vars");
            Msg.M("\ti_instruction: " + String.format("%08x", i_instruction) + " " + MIPSInstr.format(i_instruction));
            Msg.M("\ti_instrAddr: " + ((i_instrAddr == -1) ? "--------" : String.format("%08x", i_instrAddr)));

            // EX stage pipeline registers
            Msg.M("\ti_fwd_ctl_memtoreg: " + i_fwd_ctl_memtoreg);
            Msg.M("\ti_fwd_ctl_regwrite: " + i_fwd_ctl_regwrite);
            Msg.M("\ti_fwd_ctl_memwrite: " + i_fwd_ctl_memwrite);
            Msg.M("\ti_fwd_ctl_memread: " + i_fwd_ctl_memread);
            Msg.M("\ti_fwd_ctl_linkaddr: " + String.format("%08x",i_fwd_ctl_linkaddr));
            Msg.M("\ti_fwd_ctl_jal: " + i_fwd_ctl_jal);

            Msg.M("\ti_ctl_aluSrc: " + i_ctl_aluSrc);
            Msg.M("\ti_ctl_aluOp: " + String.format("%08x",i_ctl_aluOp));
            Msg.M("\ti_ctl_regDst: " + i_ctl_regDst);

            Msg.M("\ti_ctl_branchtarget: " + String.format("%08x", i_ctl_branchtarget));
            Msg.M("\ti_ctl_jump: " + i_ctl_jump);
            Msg.M("\ti_ctl_branch: " + i_ctl_branch);

            Msg.M("\ti_data_imm_signExtended: " + String.format("%08x",i_data_imm_signExtended));
            Msg.M("\ti_data_alu_in: " + String.format("%08x",i_data_alu_in));
            Msg.M("\ti_data_rt: " + String.format("%08x",i_data_rt));
        }

        public void printinstr() {
            Msg.M("ex:   " + ((instrAddr == -1) ? "--------" : String.format("%08x", instrAddr)) +
                     " instr: " + String.format("%08x", instruction) +
                     " : " + MIPSInstr.format(instruction));
        }

        private int eval() {
            try {

            mem_reg.i_instruction = instruction;
            mem_reg.i_instrAddr = instrAddr;

            if(this.hot) {
                this.hot = false;
                mem_reg.hot = true;
            }

            if(instrAddr == -1) return Constants.PLP_OK; // cleared

            mem_reg.i_fwd_ctl_memtoreg = fwd_ctl_memtoreg;
            mem_reg.i_fwd_ctl_regwrite = fwd_ctl_regwrite;
            mem_reg.i_fwd_ctl_dest_reg_addr = (ctl_regDst == 1) ? ctl_rd_addr : ctl_rt_addr;

            mem_reg.i_ctl_memwrite = fwd_ctl_memwrite;
            mem_reg.i_ctl_memread = fwd_ctl_memread;
            mem_reg.i_fwd_ctl_linkaddr = fwd_ctl_linkaddr;

            mem_reg.i_fwd_ctl_jal = fwd_ctl_jal;

            mem_reg.i_data_memwritedata = data_rt;

            internal_alu_out =
                exAlu.eval(data_alu_in,
                           ((ctl_aluSrc == 1) ? data_imm_signExtended : data_rt),
                           ctl_aluOp) & (((long) 0xfffffff << 4) | 0xf);
            
            mem_reg.i_fwd_data_alu_result = internal_alu_out;

            ctl_pcsrc = (internal_alu_out == 1) ? 1 : 0;
            ctl_pcsrc &= ctl_branch;

            byte opcode = MIPSInstr.opcode(instruction);
            int jtype = Asm.lookupInstrType(Asm.lookupInstrOpcode(opcode));

            ctl_jumptarget = (jtype == 7) ? (instrAddr & 0xF0000000) |
                                            (MIPSInstr.jaddr(instruction) << 2)
                                            : data_alu_in;

            // Jump / branch taken, clear next IF stage / create a bubble
            if(ctl_jump == 1 || ctl_pcsrc == 1 && !ex_stall) {
                if_stall = true;
                sim_flags |= Constants.PLP_SIM_IF_STALL_SET;
            }

            return Constants.PLP_OK;

            } catch(Exception e) {
                return Msg.E("I screwed up: " + e,
                                Constants.PLP_SIM_EVALUATION_FAILED, this);
            }
        }

        private void clock() {
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

            data_alu_in = i_data_alu_in;
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
     * This is the memory (MEM) stage of the MIPS pipeline. Writeback module
     * is attached to this class. The bus module is also attached to this
     * class, and it is the only place where the MIPS simulation core interacts
     * with the outside world aside from fetching instructions in IF stage.
     */
    public class mem {
        boolean hot = false;
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

        public long data_memwritedata;

        public long ctl_regwrite;

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

        public long i_ctl_regwrite;

        private wb   wb_reg;
        private PLPSimBus bus;

        public mem(wb wb_reg, PLPSimBus bus) {
            this.wb_reg = wb_reg;
            this.bus = bus;
        }

        public void printvars() {
            Msg.M("MEM vars");
            Msg.M("\tinstruction: " + String.format("%08x", instruction) + " " + MIPSInstr.format(instruction));
            Msg.M("\tinstrAddr: " + ((instrAddr == -1) ? "--------" : String.format("%08x", instrAddr)));

            // MEM stage pipeline registers
            Msg.M("\tfwd_ctl_memtoreg: " + fwd_ctl_memtoreg);
            Msg.M("\tfwd_ctl_regwrite: " + fwd_ctl_regwrite);
            Msg.M("\tfwd_ctl_dest_reg_addr: " + fwd_ctl_dest_reg_addr);
            Msg.M("\tfwd_ctl_linkaddr: " + String.format("%08x",fwd_ctl_linkaddr));
            Msg.M("\tfwd_ctl_jal: " + fwd_ctl_jal);
            Msg.M("\tfwd_data_alu_result: " + String.format("%08x",fwd_data_alu_result));

            Msg.M("\tctl_regwrite: " + ctl_regwrite);
            Msg.M("\tctl_memwrite: " + ctl_memwrite);
            Msg.M("\tctl_memread: " + ctl_memread);

            Msg.M("\tdata_memwritedata: " + String.format("%08x",data_memwritedata));
        }

        public void printnextvars() {
            Msg.M("MEM next vars");
            Msg.M("\ti_instruction: " + String.format("%08x", i_instruction) + " " + MIPSInstr.format(i_instruction));
            Msg.M("\ti_instrAddr: " + ((i_instrAddr == -1) ? "--------" : String.format("%08x", i_instrAddr)));

            // MEM stage pipeline registers
            Msg.M("\ti_fwd_ctl_memtoreg: " + i_fwd_ctl_memtoreg);
            Msg.M("\ti_fwd_ctl_regwrite: " + i_fwd_ctl_regwrite);
            Msg.M("\ti_fwd_ctl_dest_reg_addr: " + i_fwd_ctl_dest_reg_addr);
            Msg.M("\ti_fwd_ctl_linkaddr: " + String.format("%08x",i_fwd_ctl_linkaddr));
            Msg.M("\ti_fwd_ctl_jal: " + i_fwd_ctl_jal);
            Msg.M("\ti_fwd_data_alu_result: " + String.format("%08x",i_fwd_data_alu_result));

            Msg.M("\ti_ctl_regwrite: " + i_ctl_regwrite);
            Msg.M("\ti_ctl_memwrite: " + i_ctl_memwrite);
            Msg.M("\ti_ctl_memread: " + i_ctl_memread);

            Msg.M("\ti_data_memwritedata: " + String.format("%08x",i_data_memwritedata));
        }

        public void printinstr() {
            Msg.M("mem:  " + ((instrAddr == -1) ? "--------" : String.format("%08x", instrAddr)) +
                     " instr: " + String.format("%08x", instruction) +
                     " : " + MIPSInstr.format(instruction));
        }

        private int eval() {
            try {

            wb_reg.i_instruction = instruction;
            wb_reg.i_instrAddr = instrAddr;

            if(this.hot) {
                this.hot = false;
                wb_reg.hot = true;
            }

            if(instrAddr == -1) return Constants.PLP_OK; // cleared

            wb_reg.i_instruction = instruction;
            wb_reg.i_instrAddr = instrAddr;

            wb_reg.i_ctl_memtoreg = fwd_ctl_memtoreg;
            wb_reg.i_ctl_regwrite = fwd_ctl_regwrite;
            wb_reg.i_ctl_dest_reg_addr = fwd_ctl_dest_reg_addr;
            wb_reg.i_ctl_jal = fwd_ctl_jal;
            wb_reg.i_ctl_linkaddr = fwd_ctl_linkaddr;
        
            wb_reg.i_data_alu_result = fwd_data_alu_result;

            if(ctl_memread == 1) {
                if(bus.read(fwd_data_alu_result) == null)
                    return Msg.E("The bus returned no data, check previous error.",
                                    Constants.PLP_SIM_BUS_ERROR, this);
                wb_reg.i_data_memreaddata = (Long) bus.read(fwd_data_alu_result);
            }

            if(ctl_memwrite == 1)
                if(bus.write(fwd_data_alu_result, data_memwritedata, false) != Constants.PLP_OK)
                    return Msg.E("Write failed, check previous error.",
                                    Constants.PLP_SIM_BUS_ERROR, this);;

            return Constants.PLP_OK;

            } catch(Exception e) {
                return Msg.E("I screwed up: " + e,
                                Constants.PLP_SIM_EVALUATION_FAILED, this);
            }
        }

        private void clock() {
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

            ctl_regwrite = i_ctl_regwrite;
        }

        @Override public String toString() {
            return "mips.SimCore.mem(addr:" + String.format("%08x", instrAddr)
                    + " instr: " + MIPSInstr.format(instruction) + ")";
        }
    }

    /**
     * This is the writeback (WB) and final stage of the MIPS pipeline.
     * Instructions are retired and register file is written to in this
     * stage.
     */
    public class wb {
        boolean hot = false;
        boolean instr_retired = false;
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
            Msg.M("WB vars");
            Msg.M("\tinstruction: " + String.format("%08x", instruction) + " " + MIPSInstr.format(instruction));
            Msg.M("\tinstrAddr: " + ((instrAddr == -1) ? "--------" : String.format("%08x", instrAddr)));

            // WB stage pipeline registers
            Msg.M("\tctl_memtoreg: " + ctl_memtoreg);
            Msg.M("\tctl_regwrite: " + ctl_regwrite);
            Msg.M("\tctl_dest_reg_addr: " + ctl_dest_reg_addr);
            Msg.M("\tctl_linkaddr: " + String.format("%08x",ctl_linkaddr));
            Msg.M("\tctl_jal: " + ctl_jal);

            Msg.M("\tdata_memreaddata: " + String.format("%08x",data_memreaddata));
            Msg.M("\tdata_alu_result: " + String.format("%08x",data_alu_result));
        }

        public void printnextvars() {
            Msg.M("WB next vars");
            Msg.M("\ti_instruction: " + String.format("%08x", i_instruction) + " " + MIPSInstr.format(i_instruction));
            Msg.M("\ti_instrAddr: " + ((i_instrAddr == -1) ? "--------" : String.format("%08x", i_instrAddr)));

            // WB stage pipeline registers
            Msg.M("\ti_ctl_memtoreg: " + i_ctl_memtoreg);
            Msg.M("\ti_ctl_regwrite: " + i_ctl_regwrite);
            Msg.M("\ti_ctl_dest_reg_addr: " + i_ctl_dest_reg_addr);
            Msg.M("\ti_ctl_linkaddr: " + String.format("%08x",i_ctl_linkaddr));
            Msg.M("\ti_ctl_jal: " + i_ctl_jal);

            Msg.M("\ti_data_memreaddata: " + String.format("%08x",i_data_memreaddata));
            Msg.M("\ti_data_alu_result: " + String.format("%08x",i_data_alu_result));
        }

        public void printinstr() {
            Msg.M("wb:   " + ((instrAddr == -1) ? "--------" : String.format("%08x", instrAddr)) +
                     " instr: " + String.format("%08x", instruction) +
                     " : " + MIPSInstr.format(instruction));
        }
    
        private int eval() {
            try {

            if(this.hot) {
                this.hot = false;
                instr_retired = true;
            }

            if(instrAddr == -1) return Constants.PLP_OK; // cleared

            long internal_2x1 = (ctl_jal == 0) ?
                                 data_alu_result : ctl_linkaddr;

            if(ctl_regwrite == 1 && ctl_dest_reg_addr != 0)
                regfile.write(ctl_dest_reg_addr,
                    (Long) ((ctl_memtoreg == 0) ? internal_2x1 : data_memreaddata), false);

            return Constants.PLP_OK;

            } catch(Exception e) {
                return Msg.E("I screwed up: " + e,
                                Constants.PLP_SIM_EVALUATION_FAILED, this);
            }
        }

        private void clock() {
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

            return 0;
        }
    }

    /**
     * The MIPS forwarding module scans the pipeline stages when evaluated
     * and will overwrite signals to avoid hazards.
     */
    public class mod_forwarding {

        public boolean mem_mem = true;
        public boolean mem_ex_rtype = true;
        public boolean mem_ex_itype = true;
        public boolean mem_ex_lw = true;
        public boolean ex_ex_rtype = true;
        public boolean ex_ex_itype = true;

        public mod_forwarding() {

        }

        public int eval(SimCore.id id_stage, SimCore.ex ex_stage,
                         SimCore.mem mem_stage, SimCore.wb wb_stage) {

            byte mem_opcode =   MIPSInstr.opcode(mem_stage.instruction);
            int mem_instrType = Asm.lookupInstrType(Asm.lookupInstrOpcode(mem_opcode));
            byte ex_opcode =   MIPSInstr.opcode(ex_stage.instruction);
            int ex_instrType = Asm.lookupInstrType(Asm.lookupInstrOpcode(ex_opcode));
            byte id_opcode =   MIPSInstr.opcode(id_stage.instruction);
            int id_instrType = Asm.lookupInstrType(Asm.lookupInstrOpcode(id_opcode));
            
            long mem_rt =       MIPSInstr.rt(mem_stage.instruction);
            long mem_rd =       MIPSInstr.rd(mem_stage.instruction);
            long id_rt =        MIPSInstr.rt(id_stage.instruction);
            long id_rs =        MIPSInstr.rs(id_stage.instruction);
            long ex_rt =        MIPSInstr.rt(ex_stage.instruction);
            long ex_rd =        MIPSInstr.rd(ex_stage.instruction);

            boolean mem_instr_is_branch = (mem_instrType == 3) ? true : false;
            boolean ex_instr_is_branch = (ex_instrType == 3) ? true : false;
            boolean id_instr_is_branch = (id_instrType == 3) ? true : false;
            boolean mem_instr_is_itype =
                    (mem_instrType >= 3 && mem_instrType <= 6) ? true : false;
            boolean ex_instr_is_itype =
                    (ex_instrType >= 3 && ex_instrType <= 6) ? true : false;
            boolean id_instr_is_itype =
                    (id_instrType >= 3 && id_instrType <= 6) ? true : false;

            if(wb_stage.hot && mem_stage.fwd_ctl_regwrite == 1) {
                // MEM->MEM
                if(mem_rt == ex_rt && mem_rt != 0 && ex_rt != 0 &&
                        mem_stage.ctl_memread == 1 && mem_mem) {
                    mem_stage.i_data_memwritedata = wb_stage.i_data_memreaddata;
                    sim_flags |= Constants.PLP_SIM_FWD_MEM_MEM;
                }

                // MEM->EX forward
                if(mem_ex_rtype && !mem_instr_is_itype) {
                    if(mem_rd == id_rt && mem_rd != 0 && id_rt != 0 &&
                            (id_opcode == 0x2B || // rt in SW is source reg.
                            !id_instr_is_itype || id_instr_is_branch)) {
                        ex_stage.i_data_rt = (mem_stage.ctl_memread == 0) ?
                            mem_stage.fwd_data_alu_result : wb_stage.i_data_memreaddata;
                        sim_flags |= Constants.PLP_SIM_FWD_MEM_EX_RTYPE;
                    }
                    if(mem_rd == id_rs && mem_rd != 0 && id_rs != 0) {
                        ex_stage.i_data_alu_in = (mem_stage.ctl_memread == 0) ?
                            mem_stage.fwd_data_alu_result : wb_stage.i_data_memreaddata;
                        sim_flags |= Constants.PLP_SIM_FWD_MEM_EX_RTYPE;
                    }
                }
                if(mem_ex_itype && mem_instr_is_itype && !mem_instr_is_branch) {
                    if(mem_rt == id_rt && mem_rt != 0 && id_rt != 0 &&
                            (id_opcode == 0x2B || // rt in SW is source reg.
                            !id_instr_is_itype || id_instr_is_branch)) {
                        ex_stage.i_data_rt = (mem_stage.ctl_memread == 0) ?
                            mem_stage.fwd_data_alu_result : wb_stage.i_data_memreaddata;
                        sim_flags |= Constants.PLP_SIM_FWD_MEM_EX_ITYPE;
                    }
                    if(mem_rt == id_rs && mem_rt != 0 && id_rs != 0) {
                        ex_stage.i_data_alu_in = (mem_stage.ctl_memread == 0) ?
                            mem_stage.fwd_data_alu_result : wb_stage.i_data_memreaddata;
                        sim_flags |= Constants.PLP_SIM_FWD_MEM_EX_ITYPE;
                    }
                }
            }

            if(mem_stage.hot && mem_ex_lw) {
                // MEM->EX Load Word, stall
                if(ex_rt == id_rt && ex_rt != 0 && id_rt != 0 && ex_stage.fwd_ctl_memread == 1
                        && (id_opcode == 0x2B || !id_instr_is_itype || id_instr_is_branch)) {
                    ex_stall = true;
                    sim_flags |= Constants.PLP_SIM_FWD_MEM_EX_LW;
                }
                if(ex_rt == id_rs && ex_rt != 0 && id_rs != 0 && ex_stage.fwd_ctl_memread == 1) {
                    ex_stall = true;
                    sim_flags |= Constants.PLP_SIM_FWD_MEM_EX_LW;
                }
            }

            if(mem_stage.hot && ex_stage.fwd_ctl_regwrite == 1) {
                // EX->EX
                if(ex_ex_rtype && !ex_instr_is_itype) {
                    if(ex_rd == id_rs && ex_rd != 0 && id_rs != 0) {
                        ex_stage.i_data_alu_in = mem_stage.i_fwd_data_alu_result;
                        sim_flags |= Constants.PLP_SIM_FWD_EX_EX_RTYPE;
                    }
                    if(ex_rd == id_rt && ex_rd != 0 && id_rt != 0 &&
                            (id_opcode == 0x2B || // rt in SW is source reg.
                            !id_instr_is_itype || id_instr_is_branch)) {
                        ex_stage.i_data_rt = mem_stage.i_fwd_data_alu_result;
                        sim_flags |= Constants.PLP_SIM_FWD_EX_EX_RTYPE;
                    }
                }
                if(mem_ex_itype && ex_instr_is_itype && !ex_instr_is_branch) {
                    if(ex_rt == id_rs && ex_rt != 0 && id_rs != 0 &&
                            ex_stage.fwd_ctl_memwrite != 1) {
                        ex_stage.i_data_alu_in = mem_stage.i_fwd_data_alu_result;
                        sim_flags |= Constants.PLP_SIM_FWD_EX_EX_ITYPE;
                    }
                    if(ex_rt == id_rt && ex_rt != 0 && id_rt != 0 &&
                            (id_opcode == 0x2B || // rt in SW is source reg.
                            !id_instr_is_itype || id_instr_is_branch)) {
                        ex_stage.i_data_rt = mem_stage.i_fwd_data_alu_result;
                        sim_flags |= Constants.PLP_SIM_FWD_EX_EX_ITYPE;
                    }
                }
            }

            return Constants.PLP_OK;
        }
    }
   
}


