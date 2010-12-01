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

/**
 * The PLP MIPS Architecture Simulator Backend
 *
 * @author wira
 */
public class PLPMIPSSim {

    // external modules
    public mod_memory       memory;
    public mod_forwarding   forwarding;

    // statistics
    private int instructionCount;

    // pipeline stages
    public id   id_stage;
    public ex   ex_stage;
    public mem  mem_stage;
    public wb   wb_stage;

    // flags
    public long sim_flags;

    // assembler objects
    long[] objCode;
    long[] addrTable;

    PLPAsm asm;

    // Initialize core
    public PLPMIPSSim(PLPAsm asm, int RAMsize) {
        memory = new mod_memory(new long[RAMsize],
                                new long[32],
                                new boolean[RAMsize],
                                0);                // pc=0 on reset

        this.asm = asm;
        this.objCode = asm.getObjectCode();
        this.addrTable = asm.getAddrTable();

        sim_flags = 0;

        // Instantiate stages
        wb_stage = new wb(memory);
        mem_stage = new mem(wb_stage, memory);
        ex_stage = new ex(mem_stage, new alu());
        id_stage = new id(ex_stage, memory);

        forwarding = new mod_forwarding();
    }

    public int reset() {
        int i;

        // init RAM
        for(i = 0; i < memory.main.length; i++)
            memory.main[i] = -1;

        // init regfile
        for(i = 0; i < 32; i++)
            memory.regfile[i] = 0;

        // load program to RAM
        for(i = 0; i < objCode.length; i++) {
            if((int) (addrTable[i] / 4) >= memory.main.length)
                PLPMsg.M("Warning: Program doesn't fit in memory.");
            else {
                if (asm.isInstruction(i) == 0)
                    memory.isInstr[(int) addrTable[i] / 4] = true;
                else
                    memory.isInstr[(int) addrTable[i] / 4] = false;

                memory.main[(int) addrTable[i] / 4] = objCode[i];
            }
        }

        memory.pc = -1;
        memory.i_pc = 0;
        instructionCount = 0;
        sim_flags = 0;
        flushpipeline();

        PLPMsg.M("core: reset");

        return PLPMsg.PLP_OK;
    }

    public int softreset() {
        memory.pc = -1;
        memory.i_pc = 0;
        flushpipeline();

        PLPMsg.M("core: soft reset");

        return PLPMsg.PLP_OK;
    }

    public int step () {
        instructionCount++;
        int ret = 0;

        // Propagate values
        if(id_stage.hot)  id_stage.clock();
        if(ex_stage.hot)  ex_stage.clock();
        if(mem_stage.hot) mem_stage.clock();
        if(wb_stage.hot)  wb_stage.clock();

        memory.clock_pc(); // clock PC to get new instruction

        // Evaluate stages
        if(wb_stage.hot)  ret += wb_stage.eval();
        if(mem_stage.hot) ret += mem_stage.eval();
        if(ex_stage.hot)  ret += ex_stage.eval();
        if(id_stage.hot)  ret += id_stage.eval();

        // Engage forwarding unit
        if(PLPCfg.cfgSimForwardingUnit)
            forwarding.eval(id_stage, ex_stage, mem_stage, wb_stage);


        if(ret != 0) {
            return PLPMsg.E("Evaluation failed.",
                            PLPMsg.PLP_SIM_EVALUATION_FAILED, this);
        }

        // update i_pc for next instruction
        if(id_stage.hot && id_stage.ctl_pcsrc == 1)
            memory.i_pc = id_stage.ctl_branchtarget;
        else if(id_stage.hot && id_stage.ctl_jump == 1)
            memory.i_pc = id_stage.ctl_jumptarget;
        else
            memory.i_pc += 4;

        return fetch();
    }

    // IF stage
    private int fetch()  {

        if(memory.pc / 4 >= memory.main.length)
            return PLPMsg.E("step(): Instruction memory out-of-bounds: addr=" +
                            String.format("%08x", memory.pc),
                            PLPMsg.PLP_SIM_INSTRMEM_OUT_OF_BOUNDS, this);

        if(memory.main[(int) (memory.pc / 4)] == -1)
            return PLPMsg.E("step(): Memory location uninitialized: addr=" +
                            String.format("%08x", memory.pc),
                            PLPMsg.PLP_SIM_UNINITIALIZED_MEMORY, this);

        if(!memory.isInstr[(int) (memory.pc / 4)])
            return PLPMsg.E("step(): Unprogrammed memory: addr=" +
                            String.format("%08x", memory.pc),
                            PLPMsg.PLP_SIM_INSTRMEM_OUT_OF_BOUNDS, this);

        // fetch instruction / frontend stage
        id_stage.i_instruction = memory.main[(int) (memory.pc / 4)];
        id_stage.i_instrAddr = memory.pc;
        id_stage.i_ctl_pcplus4 = memory.pc + 4;

        id_stage.hot = true;

        return PLPMsg.PLP_OK;
    }

    public int flushpipeline() {
        id_stage.hot = false;
        ex_stage.hot = false;
        mem_stage.hot = false;
        wb_stage.hot = false;
        wb_stage.instr_retired = false;

        return PLPMsg.PLP_OK;
    }

    public void printfrontend() {

        if(memory.pc < 0)
            PLPMsg.M("if:   -" +
                     "\ni_pc: " + String.format("%08x", memory.i_pc));
        else
            PLPMsg.M("if:   " + String.format("%08x", memory.pc) +
                " instr: " + String.format("%08x", id_stage.i_instruction) +
                " : " + MIPSInstr.format(id_stage.i_instruction) +
                "\ni_pc: "  + String.format("%08x", memory.i_pc));
    }

    public int run (int instructions) {

        return PLPMsg.PLP_OK;
    }

    public void pause() {

    }

    public int printprogram() {
        int i, j;
        PLPMsg.M("pc\taddress\t\thex\t\tinstruction");
        PLPMsg.M("--\t-------\t\t---\t\t-----------");
        for(i = 0; i < memory.main.length; i++) {
            for(j = 0; j < addrTable.length; j++) {
                if(i * 4 == addrTable[j] && asm.isInstruction(j) == 0) {
                    if(memory.pc == i * 4)
                        System.out.print(">>>\t");
                    else
                        System.out.print("\t");
                    PLPMsg.M(String.format("%08x", addrTable[j]) + "\t" +
                             String.format("%08x", objCode[j]) + "\t" +
                             MIPSInstr.format(objCode[j]));
                    break;
                }
            }
        }

        return PLPMsg.PLP_OK;
    }

    public int getinstrcount() {
        return instructionCount;
    }

    @Override public String toString() {
        return "PLPMIPSSim(asm: " + asm.toString() + ")";
    }

    public class mod_memory {
        long            pc;
        long            i_pc;
        long[]          main;
        long[]          regfile;
        boolean[]       isInstr;

        public mod_memory(long[] main, long[] regfile, boolean[] isInstr, long init_pc) {
            this.i_pc = init_pc;
            this.pc = -1;
            this.main = main;
            this.regfile = regfile;
            this.isInstr = isInstr;
        }

        public void printMain() {
            PLPMsg.M("pc\taddress\t\tcontents\tASCII");
            PLPMsg.M("--\t-------\t\t--------\t-----");
            for(int i = 0; i < main.length; i++) {
                if(main[i] != -1) {
                    if(i * 4 == pc)
                        System.out.print(">>>");
                    PLPMsg.M(String.format("\t%08x\t%08x\t" +
                                           PLPAsmFormatter.asciiWord(main[i]),
                                           i * 4, main[i]));
                }
            }
        }

        public void printProgram() {
            PLPMsg.M("pc\taddress\t\thex\t\tDisassembly");
            PLPMsg.M("--\t-------\t\t---\t\t-----------");
            for(int i = 0; i < main.length; i++) {
                if(main[i] != -1 && isInstr[i]) {
                    if(i * 4 == pc)
                        System.out.print(">>>");
                    PLPMsg.M(String.format("\t%08x\t%08x\t" +
                                           MIPSInstr.format(main[i]),
                                           i * 4, main[i]));
                }
            }
        }

        public void printMain(int addr) {

            if((addr / 4) >= main.length || addr < 0)
                PLPMsg.E("printMain(int): Address out of range.",
                        PLPMsg.PLP_SIM_OUT_ADDRESS_OUT_OF_RANGE, this);
            else if(main[addr / 4] == -1)
                PLPMsg.E("printMain(int): Address not initialized.",
                        PLPMsg.PLP_SIM_UNINITIALIZED_MEMORY, this);
            else if(addr % 4 != 0)
                PLPMsg.E("printMain(int): Unaligned memory.",
                         PLPMsg.PLP_SIM_OUT_UNALIGNED_MEMORY, this);
            else {
                addr /= 4;

                PLPMsg.M("\naddress\t\tcontents\tASCII");
                PLPMsg.M("-------\t\t--------\t-----");
                PLPMsg.M(String.format("%08x\t%08x\t" +
                                       PLPAsmFormatter.asciiWord(main[addr]),
                                       addr * 4, main[addr]));
            }
        }

        public void printRegFile() {
            for(int i = 0; i < 32; i++)
                PLPMsg.M(i + "\t" + String.format("%08x", regfile[i]));
        }

        public void clock_pc() {
            pc = i_pc;
        }

        public int writeMain(int addr, long data) {
            if((addr / 4) >= main.length || addr < 0)
                return PLPMsg.E("printMain(int): Address out of range.",
                                PLPMsg.PLP_SIM_OUT_ADDRESS_OUT_OF_RANGE, this);
            else if(addr % 4 != 0)
                return PLPMsg.E("printMain(int): Unaligned memory.",
                                PLPMsg.PLP_SIM_OUT_UNALIGNED_MEMORY, this);
            else {
                main[addr / 4] = data;
            }

            return PLPMsg.PLP_OK;
        }

        @Override public String toString() {
            return "PLPMIPSSim.mod_memory";
        }
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
        private mod_memory memory;

        public id(ex ex_reg, mod_memory memory) {
            this.ex_reg = ex_reg;
            this.memory = memory;
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
            ex_reg.i_instruction = instruction;
            ex_reg.i_instrAddr = instrAddr;

            byte opcode      = (byte) MIPSInstr.opcode(instruction);
            byte funct       = (byte) MIPSInstr.funct(instruction);

            long addr_read_0 = MIPSInstr.rt(instruction); // rt
            long addr_read_1 = MIPSInstr.rs(instruction); // rs

            // rt
            ex_reg.i_data_rt     = (addr_read_0 == 0) ?
                                   0 : memory.regfile[(int) addr_read_0];

            // rs
            ex_reg.i_data_alu_in = (addr_read_1 == 0) ?
                                   0 : memory.regfile[(int) addr_read_1];

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
        private mod_memory memory;

        public mem(wb wb_reg, mod_memory memory) {
            this.wb_reg = wb_reg;
            this.memory = memory;
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
            wb_reg.i_instruction = instruction;
            wb_reg.i_instrAddr = instrAddr;

            wb_reg.i_ctl_memtoreg = fwd_ctl_memtoreg;
            wb_reg.i_ctl_regwrite = fwd_ctl_regwrite;
            wb_reg.i_ctl_dest_reg_addr = fwd_ctl_dest_reg_addr;
            wb_reg.i_ctl_jal = fwd_ctl_jal;
            wb_reg.i_ctl_linkaddr = fwd_ctl_linkaddr;
        
            wb_reg.i_data_alu_result = fwd_data_alu_result;

            if(ctl_memread == 1)
                wb_reg.i_data_memreaddata = memory.main[(int) fwd_data_alu_result / 4];

            if(ctl_memwrite == 1)
                memory.main[(int) fwd_data_alu_result / 4] = data_memwritedata;

            wb_reg.hot = true;

            return PLPMsg.PLP_OK;
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

        private mod_memory memory;

        public wb(mod_memory memory) {
            this.memory = memory;
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
            long internal_2x1 = (ctl_jal == 0) ?
                                 data_alu_result : ctl_linkaddr;

            if(ctl_regwrite == 1 && ctl_dest_reg_addr != 0)
                memory.regfile[(int) ctl_dest_reg_addr] =
                    (ctl_memtoreg == 0) ? internal_2x1 : data_memreaddata;

            instr_retired = true;

            return PLPMsg.PLP_OK;
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
                        case 0x24:
                            return a & b;
                        case 0x25:
                            return a | b;
                        case 0x27:
                            return ~(a | b);
                        case 0x21:
                            return a + b;
                        case 0x23:
                            return a - b;
                    }

                case 0x04:
                    return (a - b == 0) ? 1 : 0;

                case 0x0d:
                    return a | b;

                case 0x0f:
                    return b << 16;

                case 0x23:
                case 0x2B:
                    return a + b;
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

        private int eval(id id_stage, ex ex_stage, mem mem_stage, wb wb_stage) {
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

    }

    public class io_7segs {

    }

    public class io_vga {

    }

    public class io_uart {

    }

    public class io_leds {

    }

    public class io_switches {
        
    }
}

