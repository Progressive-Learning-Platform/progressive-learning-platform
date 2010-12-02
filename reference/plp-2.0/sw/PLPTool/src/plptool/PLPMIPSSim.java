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

import java.util.Iterator;

/**
 * PLPMIPSSim is the PLP MIPS Architecture Simulator Backend. This class
 * contains core functionality of the simulator: the CPU itself, memory
 * elements, pipeline registers, and I/O devices. Users can interact with the
 * simulation core by using typical procedures such as stepping and I/O. It's
 * up to the command line interface or the GUI to present these features to the
 * user.
 *
 * @author wira
 */
public class PLPMIPSSim {

    /**
     * Collection of modules attached to the simulated front-bus.
     */
    public PLPSimBusModule[]           bus_modules;

    /**
     * The core's program counter.
     */
    public mod_register                   pc;

    /**
     * CPU front-side bus.
     */
    public mod_bus                        bus;

    /**
     * Main memory.
     */
    public mod_memory                     memory;

    /**
     * Register file.
     */
    public mod_memory                     regfile;

    /**
     * Forwarding unit.
     */
    public mod_forwarding                 forwarding;

    /**
     * Numbers of instructions issued.
     */
    private int instructionCount;

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
     * Simulation flags. This flag is used by various modules to indicate
     * whether certain events have occured. Simulation flags are cleared
     * in the beginning of each cycle.
     */
    private Long sim_flags;

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
     * The assembler object passed to the simulator.
     *
     * @see plptool.PLPAsm
     */
    PLPAsm asm;

    /**
     * Simulator backend constructor.
     *
     * @param RAMsize the size of main memory attached to this core
     * @param asm assembler object passed on to this simulator
     * @see plptool.PLPAsm
     */
    public PLPMIPSSim(PLPAsm asm, long RAMsize) {
        if(RAMsize <= 0)
            RAMsize = (long) Math.pow(2, 62);

        memory = new mod_memory(RAMsize, PLPMsg.FLAGS_ALIGNED_MEMORY);
        regfile = new mod_memory(32, false);
        pc = new mod_register(0); // pc=0 on reset

        this.asm = asm;
        this.objCode = asm.getObjectCode();
        this.addrTable = asm.getAddrTable();

        sim_flags = (long) 0;

        // core mods
        forwarding = new mod_forwarding();
        
        // modules attached to the front side bus
        bus_modules = new PLPSimBusModule[3];

        bus_modules[0] = memory;
        bus_modules[1] = new io_example((long) 0x8000000 << 4);
        bus_modules[2] = new io_leds((long) 0x8000001 << 4);
        bus = new mod_bus(bus_modules);

        PLPMsg.M("Built-in modules:");
        for(int i = 0; i < bus_modules.length; i++)
            PLPMsg.M(" " + i + ": " + bus_modules[i].introduce());

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
            regfile.write(i, 0, false);

        // load program to RAM
        for(i = 0; i < objCode.length; i++) {
            if((addrTable[i] / 4) >= memory.size())
                PLPMsg.M("Warning: Program doesn't fit in memory.");
            else {
                if (asm.isInstruction(i) == 0)
                    memory.write(addrTable[i], objCode[i], true);
                else
                    memory.write(addrTable[i], objCode[i], false);
            }
        }

        pc.reset(0);
        instructionCount = 0;
        sim_flags = (long) 0;
        flushpipeline();

        PLPMsg.M("core: reset");

        return PLPMsg.PLP_OK;
    }

    /**
     * This function resets the program counter and flushes the pipeline.
     *
     * @return Returns 0 on completion.
     * @see reset()
     */
    public int softreset() {
        pc.reset(0);
        flushpipeline();

        PLPMsg.M("core: soft reset");

        return PLPMsg.PLP_OK;
    }

    /**
     * Advances the simulation by one cycle.
     *
     * @return Returns 0 on successful completion. Error code otherwise.
     */
    public int step () {
        instructionCount++;
        int ret = 0;

        // Propagate values
        if(id_stage.hot)  id_stage.clock();
        if(ex_stage.hot)  ex_stage.clock();
        if(mem_stage.hot) mem_stage.clock();
        if(wb_stage.hot)  wb_stage.clock();

        // clock pc for next instruction
        pc.clock();

        // Evaluate stages
        if(wb_stage.hot)  ret += wb_stage.eval();
        if(mem_stage.hot) ret += mem_stage.eval();
        if(ex_stage.hot)  ret += ex_stage.eval();
        if(id_stage.hot)  ret += id_stage.eval();

        // Engage forwarding unit
        if(PLPCfg.cfgSimForwardingUnit)
            forwarding.eval(id_stage, ex_stage, mem_stage, wb_stage);

        // Evaluate modules attached to FSB
        for(int i = 0; i < bus_modules.length; i++)
            ret += bus_modules[i].eval();

        if(ret != 0) {
            return PLPMsg.E("Evaluation failed.",
                            PLPMsg.PLP_SIM_EVALUATION_FAILED, this);
        }

        if(id_stage.hot && id_stage.ctl_pcsrc == 1)
            pc.write(id_stage.ctl_branchtarget);
        else if(id_stage.hot && id_stage.ctl_jump == 1)
            pc.write(id_stage.ctl_jumptarget);
        else
            pc.write(pc.eval() + 4);

        return fetch();
    }

    /**
     * Perform an instruction fetch and warm up the decode stage.
     *
     * @return Returns 0 on successful completion. Error code otherwise.
     */
    private int fetch()  {
        long addr = pc.eval();

        if(addr / 4 >= memory.size())
            return PLPMsg.E("step(): Instruction memory out-of-bounds: addr=" +
                            String.format("%08x", addr),
                            PLPMsg.PLP_SIM_INSTRMEM_OUT_OF_BOUNDS, this);

        if(memory.read(addr) == PLPMsg.PLP_ERROR_RETURN)
            return PLPMsg.E("step(): Memory location uninitialized: addr=" +
                            String.format("%08x", addr),
                            PLPMsg.PLP_SIM_UNINITIALIZED_MEMORY, this);

        if(!memory.isInstr(addr))
            return PLPMsg.E("step(): Non-executable memory: addr=" +
                            String.format("%08x", addr),
                            PLPMsg.PLP_SIM_INSTRMEM_OUT_OF_BOUNDS, this);

        // fetch instruction / frontend stage
        id_stage.i_instruction = memory.read(addr);
        id_stage.i_instrAddr = addr;
        id_stage.i_ctl_pcplus4 = addr + 4;

        id_stage.hot = true;

        return PLPMsg.PLP_OK;
    }

    /**
     * Disable all pipeline stages, in-flight instructions are discarded.
     *
     * @return Returns 0 on completion.
     */
    public int flushpipeline() {
        id_stage.hot = false;
        ex_stage.hot = false;
        mem_stage.hot = false;
        wb_stage.hot = false;
        wb_stage.instr_retired = false;

        return PLPMsg.PLP_OK;
    }

    /**
     * Print front end states.
     */
    public void printfrontend() {

        if(pc.eval() < 0)
            PLPMsg.M("if:   -" +
                     "\ni_pc: " + String.format("%08x", pc.input()));
        else
            PLPMsg.M("if:   " + String.format("%08x", pc.eval()) +
                " instr: " + String.format("%08x", id_stage.i_instruction) +
                " : " + MIPSInstr.format(id_stage.i_instruction) +
                "\ni_pc: "  + String.format("%08x", pc.input()));
    }

    /**
     * Returns simulation flags.
     *
     * @return Returns the simulation flags.
     */
    public long getFlags() {
        return sim_flags;
    }

    /**
     * Enable all I/O mods.
     *
     * @return Returns 0 on completion.
     */
    public int enableiomods() {
        for(int i = 0; i < bus_modules.length; i++)
            bus_modules[i].enable();

        return PLPMsg.PLP_OK;
    }

    /**
     * Disable all I/O mods. Including the main memory. Be careful.
     *
     * @return Returns 0 on completion.
     */
    public int disableiomods() {
        for(int i = 0; i < bus_modules.length; i++)
            bus_modules[i].disable();

        return PLPMsg.PLP_OK;
    }

    /**
     * Get the number of instructions that have been issued on this core.
     *
     * @return Returns instruction count in int.
     */
    public int getinstrcount() {
        return instructionCount;
    }

    @Override public String toString() {
        return "PLPMIPSSim(asm: " + asm.toString() + ")";
    }

    // Register file stage / instruction decode
    public class id {
        boolean hot = false;
        long instruction;
        long instrAddr;

        // ID stage pipeline registers
        long i_instruction;
        long i_instrAddr;
        long i_ctl_pcplus4;

        long ctl_pcplus4;

        long ctl_pcsrc;
        long ctl_branch;
        long ctl_jump;

        long ctl_branchtarget;
        long ctl_jumptarget;

        long i_ctl_branch;
        long i_ctl_jump;
        
        private ex   ex_reg;
        private mod_memory regfile;

        public id(ex ex_reg, mod_memory regfile) {
            this.ex_reg = ex_reg;
            this.regfile = regfile;
        }

        public void printvars() {
            PLPMsg.M("ID vars");
            PLPMsg.M("\tinstruction: " + String.format("%08x", instruction) + " " + MIPSInstr.format(instruction));
            PLPMsg.M("\tinstrAddr: " + String.format("%08x", instrAddr));
            PLPMsg.M("\tctl_pcplus4: " + String.format("%08x", ctl_pcplus4));
            PLPMsg.M("\tctl_branchtarget: " + String.format("%08x", ctl_branchtarget));
            PLPMsg.M("\tctl_jumptarget: " + String.format("%08x", ctl_jumptarget));
            PLPMsg.M("\tctl_pcsrc: " + ctl_pcsrc);
            PLPMsg.M("\tctl_branch: " + ctl_branch);
            PLPMsg.M("\tctl_jump: " + ctl_jump);
        }

        public void printnextvars() {
            PLPMsg.M("ID next vars");
            PLPMsg.M("\thot: " + hot);
            PLPMsg.M("\ti_instruction: " + String.format("%08x", i_instruction));
            PLPMsg.M("\ti_instrAddr: " + String.format("%08x", i_instrAddr));
            PLPMsg.M("\ti_ctl_pcplus4: " + String.format("%08x", i_ctl_pcplus4));
        }

        public void printinstr() {
            if(ex_reg.hot)
                PLPMsg.M("id:   " + String.format("%08x", instrAddr) +
                         " instr: " + String.format("%08x", instruction) +
                         " : " + MIPSInstr.format(instruction));
            else
                PLPMsg.M("id:   -");
        }

        private int eval() {
            try {

            ex_reg.i_instruction = instruction;
            ex_reg.i_instrAddr = instrAddr;

            byte opcode      = (byte) MIPSInstr.opcode(instruction);
            byte funct       = (byte) MIPSInstr.funct(instruction);

            long addr_read_0 = MIPSInstr.rt(instruction); // rt
            long addr_read_1 = MIPSInstr.rs(instruction); // rs

            // rt
            ex_reg.i_data_rt     = (addr_read_0 == 0) ?
                                   0 : regfile.read(addr_read_0);

            // rs
            ex_reg.i_data_alu_in = (addr_read_1 == 0) ?
                                   0 : regfile.read(addr_read_1);

            long imm_field = MIPSInstr.imm(instruction);
            ex_reg.i_data_imm_signExtended = imm_field;

            ex_reg.i_ctl_rd_addr = MIPSInstr.rd(instruction); // rd
            ex_reg.i_ctl_rt_addr = addr_read_0;

            ex_reg.i_ctl_aluOp = instruction;

            ex_reg.i_fwd_ctl_linkaddr = ctl_pcplus4;

            // control logic
            ex_reg.i_fwd_ctl_memtoreg = 0;
            ex_reg.i_fwd_ctl_regwrite = 0;
            ex_reg.i_fwd_ctl_memwrite = 0;
            ex_reg.i_fwd_ctl_memread = 0;
            ex_reg.i_fwd_ctl_jal = 0;
            ctl_branch = 0;
            ex_reg.i_ctl_aluSrc = 0;
            ex_reg.i_ctl_regDst = 0;
            ctl_jump = 0;
            ctl_jumptarget = 0;

            if(opcode != 0) {
                switch(PLPAsm.lookupInstrType(PLPAsm.lookupInstrOpcode(opcode))) {
                    case 3:
                        ctl_branch = 1;
                        break;

                    case 4:
                    case 5:
                        ex_reg.i_fwd_ctl_regwrite = 1;
                        ex_reg.i_ctl_aluSrc = 1;

                        break;

                    case 6:
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
                    
                    case 7:
                        ctl_jump = 1;
                        ctl_jumptarget = MIPSInstr.jaddr(instruction) << 2;
                        if(PLPAsm.lookupInstrOpcode(opcode).equals("jal")) {
                            ex_reg.i_fwd_ctl_regwrite = 1;
                            ex_reg.i_ctl_regDst = 1;
                            ex_reg.i_ctl_rd_addr = 31;
                            ex_reg.i_fwd_ctl_jal = 1;
                        }

                        break;
                    default:
                        return PLPMsg.E("Unhandled instruction type.",
                                        PLPMsg.PLP_SIM_UNHANDLED_INSTRUCTION_TYPE,
                                        this);
                }
            } else {
                switch(PLPAsm.lookupInstrType(PLPAsm.lookupInstrFunct(funct))) {
                    case 0:
                    case 1:
                        ex_reg.i_fwd_ctl_regwrite = 1;
                        ex_reg.i_ctl_regDst = 1;
                        break;

                    case 2:
                        ctl_jump = 1;
                        ctl_jumptarget = ex_reg.i_data_alu_in;
                        break;

                    default:
                        return PLPMsg.E("Unhandled instruction type.",
                                        PLPMsg.PLP_SIM_UNHANDLED_INSTRUCTION_TYPE,
                                        this);
                }
            }

            ctl_pcsrc = (ex_reg.i_data_rt == ex_reg.i_data_alu_in) ? 1 : 0;
            ctl_pcsrc &= ctl_branch;
            ctl_branchtarget = ctl_pcplus4 +
                               ((short) ex_reg.i_data_imm_signExtended << 2);
            
            ex_reg.hot = true;

            return PLPMsg.PLP_OK;

            } catch(Exception e) {
                return PLPMsg.E("I screwed up: " + e,
                                PLPMsg.PLP_SIM_EVALUATION_FAILED, this);
            }
        }

        private void clock() {
            ctl_pcplus4 = i_ctl_pcplus4;
            instruction = i_instruction;
            instrAddr = i_instrAddr;
        }

        @Override public String toString() {
            return "PLPMIPSSim.id(addr:" + String.format("%08x", instrAddr)
                    + " instr: " + MIPSInstr.format(instruction) + ")";
        }
    }

    // Execute stage
    public class ex {
        boolean hot = false;
        long instruction;
        long instrAddr;

        // EX stage pipeline registers
        long fwd_ctl_memtoreg;
        long fwd_ctl_regwrite;

        long fwd_ctl_memwrite;
        long fwd_ctl_memread;
        long fwd_ctl_jal;
        long fwd_ctl_linkaddr;

        long ctl_aluSrc;
        long ctl_aluOp;
        long ctl_regDst;

        long data_alu_in;
        long data_rt;

        long data_imm_signExtended;
        long ctl_rt_addr;
        long ctl_rd_addr;

        long i_instruction;
        long i_instrAddr;

        long i_fwd_ctl_memtoreg;
        long i_fwd_ctl_regwrite;

        long i_fwd_ctl_memwrite;
        long i_fwd_ctl_memread;
        long i_fwd_ctl_jal;
        long i_fwd_ctl_linkaddr;

        long i_ctl_aluSrc;
        long i_ctl_aluOp;
        long i_ctl_regDst;

        long i_data_alu_in;
        long i_data_rt;

        long i_data_imm_signExtended;
        long i_ctl_rt_addr;
        long i_ctl_rd_addr;

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
            PLPMsg.M("\tdata_imm_signExtended: " + String.format("%08x",data_imm_signExtended));
            PLPMsg.M("\tdata_alu_in: " + String.format("%08x",data_alu_in));
            PLPMsg.M("\tdata_rt: " + String.format("%08x",data_rt));
        }

        public void printnextvars() {
            PLPMsg.M("EX next vars");
            PLPMsg.M("\thot: " + hot);
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

            PLPMsg.M("\ti_data_imm_signExtended: " + String.format("%08x",i_data_imm_signExtended));
            PLPMsg.M("\ti_data_alu_in: " + String.format("%08x",i_data_alu_in));
            PLPMsg.M("\ti_data_rt: " + String.format("%08x",i_data_rt));
        }

        public void printinstr() {
            if(mem_reg.hot)
                PLPMsg.M("ex:   " + String.format("%08x", instrAddr) +
                         " instr: " + String.format("%08x", instruction) +
                         " : " + MIPSInstr.format(instruction));
            else
                PLPMsg.M("ex:   -");
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

            mem_reg.i_fwd_data_alu_result =
                exAlu.eval(data_alu_in,
                           ((ctl_aluSrc == 1) ? data_imm_signExtended : data_rt),
                           ctl_aluOp);

            mem_reg.hot = true;

            return PLPMsg.PLP_OK;

            } catch(Exception e) {
                return PLPMsg.E("I screwed up: " + e,
                                PLPMsg.PLP_SIM_EVALUATION_FAILED, this);
            }
        }

        private void clock() {
            instruction = i_instruction;
            instrAddr = i_instrAddr;
            
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
    }

    // Memory stage
    public class mem {
        boolean hot = false;
        long instruction;
        long instrAddr;

        // MEM stage pipeline registers
        long fwd_ctl_memtoreg;
        long fwd_ctl_regwrite;
        long fwd_ctl_dest_reg_addr;
        long fwd_ctl_linkaddr;
        long fwd_ctl_jal;
        long fwd_data_alu_result;

        long ctl_memwrite;
        long ctl_memread;

        long data_memwritedata;

        long ctl_regwrite;

        long i_instruction;
        long i_instrAddr;

        long i_fwd_ctl_memtoreg;
        long i_fwd_ctl_regwrite;
        long i_fwd_ctl_dest_reg_addr;
        long i_fwd_ctl_linkaddr;
        long i_fwd_ctl_jal;
        long i_fwd_data_alu_result;

        long i_ctl_memwrite;
        long i_ctl_memread;

        long i_data_memwritedata;

        long i_ctl_regwrite;

        private wb   wb_reg;
        private mod_bus bus;

        public mem(wb wb_reg, mod_bus bus) {
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
            PLPMsg.M("\thot: " + hot);
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
            if(wb_reg.hot)
                PLPMsg.M("mem:  " + String.format("%08x", instrAddr) +
                         " instr: " + String.format("%08x", instruction) +
                         " : " + MIPSInstr.format(instruction));
            else
                PLPMsg.M("mem:  -");
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

            if(ctl_memread == 1)
                wb_reg.i_data_memreaddata = bus.read(fwd_data_alu_result);

            if(ctl_memwrite == 1)
                bus.write(fwd_data_alu_result, data_memwritedata, false);

            wb_reg.hot = true;

            return PLPMsg.PLP_OK;

            } catch(Exception e) {
                return PLPMsg.E("I screwed up: " + e,
                                PLPMsg.PLP_SIM_EVALUATION_FAILED, this);
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
    }

    // Writeback stage
    public class wb {
        boolean hot = false;
        boolean instr_retired = false;
        long instruction;
        long instrAddr;

        // WB stage pipeline registers
        long ctl_memtoreg;
        long ctl_regwrite;
        long ctl_dest_reg_addr;
        long ctl_linkaddr;
        long ctl_jal;

        long data_memreaddata;
        long data_alu_result;

        long i_instruction;
        long i_instrAddr;
        
        long i_ctl_memtoreg;
        long i_ctl_regwrite;
        long i_ctl_dest_reg_addr;
        long i_ctl_linkaddr;
        long i_ctl_jal;

        long i_data_memreaddata;
        long i_data_alu_result;

        private mod_memory regfile;

        public wb(mod_memory regfile) {
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
            PLPMsg.M("\thot: " + hot);
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
            if(instr_retired)
                PLPMsg.M("wb:   " + String.format("%08x", instrAddr) +
                         " instr: " + String.format("%08x", instruction) +
                         " : " + MIPSInstr.format(instruction));
            else
                PLPMsg.M("wb:   -");
        }
    
        private int eval() {
            try {

            long internal_2x1 = (ctl_jal == 0) ?
                                 data_alu_result : ctl_linkaddr;

            if(ctl_regwrite == 1 && ctl_dest_reg_addr != 0)
                regfile.write(ctl_dest_reg_addr,
                    (ctl_memtoreg == 0) ? internal_2x1 : data_memreaddata, false);

            instr_retired = true;

            return PLPMsg.PLP_OK;

            } catch(Exception e) {
                return PLPMsg.E("I screwed up: " + e,
                                PLPMsg.PLP_SIM_EVALUATION_FAILED, this);
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
    }

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
                        case 0x2B: return a - b;
                        case 0x00: return a << b;
                        case 0x02: return a >> b;
                    }

                case 0x04: return (a - b == 0) ? 1 : 0;
                case 0x05: return (a - b == 0) ? 0 : 1;
                case 0x0c: return a & b;
                case 0x0d: return a | b;
                case 0x0f: return b << 16;
                case 0x0A:
                case 0x0B: return a - b;
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

    public class mod_forwarding {
        public mod_forwarding() {

        }

        public int eval(PLPMIPSSim.id id_stage, PLPMIPSSim.ex ex_stage,
                         PLPMIPSSim.mem mem_stage, PLPMIPSSim.wb wb_stage) {
            sim_flags &= PLPMsg.PLP_SIM_FWD_NO_EVENTS;

            if(mem_stage.hot) {
                if(MIPSInstr.rd(ex_stage.instruction) == MIPSInstr.rs(id_stage.instruction)) {
                    ex_stage.i_data_alu_in = mem_stage.i_fwd_data_alu_result;
                    sim_flags |= PLPMsg.PLP_SIM_FWD_EX_EX_RTYPE;
                }
                else if(MIPSInstr.rd(ex_stage.instruction) == MIPSInstr.rt(id_stage.instruction)) {
                    ex_stage.i_data_rt = mem_stage.i_fwd_data_alu_result;
                    sim_flags |= PLPMsg.PLP_SIM_FWD_EX_EX_RTYPE;
                }
                else if(MIPSInstr.rt(ex_stage.instruction) == MIPSInstr.rs(id_stage.instruction)) {
                    ex_stage.i_data_alu_in = mem_stage.i_fwd_data_alu_result;
                    sim_flags |= PLPMsg.PLP_SIM_FWD_EX_EX_ITYPE;
                }
            }

            return PLPMsg.PLP_OK;
        }
    }

    public class mod_bus {
        private PLPSimBusModule[] bus_modules;

        public mod_bus(PLPSimBusModule[] bus_modules) {
            this.bus_modules = bus_modules;
        }

        public long read(long addr) {
            for(int i = bus_modules.length - 1; i >= 0; i--) {
                if(addr >= bus_modules[i].startAddr() &&
                   addr <= bus_modules[i].endAddr())
                    return bus_modules[i].read(addr);
            }

            return -1;
        }

        public int write(long addr, long data, boolean isInstr) {
            boolean noMapping = true;

            for(int i = bus_modules.length - 1; i >= 0; i--) {
                if(addr >= bus_modules[i].startAddr() &&
                   addr <= bus_modules[i].endAddr()) {
                    bus_modules[i].write(addr, data, isInstr);
                    noMapping = false;
                }
            }

            if(noMapping)
                return PLPMsg.PLP_SIM_UNINITIALIZED_MEMORY;
            else
                return PLPMsg.PLP_OK;
        }
    }

    public class mod_register {
        private long data;
        private long i_data;

        public mod_register(long i_data) {
            this.i_data = i_data;
        }

        public int write(long data) {
            this.i_data = data;

            return PLPMsg.PLP_OK;
        }

        public long input() {
            return i_data;
        }

        public long eval() {
            return data;
        }

        public void clock() {
            data = i_data;
        }

        public void reset(long i_data) {
            data = -1;
            this.i_data = i_data;
        }
    }

    public class mod_memory extends PLPSimBusModule {

        public mod_memory(long size, boolean wordAligned) {
            super();
            super.startAddr = 0;
            super.endAddr = size;
            super.wordAligned = wordAligned;
        }

        // memory module doesn't need eval, just return OK.
        public int eval () {
            return PLPMsg.PLP_OK;
        }

        public String introduce() {
            return "PLPTool 2.0 mod_memory " + PLPMsg.versionString;
        }

        public void printAll(long highlight) {
            long key;
            PLPMsg.M("->\taddress\t\tcontents\tASCII");
            PLPMsg.M("--\t-------\t\t--------\t-----");
            Iterator keyIterator = super.values.keySet().iterator();
            while(keyIterator.hasNext()) {
                key = (Long) keyIterator.next();
                if(wordAligned) {
                    if(key * 4 == highlight)
                        System.out.print(">>>");
                    PLPMsg.M(String.format("\t%08x\t%08x\t",
                                           key * 4, super.values.get(key)) +
                                           PLPAsmFormatter.asciiWord(super.values.get(key)));
                }
                else {
                    if(key == highlight)
                        System.out.print(">>>");
                    PLPMsg.M(String.format("\t%08x\t%08x\t",
                                            key, super.values.get(key)) +
                                            PLPAsmFormatter.asciiWord(super.values.get(key)));
                }
            }
        }

        public void printProgram(long highlight) {
            if(wordAligned) {
                long key;
                PLPMsg.M("pc\taddress\t\thex\t\tDisassembly");
                PLPMsg.M("--\t-------\t\t---\t\t-----------");
                Iterator keyIterator = super.values.keySet().iterator();
                while(keyIterator.hasNext()) {
                    key = (Long) keyIterator.next();
                    if(super.isInstr.get(key) == true) {
                        if(key * 4 == highlight)
                            System.out.print(">>>");
                        PLPMsg.M(String.format("\t%08x\t%08x\t",
                                               key * 4, super.values.get(key)) +
                                               MIPSInstr.format(super.values.get(key)));
                    }
                }
            }
        }

        public void print(long addr) {
            if(wordAligned) {
                addr /= 4;

                PLPMsg.M("\naddress\t\tcontents\tASCII");
                PLPMsg.M("-------\t\t--------\t-----");
                PLPMsg.M(String.format("%08x\t%08x\t" +
                                       PLPAsmFormatter.asciiWord(super.read(addr)),
                                       addr * 4, super.read(addr)));
            }
            else {
                PLPMsg.M("\naddress\t\tcontents\tASCII");
                PLPMsg.M("-------\t\t--------\t-----");
                PLPMsg.M(String.format("%08x\t%08x\t" +
                                       PLPAsmFormatter.asciiWord(super.read(addr)),
                                       addr, super.read(addr)));
            }
        }

        @Override public String toString() {
            return "PLPMIPSSim.mod_memory";
        }
    }

    // This module says Hello World when its register is written with 0xBEEF
    public class io_example extends PLPSimBusModule {
        public io_example(long addr) {
            super();
            // This I/O only has 1 register
            super.startAddr = addr;
            super.endAddr = addr;
            super.wordAligned = true;
        }

        // When this module is evaluated, do this:
        public int eval() {
            if(!enabled)
                return PLPMsg.PLP_OK;

            // Get register value
            long value = super.read(startAddr);

            // Combinational logic
            if(value == 0xBEEF)
                PLPMsg.M(this + ": Hey, it's beef!");

            return PLPMsg.PLP_OK;
        }

        public String introduce() {
            return "PLPTool 2.0 io_example: an example of PLP bus module, try writing 0xbeef to " +
                    String.format("0x%08x", startAddr) + "!";
        }

        // Make sure to do this for error tracking
        @Override public String toString() {
            return "PLPMIPSSim.io_example";
        }
    }

    public class io_7segs extends PLPSimBusModule {
        public io_7segs() {
            
        }

        public int eval() {
            return PLPMsg.PLP_OK;
        }

        public String introduce() {
            return null;
        }
    }

    public class io_vga extends PLPSimBusModule {
        public io_vga() {

        }

        public int eval() {
            return PLPMsg.PLP_OK;
        }

        public String introduce() {
            return null;
        }
    }

    public class io_uart extends PLPSimBusModule {
        public io_uart() {

        }

        public int eval() {
            return PLPMsg.PLP_OK;
        }

        public String introduce() {
            return null;
        }
    }

    public class io_leds extends PLPSimBusModule {
        public io_leds(long addr) {
            super();
            // This I/O only has 1 register
            super.startAddr = addr;
            super.endAddr = addr;
            super.wordAligned = true;
        }

        // When this module is evaluated, do this:
        public int eval() {
            if(!enabled)
                return PLPMsg.PLP_OK;

            // Get register value
            long value = super.read(startAddr);

            System.out.print(this + ": ");

            // Combinational logic
            for(int i = 7; i >= 0; i--) {
                if((value & (long) Math.pow(2, i)) == (long) Math.pow(2, i))
                    System.out.print("* ");
                else
                    System.out.print(". ");
            }

            System.out.println();

            return PLPMsg.PLP_OK;
        }

        public String introduce() {
            return "PLPTool 2.0 io_leds: 8 LED arrays attached to " +
                   String.format("0x%08x", startAddr);
        }

        // Make sure to do this for error tracking
        @Override public String toString() {
            return "PLPMIPSSim.io_leds";
        }
    }

    public class io_switches extends PLPSimBusModule {
        public io_switches() {

        }

        public int eval() {
            return PLPMsg.PLP_OK;
        }

        public String introduce() {
            return null;
        }
    }
}

