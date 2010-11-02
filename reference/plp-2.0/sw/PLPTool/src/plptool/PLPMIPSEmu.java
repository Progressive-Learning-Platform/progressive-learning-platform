/*
 * The PLP MIPS Architecture Emulator Backend
 */

package plptool;

/**
 *
 * @author wira
 */
public class PLPMIPSEmu {

    private memory coreMem;

    public int instructionCount;

    public rf   rf_stage;
    public ex   ex_stage;
    public mem  mem_stage;
    public wb   wb_stage;

    // Initialize core
    public PLPMIPSEmu(long[] objCode, long[] addrTable, int RAMsize) {
        int i;
        coreMem = new memory(new long[RAMsize],
                             new long[32],
                             0);                // pc=0 on reset

        // load program to RAM
        for(i = 0; i < objCode.length; i++)
            coreMem.mainMem[(int) addrTable[i]] =  objCode[i];

        // Instantiate stages
        wb_stage = new wb(coreMem);
        mem_stage = new mem(wb_stage, coreMem);
        ex_stage = new ex(mem_stage, new alu());
        rf_stage = new rf(ex_stage, coreMem);

        this.reset();
    }

    public int reset() {
        coreMem.pc = 0;
        instructionCount = 0;

        // load up first instruction to core's frontend (rf_stage.i_*)
        rf_stage.i_instruction = coreMem.mainMem[(int) coreMem.pc];
        rf_stage.i_instrAddr = coreMem.pc;
        rf_stage.i_fwd_ctl_pcplus4 = coreMem.pc + 4;
        rf_stage.hot = true;
        ex_stage.hot = false;
        mem_stage.hot = false;
        wb_stage.hot = false;

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
        if(mem_stage.hot && mem_stage.ctl_pcsrc == 1)
            coreMem.pc = mem_stage.ctl_branchTarget;
        else
            coreMem.pc += 4;

        if(coreMem.pc >= coreMem.mainMem.length) {
            return PLPMsg.E("step(): Instruction memory out-of-bounds: addr=" +
                            coreMem.pc,
                            PLPMsg.PLP_EMU_INSTRMEM_OUT_OF_BOUNDS, this);
        }

         // fetch instruction / frontend stage
        rf_stage.i_instruction = coreMem.mainMem[(int) coreMem.pc];
        rf_stage.i_instrAddr = coreMem.pc;
        rf_stage.i_fwd_ctl_pcplus4 = coreMem.pc + 4;

        return PLPMsg.PLP_OK;
    }

    public int run (int instructions) {

        return PLPMsg.PLP_OK;
    }

    public void pause() {

    }

    class consts {
        final static  int   R_MASK = 0x1F;
        final static  int   V_MASK = 0x3F;
        final static  int   C_MASK = 0xFFFF;
    }

    public class memory {
        long                    pc;
        long[]                  mainMem;
        long[]                  regFile;

        public memory(long[] mainMem, long[] regFile, long pc) {
            this.pc = pc;
            this.mainMem = mainMem;
            this.regFile = regFile;
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

        private void eval() {
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
            long imm_field = (short) (instruction & consts.C_MASK);
            ex_reg.i_data_imm_signExtended = imm_field;

            ex_reg.i_ctl_rd_addr = (instruction >> 11) & consts.R_MASK;
            ex_reg.i_ctl_rt_addr = addr_read_1;

            ex_reg.i_ctl_aluOp = instruction;

            // control logic
            ex_reg.i_fwd_ctl_memtoreg = 0;
            ex_reg.i_fwd_ctl_regwrite = 0;
            ex_reg.i_fwd_ctl_branch = 0;
            ex_reg.i_fwd_ctl_memwrite = 0;
            ex_reg.i_fwd_ctl_memread = 0;
            ex_reg.i_ctl_aluSrc = 0;
            ex_reg.i_ctl_regDst = 0;

            switch(PLPAsm.lookupInstrType(PLPAsm.lookupInstr(opcode))) {
                case 0:
                case 1:
                case 2:
                    ex_reg.i_fwd_ctl_regwrite = 1;
                    ex_reg.i_ctl_regDst = 1;

                    break;

                case 3:
                    ex_reg.i_fwd_ctl_branch = 1;

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
            }
            ex_reg.hot = true;
        }

        private void clock() {
            fwd_ctl_pcplus4 = i_fwd_ctl_pcplus4;
            instruction = i_instruction;
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

        long fwd_ctl_branch;
        long fwd_ctl_memwrite;
        long fwd_ctl_memread;

        long ctl_aluSrc;
        long ctl_aluOp;
        long ctl_regDst;

        long ctl_pcplus4;

        long data_alu_in;
        long data_rt;

        long data_imm_signExtended;
        long ctl_rt_addr;
        long ctl_rd_addr;

        long i_instruction;
        long i_instrAddr;

        long i_fwd_ctl_memtoreg;
        long i_fwd_ctl_regwrite;

        long i_fwd_ctl_branch;
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

        private void eval() {
            mem_reg.i_instruction = instruction;
            mem_reg.i_instrAddr = instrAddr;

            mem_reg.i_fwd_ctl_memtoreg = fwd_ctl_memtoreg;
            mem_reg.fwd_ctl_regwrite = fwd_ctl_regwrite;
            mem_reg.i_fwd_ctl_dest_reg_addr = (ctl_regDst == 1) ? ctl_rd_addr : ctl_rt_addr;

            mem_reg.i_ctl_memwrite = fwd_ctl_memwrite;
            mem_reg.i_ctl_memread = fwd_ctl_memread;

            mem_reg.i_ctl_branch = fwd_ctl_branch;
            mem_reg.i_ctl_branchTarget = ctl_pcplus4 + (data_imm_signExtended << 2);

            mem_reg.i_data_memwritedata = data_rt;

            mem_reg.i_fwd_data_alu_result =
                exAlu.eval(data_alu_in,
                           ((ctl_aluSrc == 1) ? data_imm_signExtended : data_rt),
                           ctl_aluOp);
            
            mem_reg.hot = true;
        }

        private void clock() {
            instruction = i_instruction;
            instrAddr = i_instrAddr;
            
            fwd_ctl_memtoreg = i_fwd_ctl_memtoreg;
            fwd_ctl_regwrite = i_fwd_ctl_regwrite;

            fwd_ctl_branch = i_fwd_ctl_branch;
            fwd_ctl_memwrite = i_fwd_ctl_memwrite;
            fwd_ctl_memread = i_fwd_ctl_memread;

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

        long ctl_branch;
        long ctl_memwrite;
        long ctl_memread;
        long ctl_branchTarget;

        long data_memwritedata;

        long ctl_pcsrc;
        long ctl_regwrite;

        long i_instruction;
        long i_instrAddr;

        long i_fwd_ctl_memtoreg;
        long i_fwd_ctl_regwrite;
        long i_fwd_ctl_dest_reg_addr;
        long i_fwd_data_alu_result;

        long i_ctl_branch;
        long i_ctl_memwrite;
        long i_ctl_memread;
        long i_ctl_branchTarget;

        long i_data_memwritedata;

        long i_ctl_pcsrc;
        long i_ctl_regwrite;

        private wb   wb_reg;
        private memory coreMem;

        public mem(wb wb_reg, memory coreMem) {
            this.wb_reg = wb_reg;
            this.coreMem = coreMem;
        }

        private void eval() {
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

            ctl_pcsrc = (fwd_data_alu_result == 0) ? 0 : 1;
            ctl_pcsrc &= ctl_branch;
            wb_reg.hot = true;
        }

        private void clock() {
            wb_reg.hot = true;
            instruction = i_instruction;
            instrAddr = i_instrAddr;
            
            fwd_ctl_memtoreg = i_fwd_ctl_memtoreg;
            fwd_ctl_regwrite = i_fwd_ctl_regwrite;
            fwd_ctl_dest_reg_addr = i_fwd_ctl_dest_reg_addr;
            fwd_data_alu_result = i_fwd_data_alu_result;

            ctl_branch = i_ctl_branch;
            ctl_memwrite = i_ctl_memwrite;
            ctl_memread = i_ctl_memread;
            ctl_branchTarget = i_ctl_branchTarget;

            data_memwritedata = i_data_memwritedata;

            ctl_pcsrc = i_ctl_pcsrc;
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
    
        private void eval() {
            if(ctl_regwrite == 1)
                coreMem.regFile[(int) ctl_dest_reg_addr] =
                    (ctl_memtoreg == 1) ? data_alu_result : data_memreaddata;
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

            return 0;
        }
    }

}
