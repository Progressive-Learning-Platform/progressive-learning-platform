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

import plptool.mods.MemModule;
import plptool.PLPCfg;
import plptool.PLPMsg;
import plptool.Constants;
import plptool.PLPSimBus;
import plptool.PLPSimCore;
import plptool.PLPSimRegModule;

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
     * Stallers
     */
    boolean if_stall;
    boolean ex_stall;

    /**
     * The assembler object passed to the simulator.
     *
     * @see plptool.PLPAsm
     */
    Asm asm;

    /**
     * Simulator plp constructor.
     *
     * @param RAMsize the size of main memory attached to this core
     * @param startAddr the starting address of the main memory
     * @param asm assembler object passed on to this simulator
     * @see plptool.PLPAsm
     */
    public SimCore(Asm asm, long startAddr, long RAMsize) {
        if(RAMsize <= 0)
            RAMsize = (long) Math.pow(2, 31);

        this.startAddr = startAddr;

        memory = new MemModule(startAddr, RAMsize, Constants.FLAGS_ALIGNED_MEMORY);
        regfile = new MemModule(0, 32, false);
        pc = new PLPSimRegModule(0); // pc=0 on reset

        this.asm = asm;
        this.objCode = asm.getObjectCode();
        this.addrTable = asm.getAddrTable();

        sim_flags = (long) 0;

        // core mods
        forwarding = new mod_forwarding();
        bus = new PLPSimBus(memory);

        // Instantiate stages
        wb_stage = new wb(regfile);
        mem_stage = new mem(wb_stage, bus);
        ex_stage = new ex(mem_stage, new alu());
        id_stage = new id(ex_stage, regfile);

        memory.enable();
        regfile.enable();
    }

    /**
     * This function resets the core: clears RAM and zeroes out register file,
     * reloads the program to memory, resets program counter, flushes the
     * pipeline, clears flags, and resets statistics.
     *
     * @return Returns 0 on successful completion. Should not fail.
     * @see softreset()
     */
    public int reset() {
        int i;
        
        // clear RAM
        memory.clear();

        // init regfile
        for(i = 0; i < 32; i++)
            regfile.write(i, new Long(0), false);

        loadProgram(asm);

        pc.reset(startAddr);
        instructionCount = 0;
        sim_flags = (long) 0;
        ex_stall = false;
        if_stall = false;
        flushpipeline();

        PLPMsg.M("core: reset");

        return Constants.PLP_OK;
    }

    /**
     * This function resets the program counter and flushes the pipeline.
     *
     * @return Returns 0 on completion.
     * @see reset()
     */
    public int softreset() {
        pc.reset(startAddr);
        flushpipeline();

        ex_stall = false;
        if_stall = false;

        PLPMsg.M("core: soft reset");

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
            return PLPMsg.E("Unable to write program to memory",
                            Constants.PLP_SIM_UNABLE_TO_LOAD_PROGRAM, this);

        return ret;
    }

    /**
     * Advances the simulation by one cycle.
     *
     * @return Returns 0 on successful completion. Error code otherwise.
     */
    public int step () {
        sim_flags = (long) 0;
        instructionCount++;
        wb_stage.instr_retired = false;
        int ret = 0;
        long old_pc = pc.eval();

        // Propagate values
        if(wb_stage.hot)  wb_stage.clock();
        if(mem_stage.hot) mem_stage.clock();
        if(ex_stage.hot)  ex_stage.clock();
        if(id_stage.hot)  id_stage.clock();

        // clock pc for next instruction
        if(!if_stall)
            pc.clock();

        // Evaluate stages
        ret += wb_stage.eval();
        ret += mem_stage.eval();
        ret += ex_stage.eval();
        ret += id_stage.eval();
        
        // Evaluate modules attached to the bus
        ret += bus.eval();

        // See if we're stalled on EX
        if(ex_stall) {
            ex_stage.instruction = 0;
            mem_stage.i_instruction = 0;
            ex_stall = false;
            mem_stage.hot = true;
            ex_stage.hot = true;
            id_stage.hot = true;
        }
        
        // Engage forwarding unit
        if(PLPCfg.cfgSimForwardingUnit)
            forwarding.eval(id_stage, ex_stage, mem_stage, wb_stage);

        if(ret != 0) {
            return PLPMsg.E("Evaluation failed.",
                            Constants.PLP_SIM_EVALUATION_FAILED, this);
        }

        // pc update logic (input side IF)
        if(ex_stage.hot && ex_stage.ctl_pcsrc == 1)
            pc.write(ex_stage.ctl_branchtarget);
        else if(ex_stage.hot && ex_stage.ctl_jump == 1)
            pc.write(ex_stage.ctl_jumptarget);
        else if(!if_stall)
            pc.write(pc.eval() + 4);

        // We're stalled in this cycle, do not fetch new instruction
        if(if_stall) {
            if_stall = false;
            id_stage.i_instruction = 0;
            //id_stage.i_instrAddr = -1;

            return Constants.PLP_OK;
        }
        else {
            return fetch();
        }
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

        if(!bus.isInitialized(addr))
            return PLPMsg.E("step(): Memory location uninitialized: addr=" +
                            String.format("%08x", addr),
                            Constants.PLP_SIM_UNINITIALIZED_MEMORY, this);

        // fetch instruction / frontend stage
        id_stage.i_instruction = (Long) bus.read(addr);
        id_stage.i_instrAddr = addr;
        id_stage.i_ctl_pcplus4 = addr + 4;

        id_stage.hot = true;

        return Constants.PLP_OK;
    }

    /**
     * Clear all pipeline stages, in-flight instructions are discarded.
     *
     * @return Returns 0 on completion.
     */
    public int flushpipeline() {
        // Zero out everything
        id_stage.i_instruction = 0;
        id_stage.i_instrAddr = 0;
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
     * Print front end states.
     */
    public void printfrontend() {
        PLPMsg.M("if:   " + String.format("%08x", id_stage.i_instrAddr) +
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
    public void printProgram(long highlight) {
        PLPMsg.M("pc\taddress\t\thex\t\tDisassembly");
        PLPMsg.M("--\t-------\t\t---\t\t-----------");
        Object[][] values = super.memory.getValueSet();
        for(int i = 0; i < values.length; i++) {
                if((Long) values[i][0] == highlight)
                    PLPMsg.m(">>>");
                if((Boolean) values[i][2])
                    PLPMsg.M(String.format("\t%08x\t%08x\t",
                                       (Long) values[i][0], (Long) values[i][1]) +
                                       MIPSInstr.format((Long) values[i][1]));
        }
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
            PLPMsg.M("ID vars");
            PLPMsg.M("\tinstruction: " + String.format("%08x", instruction) + " " + MIPSInstr.format(instruction));
            PLPMsg.M("\tinstrAddr: " + String.format("%08x", instrAddr));
            PLPMsg.M("\tctl_pcplus4: " + String.format("%08x", ctl_pcplus4));
        }

        public void printnextvars() {
            PLPMsg.M("ID next vars");
            PLPMsg.M("\ti_instruction: " + String.format("%08x", i_instruction));
            PLPMsg.M("\ti_instrAddr: " + String.format("%08x", i_instrAddr));
            PLPMsg.M("\ti_ctl_pcplus4: " + String.format("%08x", i_ctl_pcplus4));
        }

        public void printinstr() {
            PLPMsg.M("id:   " + String.format("%08x", instrAddr) +
                     " instr: " + String.format("%08x", instruction) +
                     " : " + MIPSInstr.format(instruction));
        }

        private int eval() {
            try {

            ex_reg.i_instruction = instruction;
            ex_reg.i_instrAddr = instrAddr;

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

            ex_reg.i_fwd_ctl_linkaddr = ctl_pcplus4;

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
                    case 9: // jalr
                        ex_reg.i_ctl_jump = 1;
                        ex_reg.i_fwd_ctl_regwrite = 1;
                        ex_reg.i_ctl_regDst = 1;
                        ex_reg.i_fwd_ctl_jal = 1;

                        break;
                    default:
                        return PLPMsg.E("Unhandled instruction type.",
                                        Constants.PLP_SIM_UNHANDLED_INSTRUCTION_TYPE,
                                        this);
                }
            } else {
                switch(Asm.lookupInstrType(Asm.lookupInstrFunct(funct))) {
                    case 0: // r-types
                    case 1: // shifts
                        ex_reg.i_fwd_ctl_regwrite = 1;
                        ex_reg.i_ctl_regDst = 1;
                        break;

                    case 2: // jr
                        ex_reg.i_ctl_jump = 1;
                        break;

                    default:
                        return PLPMsg.E("Unhandled instruction type.",
                                        Constants.PLP_SIM_UNHANDLED_INSTRUCTION_TYPE,
                                        this);
                }
            }

            ex_reg.i_ctl_branchtarget = ctl_pcplus4 +
                               ((short) ex_reg.i_data_imm_signExtended << 2);

            if(this.hot) {
                this.hot = false;
                ex_reg.hot = true;
            }

            return Constants.PLP_OK;

            } catch(Exception e) {
                return PLPMsg.E("I screwed up: " + e,
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
            PLPMsg.M("EX vars");
            PLPMsg.M("\tinstruction: " + String.format("%08x", instruction) + " " + MIPSInstr.format(instruction));
            PLPMsg.M("\tinstrAddr: " + String.format("%08x",instrAddr));

            // EX stage pipeline registers
            PLPMsg.M("\tfwd_ctl_memtoreg: " + fwd_ctl_memtoreg);
            PLPMsg.M("\tfwd_ctl_regwrite: " + fwd_ctl_regwrite);
            PLPMsg.M("\tfwd_ctl_memwrite: " + fwd_ctl_memwrite);
            PLPMsg.M("\tfwd_ctl_memread: " + fwd_ctl_memread);
            PLPMsg.M("\tfwd_ctl_linkaddr: " + String.format("%08x",fwd_ctl_linkaddr));
            PLPMsg.M("\tfwd_ctl_jal: " + fwd_ctl_jal);

            PLPMsg.M("\tctl_aluSrc: " + ctl_aluSrc);
            PLPMsg.M("\tctl_aluOp: " + String.format("%08x",ctl_aluOp));
            PLPMsg.M("\tctl_regDst: " + ctl_regDst);

            PLPMsg.M("\tctl_branchtarget: " + String.format("%08x", ctl_branchtarget));
            PLPMsg.M("\tctl_jumptarget: " + String.format("%08x", ctl_jumptarget));
            PLPMsg.M("\tctl_pcsrc: " + ctl_pcsrc);
            PLPMsg.M("\tctl_jump: " + ctl_jump);
            PLPMsg.M("\tctl_branch: " + ctl_branch);

            PLPMsg.M("\tdata_imm_signExtended: " + String.format("%08x",data_imm_signExtended));
            PLPMsg.M("\tdata_alu_in: " + String.format("%08x",data_alu_in));
            PLPMsg.M("\tdata_rt: " + String.format("%08x",data_rt));

            PLPMsg.M("\tinternal_alu_out: " + String.format("%08x",internal_alu_out));
        }

        public void printnextvars() {
            PLPMsg.M("EX next vars");
            PLPMsg.M("\ti_instruction: " + String.format("%08x",i_instruction));
            PLPMsg.M("\ti_instrAddr: " + String.format("%08x",i_instrAddr));

            // EX stage pipeline registers
            PLPMsg.M("\ti_fwd_ctl_memtoreg: " + i_fwd_ctl_memtoreg);
            PLPMsg.M("\ti_fwd_ctl_regwrite: " + i_fwd_ctl_regwrite);
            PLPMsg.M("\ti_fwd_ctl_memwrite: " + i_fwd_ctl_memwrite);
            PLPMsg.M("\ti_fwd_ctl_memread: " + i_fwd_ctl_memread);
            PLPMsg.M("\ti_fwd_ctl_linkaddr: " + String.format("%08x",i_fwd_ctl_linkaddr));
            PLPMsg.M("\ti_fwd_ctl_jal: " + i_fwd_ctl_jal);

            PLPMsg.M("\ti_ctl_aluSrc: " + i_ctl_aluSrc);
            PLPMsg.M("\ti_ctl_aluOp: " + String.format("%08x",i_ctl_aluOp));
            PLPMsg.M("\ti_ctl_regDst: " + i_ctl_regDst);

            PLPMsg.M("\ti_ctl_branchtarget: " + String.format("%08x", i_ctl_branchtarget));
            PLPMsg.M("\ti_ctl_jump: " + i_ctl_jump);
            PLPMsg.M("\ti_ctl_branch: " + i_ctl_branch);

            PLPMsg.M("\ti_data_imm_signExtended: " + String.format("%08x",i_data_imm_signExtended));
            PLPMsg.M("\ti_data_alu_in: " + String.format("%08x",i_data_alu_in));
            PLPMsg.M("\ti_data_rt: " + String.format("%08x",i_data_rt));
        }

        public void printinstr() {
            PLPMsg.M("ex:   " + String.format("%08x", instrAddr) +
                     " instr: " + String.format("%08x", instruction) +
                     " : " + MIPSInstr.format(instruction));
        }

        private int eval() {
            try {

            mem_reg.i_instruction = instruction;
            mem_reg.i_instrAddr = instrAddr;

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

            if(this.hot) {
                this.hot = false;
                mem_reg.hot = true;
            }

            return Constants.PLP_OK;

            } catch(Exception e) {
                return PLPMsg.E("I screwed up: " + e,
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
            PLPMsg.M("MEM vars");
            PLPMsg.M("\tinstruction: " + String.format("%08x", instruction) + " " + MIPSInstr.format(instruction));
            PLPMsg.M("\tinstrAddr: " + String.format("%08x",instrAddr));

            // MEM stage pipeline registers
            PLPMsg.M("\tfwd_ctl_memtoreg: " + fwd_ctl_memtoreg);
            PLPMsg.M("\tfwd_ctl_regwrite: " + fwd_ctl_regwrite);
            PLPMsg.M("\tfwd_ctl_dest_reg_addr: " + fwd_ctl_dest_reg_addr);
            PLPMsg.M("\tfwd_ctl_linkaddr: " + String.format("%08x",fwd_ctl_linkaddr));
            PLPMsg.M("\tfwd_ctl_jal: " + fwd_ctl_jal);
            PLPMsg.M("\tfwd_data_alu_result: " + String.format("%08x",fwd_data_alu_result));

            PLPMsg.M("\tctl_regwrite: " + ctl_regwrite);
            PLPMsg.M("\tctl_memwrite: " + ctl_memwrite);
            PLPMsg.M("\tctl_memread: " + ctl_memread);

            PLPMsg.M("\tdata_memwritedata: " + String.format("%08x",data_memwritedata));
        }

        public void printnextvars() {
            PLPMsg.M("MEM next vars");
            PLPMsg.M("\ti_instruction: " + String.format("%08x",i_instruction));
            PLPMsg.M("\ti_instrAddr: " + String.format("%08x",i_instrAddr));

            // MEM stage pipeline registers
            PLPMsg.M("\ti_fwd_ctl_memtoreg: " + i_fwd_ctl_memtoreg);
            PLPMsg.M("\ti_fwd_ctl_regwrite: " + i_fwd_ctl_regwrite);
            PLPMsg.M("\ti_fwd_ctl_dest_reg_addr: " + i_fwd_ctl_dest_reg_addr);
            PLPMsg.M("\ti_fwd_ctl_linkaddr: " + String.format("%08x",i_fwd_ctl_linkaddr));
            PLPMsg.M("\ti_fwd_ctl_jal: " + i_fwd_ctl_jal);
            PLPMsg.M("\ti_fwd_data_alu_result: " + String.format("%08x",i_fwd_data_alu_result));

            PLPMsg.M("\ti_ctl_regwrite: " + i_ctl_regwrite);
            PLPMsg.M("\ti_ctl_memwrite: " + i_ctl_memwrite);
            PLPMsg.M("\ti_ctl_memread: " + i_ctl_memread);

            PLPMsg.M("\ti_data_memwritedata: " + String.format("%08x",i_data_memwritedata));
        }

        public void printinstr() {
            PLPMsg.M("mem:  " + String.format("%08x", instrAddr) +
                     " instr: " + String.format("%08x", instruction) +
                     " : " + MIPSInstr.format(instruction));
        }

        private int eval() {
            try {

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
                    return PLPMsg.E("Attempted to read from uninitialized memory.",
                                    Constants.PLP_SIM_EVALUATION_FAILED, this);
                wb_reg.i_data_memreaddata = (Long) bus.read(fwd_data_alu_result);
            }

            if(ctl_memwrite == 1)
                bus.write(fwd_data_alu_result, data_memwritedata, false);

            if(this.hot) {
                this.hot = false;
                wb_reg.hot = true;
            }

            return Constants.PLP_OK;

            } catch(Exception e) {
                return PLPMsg.E("I screwed up: " + e,
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
            PLPMsg.M("WB vars");
            PLPMsg.M("\tinstruction: " + String.format("%08x", instruction) + " " + MIPSInstr.format(instruction));
            PLPMsg.M("\tinstrAddr: " + String.format("%08x",instrAddr));

            // WB stage pipeline registers
            PLPMsg.M("\tctl_memtoreg: " + ctl_memtoreg);
            PLPMsg.M("\tctl_regwrite: " + ctl_regwrite);
            PLPMsg.M("\tctl_dest_reg_addr: " + ctl_dest_reg_addr);
            PLPMsg.M("\tctl_linkaddr: " + String.format("%08x",ctl_linkaddr));
            PLPMsg.M("\tctl_jal: " + ctl_jal);

            PLPMsg.M("\tdata_memreaddata: " + String.format("%08x",data_memreaddata));
            PLPMsg.M("\tdata_alu_result: " + String.format("%08x",data_alu_result));
        }

        public void printnextvars() {
            PLPMsg.M("WB next vars");
            PLPMsg.M("\ti_instruction: " + String.format("%08x",i_instruction));
            PLPMsg.M("\ti_instrAddr: " + String.format("%08x",i_instrAddr));

            // WB stage pipeline registers
            PLPMsg.M("\ti_ctl_memtoreg: " + i_ctl_memtoreg);
            PLPMsg.M("\ti_ctl_regwrite: " + i_ctl_regwrite);
            PLPMsg.M("\ti_ctl_dest_reg_addr: " + i_ctl_dest_reg_addr);
            PLPMsg.M("\ti_ctl_linkaddr: " + String.format("%08x",i_ctl_linkaddr));
            PLPMsg.M("\ti_ctl_jal: " + i_ctl_jal);

            PLPMsg.M("\ti_data_memreaddata: " + String.format("%08x",i_data_memreaddata));
            PLPMsg.M("\ti_data_alu_result: " + String.format("%08x",i_data_alu_result));
        }

        public void printinstr() {
            PLPMsg.M("wb:   " + String.format("%08x", instrAddr) +
                     " instr: " + String.format("%08x", instruction) +
                     " : " + MIPSInstr.format(instruction));
        }
    
        private int eval() {
            try {

            long internal_2x1 = (ctl_jal == 0) ?
                                 data_alu_result : ctl_linkaddr;

            if(ctl_regwrite == 1 && ctl_dest_reg_addr != 0)
                regfile.write(ctl_dest_reg_addr,
                    (Long) ((ctl_memtoreg == 0) ? internal_2x1 : data_memreaddata), false);

            if(this.hot) {
                this.hot = false;
                instr_retired = true;
            }

            return Constants.PLP_OK;

            } catch(Exception e) {
                return PLPMsg.E("I screwed up: " + e,
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
                        case 0x00: return a << b;
                        case 0x02: return a >> b;
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

    public class mod_breakpoint {
        public mod_breakpoint() {

        }
    }

    public class mod_monitor {
        public mod_monitor() {
            
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

            if(wb_stage.hot && mem_ex_lw) {
                // MEM->EX Load Word, stall
                if(ex_rt == id_rt && ex_rt != 0 && id_rt != 0 && ex_stage.fwd_ctl_memread == 1
                        && (id_opcode == 0x2B || !id_instr_is_itype || id_instr_is_branch)) {
                    ex_stage.i_data_rt = wb_stage.i_data_memreaddata;
                    ex_stage.hot = false;
                    id_stage.hot = false;
                    if_stall = true;
                    ex_stall = true;
                    sim_flags |= Constants.PLP_SIM_FWD_MEM_EX_LW;
                }
                if(ex_rt == id_rs && ex_rt != 0 && id_rs != 0 && ex_stage.fwd_ctl_memread == 1) {
                    ex_stage.i_data_alu_in = wb_stage.i_data_memreaddata;
                    ex_stage.hot = false;
                    id_stage.hot = false;
                    if_stall = true;
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

