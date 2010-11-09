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
 * The PLP MIPS Architecture Emulator Backend
 *
 * @author wira
 */
public class PLPMIPSSim {

    public memory coreMem;

    public int instructionCount;

    public rf   rf_stage;
    public ex   ex_stage;
    public mem  mem_stage;
    public wb   wb_stage;

    long[] objCode;
    long[] addrTable;

    PLPAsm asm;

    // Initialize core
    public PLPMIPSSim(PLPAsm asm, int RAMsize) {
        int i;
        coreMem = new memory(new long[RAMsize],
                             new long[32],
                             new boolean[RAMsize],
                             0);                // pc=0 on reset

        this.asm = asm;
        this.objCode = asm.getObjectCode();
        this.addrTable = asm.getAddrTable();

        // init RAM
        for(i = 0; i < RAMsize; i++)
            coreMem.mainMem[i] = -1;

        // init regFile
        for(i = 0; i < 32; i++)
            coreMem.regFile[i] = 0;

        // load program to RAM
        for(i = 0; i < objCode.length; i++) {
            if((int) (addrTable[i] / 4) >= RAMsize)
                System.out.println("Warning: Program doesn't fit in memory.");
            else {
                if (asm.isInstruction(i) == 0)
                    coreMem.isInstr[(int) addrTable[i] / 4] = true;
                else
                    coreMem.isInstr[(int) addrTable[i] / 4] = false;
                
                coreMem.mainMem[(int) addrTable[i] / 4] = objCode[i];
            }
        }

        // Instantiate stages
        wb_stage = new wb(coreMem);
        mem_stage = new mem(wb_stage, coreMem);
        ex_stage = new ex(mem_stage, new alu());
        rf_stage = new rf(ex_stage, coreMem);
    }

    public int reset() {
        coreMem.pc = 0;
        instructionCount = 0;

        flushpipeline();
        fetch();

        System.out.println("core: reset");

        return PLPMsg.PLP_OK;
    }

    public int step () {
        instructionCount++;

        rf_stage.clock();
        if(ex_stage.hot) ex_stage.clock();
        if(mem_stage.hot) mem_stage.clock();
        if(wb_stage.hot) wb_stage.clock();

        if(wb_stage.hot) wb_stage.eval();
        if(mem_stage.hot) mem_stage.eval();
        if(ex_stage.hot) ex_stage.eval();
        rf_stage.eval();

        // update pc for next instruction
        if(ex_stage.hot && ex_stage.ctl_pcsrc == 1)
            coreMem.pc = ex_stage.ctl_branchTarget;
        else if(ex_stage.hot && ex_stage.ctl_jump == 1)
            coreMem.pc = ex_stage.ctl_jumpTarget;
        else
            coreMem.pc += 4;

        return fetch();
    }

    public int fetch()  {

        if(coreMem.pc / 4 >= coreMem.mainMem.length)
            return PLPMsg.E("step(): Instruction memory out-of-bounds: addr=" +
                            String.format("%08x", coreMem.pc),
                            PLPMsg.PLP_SIM_INSTRMEM_OUT_OF_BOUNDS, this);

        if(coreMem.mainMem[(int) (coreMem.pc / 4)] == -1)
            return PLPMsg.E("step(): Memory location uninitialized: addr=" +
                            String.format("%08x", coreMem.pc),
                            PLPMsg.PLP_SIM_UNINITIALIZED_MEMORY, this);

        if(!coreMem.isInstr[(int) (coreMem.pc / 4)])
            return PLPMsg.E("step(): Unprogrammed memory: addr=" +
                            String.format("%08x", coreMem.pc),
                            PLPMsg.PLP_SIM_INSTRMEM_OUT_OF_BOUNDS, this);

        // fetch instruction / frontend stage
        rf_stage.i_instruction = coreMem.mainMem[(int) (coreMem.pc / 4)];
        rf_stage.i_instrAddr = coreMem.pc;
        rf_stage.i_fwd_ctl_pcplus4 = coreMem.pc + 4;

        rf_stage.hot = true;

        return PLPMsg.PLP_OK;
    }

    public int flushpipeline() {
        rf_stage.hot = false;
        ex_stage.hot = false;
        mem_stage.hot = false;
        wb_stage.hot = false;

        return PLPMsg.PLP_OK;
    }

    public void printfrontend() {
        System.out.println("pc: " + String.format("%08x", coreMem.pc) +
                " instr: " + String.format("%08x", rf_stage.i_instruction) +
                " " + MIPSInstr.format(rf_stage.i_instruction));
    }

    public int run (int instructions) {

        return PLPMsg.PLP_OK;
    }

    public void pause() {

    }

    public int printprogram() {
        int i, j;
        System.out.println("pc\taddress\t\tinstruction");
        System.out.println("--\t-------\t\t-----------");
        for(i = 0; i < coreMem.mainMem.length; i++) {
            for(j = 0; j < addrTable.length; j++) {
                if(i * 4 == addrTable[j] && asm.isInstruction(j) == 0) {
                    if(coreMem.pc == i * 4)
                        System.out.print(">>>\t");
                    else
                        System.out.print("\t");
                    System.out.println(String.format("%08x", addrTable[j]) + "\t" +
                                       MIPSInstr.format(objCode[j]));
                    break;
                }
            }
        }

        return PLPMsg.PLP_OK;
    }

    class consts {
        final static  int   R_MASK = 0x1F;
        final static  int   V_MASK = 0x3F;
        final static  int   C_MASK = 0xFFFF;
        final static  int   J_MASK = 0x3FFFFFF;
    }

    public class memory {
        long                    pc;
        long[]                  mainMem;
        long[]                  regFile;
        boolean[]               isInstr;

        public memory(long[] mainMem, long[] regFile, boolean[] isInstr, long pc) {
            this.pc = pc;
            this.mainMem = mainMem;
            this.regFile = regFile;
            this.isInstr = isInstr;
        }

        public void printMainMem() {
            System.out.println("pc\taddress\t\tcontents");
            System.out.println("--\t-------\t\t--------");
            for(int i = 0; i < mainMem.length; i++) {
                if(mainMem[i] != -1) {
                    if(i * 4 == pc)
                        System.out.print(">>>");
                    System.out.println(String.format("\t%08x\t%08x", i * 4, mainMem[i]));
                }
            }
        }

        public void printRegFile() {
            for(int i = 0; i < 32; i++)
                System.out.println(i + "\t" + String.format("%08x", regFile[i]));
        }
    }

    // Register file stage / control decode
    public class rf {
        boolean hot = false;
        long instruction;
        long instrAddr;

        // RF stage pipeline registers
        long i_instruction;
        long i_instrAddr;
        long i_fwd_ctl_pcplus4;

        long fwd_ctl_pcplus4;
        
        private ex   ex_reg;
        private memory coreMem;

        public rf(ex ex_reg, memory coreMem) {
            this.ex_reg = ex_reg;
            this.coreMem = coreMem;
        }

        public void printvars() {
            System.out.println("RF vars");
            System.out.println("\tinstruction: " + String.format("%08x", instruction));
            System.out.println("\tinstrAddr: " + String.format("%08x", instrAddr));
            System.out.println("\tfwd_ctl_pcplus4: " + fwd_ctl_pcplus4);
        }

        public void printnextvars() {
            System.out.println("RF next vars");
            System.out.println("\ti_instruction: " + String.format("%08x", i_instruction));
            System.out.println("\ti_instrAddr: " + String.format("%08x", i_instrAddr));
            System.out.println("\ti_fwd_ctl_pcplus4: " + i_fwd_ctl_pcplus4);
        }

        private void eval() {
            System.out.println("rf eval: " + String.format("%08x", instrAddr));

            ex_reg.i_instruction = instruction;
            ex_reg.i_instrAddr = instrAddr;

            ex_reg.i_ctl_pcplus4 = fwd_ctl_pcplus4;

            byte opcode      = (byte) ((instruction >> 26) & consts.V_MASK);
            long addr_read_0 = (instruction >> 16) & consts.R_MASK;
            long addr_read_1 = (instruction >> 21) & consts.R_MASK;

            ex_reg.i_data_rt     = (addr_read_0 == 0) ? 0 : coreMem.regFile[(int) addr_read_0];
            ex_reg.i_data_alu_in = (addr_read_1 == 0) ? 0 : coreMem.regFile[(int) addr_read_1];

            // careful here, we actually need the sign
            // sign extension, java style, pffft
            long imm_field = instruction & consts.C_MASK;
            ex_reg.i_data_imm_signExtended = imm_field;

            ex_reg.i_ctl_rd_addr = (instruction >> 11) & consts.R_MASK;
            ex_reg.i_ctl_rt_addr = addr_read_0;

            ex_reg.i_ctl_aluOp = instruction;

            // control logic
            ex_reg.i_fwd_ctl_memtoreg = 0;
            ex_reg.i_fwd_ctl_regwrite = 0;
            ex_reg.i_fwd_ctl_memwrite = 0;
            ex_reg.i_fwd_ctl_memread = 0;
            ex_reg.i_ctl_branch = 0;
            ex_reg.i_ctl_aluSrc = 0;
            ex_reg.i_ctl_regDst = 0;
            ex_reg.i_ctl_jump = 0;

            switch(PLPAsm.lookupInstrType(PLPAsm.lookupInstr(opcode))) {
                case 0:
                case 1:
                case 2:
                    ex_reg.i_fwd_ctl_regwrite = 1;
                    ex_reg.i_ctl_regDst = 1;

                    break;

                case 3:
                    ex_reg.i_ctl_branch = 1;

                    break;

                case 4:
                case 5:
                    ex_reg.i_fwd_ctl_regwrite = 1;
                    ex_reg.i_ctl_aluSrc = 1;

                    break;

                case 6:
                    if(PLPAsm.lookupInstr(opcode).equals("lw")) {
                        ex_reg.i_fwd_ctl_memtoreg = 1;
                        ex_reg.i_fwd_ctl_regwrite = 1;
                        ex_reg.i_fwd_ctl_memread = 1;
                        ex_reg.i_ctl_aluSrc = 1;
                    } else if(PLPAsm.lookupInstr(opcode).equals("sw")) {
                        ex_reg.i_fwd_ctl_memwrite = 1;
                        ex_reg.i_ctl_aluSrc = 1;
                    }

                    break;
                case 7:
                    ex_reg.i_ctl_jump = 1;
                    if(PLPAsm.lookupInstr(opcode).equals("jal")) {
                        ex_reg.i_fwd_ctl_regwrite = 1;
                        ex_reg.i_ctl_regDst = 1;
                        ex_reg.i_ctl_rd_addr = 31;
                    }

                    break;
            }
            ex_reg.hot = true;
        }

        private void clock() {
            fwd_ctl_pcplus4 = i_fwd_ctl_pcplus4;
            instruction = i_instruction;
            instrAddr = i_instrAddr;
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

        long ctl_branch;
        long ctl_jump;
        long fwd_ctl_memwrite;
        long fwd_ctl_memread;

        long ctl_aluSrc;
        long ctl_aluOp;
        long ctl_regDst;
        long ctl_pcsrc;
        long ctl_pcjump;

        long ctl_pcplus4;

        long data_alu_in;
        long data_rt;

        long ctl_branchTarget;
        long ctl_jumpTarget;

        long data_imm_signExtended;
        long ctl_rt_addr;
        long ctl_rd_addr;

        long i_instruction;
        long i_instrAddr;

        long i_fwd_ctl_memtoreg;
        long i_fwd_ctl_regwrite;

        long i_ctl_branch;
        long i_ctl_jump;
        long i_fwd_ctl_memwrite;
        long i_fwd_ctl_memread;

        long i_ctl_aluSrc;
        long i_ctl_aluOp;
        long i_ctl_regDst;

        long i_ctl_pcplus4;

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
            System.out.println("EX vars");
            System.out.println("\tinstruction: " + String.format("%08x",instruction));
            System.out.println("\tinstrAddr: " + String.format("%08x",instrAddr));

            // EX stage pipeline registers
            System.out.println("\tfwd_ctl_memtoreg: " + fwd_ctl_memtoreg);
            System.out.println("\tfwd_ctl_regwrite: " + fwd_ctl_regwrite);
            System.out.println("\tfwd_ctl_memwrite: " + fwd_ctl_memwrite);
            System.out.println("\tfwd_ctl_memread: " + fwd_ctl_memread);

            System.out.println("\tctl_aluSrc: " + ctl_aluSrc);
            System.out.println("\tctl_aluOp: " + String.format("%08x",ctl_aluOp));
            System.out.println("\tctl_regDst: " + ctl_regDst);
            System.out.println("\tctl_pcsrc: " + ctl_pcsrc);
            System.out.println("\tctl_jump: " + ctl_jump);
            System.out.println("\tctl_branch: " + ctl_branch);
            System.out.println("\tctl_branchTarget: " + String.format("%08x",ctl_branchTarget));
            System.out.println("\tctl_jumpTarget: " + String.format("%08x",ctl_jumpTarget));
            System.out.println("\tctl_pcplus4: " + ctl_pcplus4);

            System.out.println("\tdata_imm_signExtended: " + String.format("%08x",data_imm_signExtended));
            System.out.println("\tdata_alu_in: " + String.format("%08x",data_alu_in));
        }

        public void printnextvars() {
            System.out.println("EX next vars");
            System.out.println("\thot: " + hot);
            System.out.println("\ti_instruction: " + String.format("%08x",i_instruction));
            System.out.println("\ti_instrAddr: " + String.format("%08x",i_instrAddr));

            // EX stage pipeline registers
            System.out.println("\ti_fwd_ctl_memtoreg: " + i_fwd_ctl_memtoreg);
            System.out.println("\ti_fwd_ctl_regwrite: " + i_fwd_ctl_regwrite);
            System.out.println("\ti_fwd_ctl_memwrite: " + i_fwd_ctl_memwrite);
            System.out.println("\ti_fwd_ctl_memread: " + i_fwd_ctl_memread);

            System.out.println("\ti_ctl_aluSrc: " + i_ctl_aluSrc);
            System.out.println("\ti_ctl_aluOp: " + String.format("%08x",i_ctl_aluOp));
            System.out.println("\ti_ctl_regDst: " + i_ctl_regDst);
            System.out.println("\ti_ctl_jump: " + i_ctl_jump);
            System.out.println("\ti_ctl_branch: " + i_ctl_branch);
            System.out.println("\ti_ctl_pcplus4: " + i_ctl_pcplus4);

            System.out.println("\ti_data_imm_signExtended: " + String.format("%08x",i_data_imm_signExtended));
            System.out.println("\ti_data_alu_in: " + String.format("%08x",i_data_alu_in));
        }

        private void eval() {
            System.out.println("ex eval: " + String.format("%08x", instrAddr));

            mem_reg.i_instruction = instruction;
            mem_reg.i_instrAddr = instrAddr;

            mem_reg.i_fwd_ctl_memtoreg = fwd_ctl_memtoreg;
            mem_reg.i_fwd_ctl_regwrite = fwd_ctl_regwrite;
            mem_reg.i_fwd_ctl_dest_reg_addr = (ctl_regDst == 1) ? ctl_rd_addr : ctl_rt_addr;

            mem_reg.i_ctl_memwrite = fwd_ctl_memwrite;
            mem_reg.i_ctl_memread = fwd_ctl_memread;

            mem_reg.i_data_memwritedata = data_rt;

            mem_reg.i_fwd_data_alu_result =
                exAlu.eval(data_alu_in,
                           ((ctl_aluSrc == 1) ? data_imm_signExtended : data_rt),
                           ctl_aluOp);

            ctl_pcsrc = (mem_reg.i_fwd_data_alu_result == 0) ? 0 : 1;
            ctl_pcsrc &= ctl_branch;
            ctl_branchTarget = ctl_pcplus4 + (data_imm_signExtended << 2);
            ctl_jumpTarget = MIPSInstr.jaddr(instruction) << 2;
           
            mem_reg.hot = true;
        }

        private void clock() {
            instruction = i_instruction;
            instrAddr = i_instrAddr;
            
            fwd_ctl_memtoreg = i_fwd_ctl_memtoreg;
            fwd_ctl_regwrite = i_fwd_ctl_regwrite;

            fwd_ctl_memwrite = i_fwd_ctl_memwrite;
            fwd_ctl_memread = i_fwd_ctl_memread;

            ctl_jump = i_ctl_jump;
            ctl_branch = i_ctl_branch;
            ctl_aluSrc = i_ctl_aluSrc;
            ctl_aluOp = i_ctl_aluOp;
            ctl_regDst = i_ctl_regDst;

            ctl_pcplus4 = i_ctl_pcplus4;

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
        long i_fwd_data_alu_result;

        long i_ctl_memwrite;
        long i_ctl_memread;

        long i_data_memwritedata;

        long i_ctl_regwrite;

        private wb   wb_reg;
        private memory coreMem;

        public mem(wb wb_reg, memory coreMem) {
            this.wb_reg = wb_reg;
            this.coreMem = coreMem;
        }

        public void printvars() {
            System.out.println("MEM vars");
            System.out.println("\tinstruction: " + String.format("%08x",instruction));
            System.out.println("\tinstrAddr: " + String.format("%08x",instrAddr));

            // MEM stage pipeline registers
            System.out.println("\tfwd_ctl_memtoreg: " + fwd_ctl_memtoreg);
            System.out.println("\tfwd_ctl_regwrite: " + fwd_ctl_regwrite);
            System.out.println("\tfwd_ctl_dest_reg_addr: " + fwd_ctl_dest_reg_addr);
            System.out.println("\tfwd_data_alu_result: " + String.format("%08x",fwd_data_alu_result));

            System.out.println("\tctl_regwrite: " + ctl_regwrite);
            System.out.println("\tctl_memwrite: " + ctl_memwrite);
            System.out.println("\tctl_memread: " + ctl_memread);

            System.out.println("\tdata_memwritedata: " + String.format("%08x",data_memwritedata));
        }

        public void printnextvars() {
            System.out.println("MEM next vars");
            System.out.println("\thot: " + hot);
            System.out.println("\ti_instruction: " + String.format("%08x",i_instruction));
            System.out.println("\ti_instrAddr: " + String.format("%08x",i_instrAddr));

            // MEM stage pipeline registers
            System.out.println("\ti_fwd_ctl_memtoreg: " + i_fwd_ctl_memtoreg);
            System.out.println("\ti_fwd_ctl_regwrite: " + i_fwd_ctl_regwrite);
            System.out.println("\ti_fwd_ctl_dest_reg_addr: " + i_fwd_ctl_dest_reg_addr);
            System.out.println("\ti_fwd_data_alu_result: " + String.format("%08x",i_fwd_data_alu_result));

            System.out.println("\ti_ctl_regwrite: " + i_ctl_regwrite);
            System.out.println("\ti_ctl_memwrite: " + i_ctl_memwrite);
            System.out.println("\ti_ctl_memread: " + i_ctl_memread);

            System.out.println("\ti_data_memwritedata: " + String.format("%08x",i_data_memwritedata));
        }

        private void eval() {
            System.out.println("mem eval: " + String.format("%08x", instrAddr));
            wb_reg.i_instruction = instruction;
            wb_reg.i_instrAddr = instrAddr;

            wb_reg.i_ctl_memtoreg = fwd_ctl_memtoreg;
            wb_reg.i_ctl_regwrite = fwd_ctl_regwrite;
            wb_reg.i_ctl_dest_reg_addr = fwd_ctl_dest_reg_addr;
        
            wb_reg.i_data_alu_result = fwd_data_alu_result;

            if(ctl_memread == 1)
                wb_reg.i_data_memreaddata = coreMem.mainMem[(int) fwd_data_alu_result];

            if(ctl_memwrite == 1)
                coreMem.mainMem[(int) fwd_data_alu_result] = data_memwritedata;

            wb_reg.hot = true;
        }

        private void clock() {
            instruction = i_instruction;
            instrAddr = i_instrAddr;
            
            fwd_ctl_memtoreg = i_fwd_ctl_memtoreg;
            fwd_ctl_regwrite = i_fwd_ctl_regwrite;
            fwd_ctl_dest_reg_addr = i_fwd_ctl_dest_reg_addr;
            fwd_data_alu_result = i_fwd_data_alu_result;
            ctl_memwrite = i_ctl_memwrite;
            ctl_memread = i_ctl_memread;
            data_memwritedata = i_data_memwritedata;

            ctl_regwrite = i_ctl_regwrite;
        }
    }

    // Writeback stage
    public class wb {
        boolean hot = false;
        long instruction;
        long instrAddr;

        // WB stage pipeline registers
        long ctl_memtoreg;
        long ctl_regwrite;
        long ctl_dest_reg_addr;

        long data_memreaddata;
        long data_alu_result;

        long i_instruction;
        long i_instrAddr;
        
        long i_ctl_memtoreg;
        long i_ctl_regwrite;
        long i_ctl_dest_reg_addr;

        long i_data_memreaddata;
        long i_data_alu_result;

        private memory coreMem;

        public wb(memory coreMem) {
            this.coreMem = coreMem;
        }

        public void printvars() {
            System.out.println("WB vars");
            System.out.println("\tinstruction: " + String.format("%08x",instruction));
            System.out.println("\tinstrAddr: " + String.format("%08x",instrAddr));

            // WB stage pipeline registers
            System.out.println("\tctl_memtoreg: " + ctl_memtoreg);
            System.out.println("\tctl_regwrite: " + ctl_regwrite);
            System.out.println("\tctl_dest_reg_addr: " + ctl_dest_reg_addr);

            System.out.println("\tdata_memreaddata: " + String.format("%08x",data_memreaddata));
            System.out.println("\tdata_alu_result: " + String.format("%08x",data_alu_result));
        }

        public void printnextvars() {
            System.out.println("WB next vars");
            System.out.println("\thot: " + hot);
            System.out.println("\ti_instruction: " + String.format("%08x",i_instruction));
            System.out.println("\ti_instrAddr: " + String.format("%08x",i_instrAddr));

            // WB stage pipeline registers
            System.out.println("\ti_ctl_memtoreg: " + i_ctl_memtoreg);
            System.out.println("\ti_ctl_regwrite: " + i_ctl_regwrite);
            System.out.println("\ti_ctl_dest_reg_addr: " + i_ctl_dest_reg_addr);

            System.out.println("\ti_data_memreaddata: " + String.format("%08x",i_data_memreaddata));
            System.out.println("\ti_data_alu_result: " + String.format("%08x",i_data_alu_result));
        }
    
        private void eval() {
            System.out.println("wb eval: " + String.format("%08x", instrAddr));

            if(ctl_regwrite == 1 && ctl_dest_reg_addr != 0)
                coreMem.regFile[(int) ctl_dest_reg_addr] =
                    (ctl_memtoreg == 0) ? data_alu_result : data_memreaddata;
        }

        private void clock() {
            instruction = i_instruction;
            instrAddr = i_instrAddr;
            
            ctl_memtoreg = i_ctl_memtoreg;
            ctl_regwrite = i_ctl_regwrite;
            ctl_dest_reg_addr = i_ctl_dest_reg_addr;

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
            }

            return 0;
        }
    }
}

