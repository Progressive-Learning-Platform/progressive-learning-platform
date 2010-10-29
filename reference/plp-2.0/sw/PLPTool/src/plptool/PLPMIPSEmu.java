/*
 * The PLP MIPS Architecture Emulator Backend
 */

package plptool;

import java.util.HashMap;

/**
 *
 * @author wira
 */
public class PLPMIPSEmu {

    static int cycle;

    // Initialize core
    public static int coreInit(long[] objCode, long[] addrTable) {
        int i;
        cycle = 0;
        memory.instr = new HashMap<Long, Long> ();
        memory.data = new HashMap<Long, Long> ();
        memory.regFile = new long[32];
        
        for(i = 0; i < objCode.length; i++)
            memory.instr.put(addrTable[i], objCode[i]);

        memory.pc = addrTable[0];
        rf.hotFreshInstruction = memory.instr.get(memory.pc);
        rf.fwd_ctl_pcplus4 = memory.pc + 4;

        return PLPMsg.PLP_OK;
    }

    public static int step () {
        cycle++;

        // Evaluate from last stage first so we don't destroy instructions
        // that are in-flight immediately. Evaluating the stages in order
        // will make a single cycle machine.
        if(wb.hot)  wb.eval();
        if(mem.hot) mem.eval();
        if(ex.hot)  ex.eval();
        rf.eval();

        // update pc for next instruction
        if(mem.hot && mem.ctl_pcsrc == 1)
            memory.pc = mem.ctl_branchTarget;
        else
            memory.pc += 4;

         // fetch instruction
        rf.hotFreshInstruction = memory.instr.get(memory.pc);
        rf.fwd_ctl_pcplus4 = memory.pc + 4;

        return 0;
    }

    public int run () {
        return 0;
    }
}

class consts {
    final static  int   R_MASK = 0x1F;
    final static  int   V_MASK = 0x3F;
    final static  int   C_MASK = 0xFFFF;
}

class memory {
    static        long                    pc;
    static        HashMap<Long, Long>     instr;
    static        HashMap<Long, Long>     data;
    static        long[]                  regFile;
}

// Register file stage / control decode
class rf {
    // RF stage pipeline registers
    static long hotFreshInstruction;
    static long fwd_ctl_pcplus4;

    static void eval() {
        ex.hot = true;

        ex.ctl_pcplus4 = fwd_ctl_pcplus4;

        byte opcode      = (byte) ((hotFreshInstruction >> 26) & consts.V_MASK);
        long addr_read_0 = (hotFreshInstruction >> 16) & consts.R_MASK;
        long addr_read_1 = (hotFreshInstruction >> 21) & consts.R_MASK;

        ex.data_rt     = (addr_read_0 == 0) ? 0 : memory.regFile[(int) addr_read_0];
        ex.data_alu_in = (addr_read_1 == 0) ? 0 : memory.regFile[(int) addr_read_1];
        
        // careful here, we actually need the sign
        //  sign extension, java style, pffft
        long imm_field = (short) (hotFreshInstruction & consts.C_MASK);
        ex.data_imm_signExtended = imm_field;

        ex.ctl_rd_addr = (hotFreshInstruction >> 11) & consts.R_MASK;
        ex.ctl_rt_addr = addr_read_1;

        ex.ctl_aluOp = hotFreshInstruction;

        // control logic
        ex.fwd_ctl_memtoreg = 0;
        ex.fwd_ctl_regwrite = 0;
        ex.fwd_ctl_branch = 0;
        ex.fwd_ctl_memwrite = 0;
        ex.fwd_ctl_memread = 0;
        ex.ctl_aluSrc = 0;
        ex.ctl_regDst = 0;

        switch(PLPAsm.lookupInstrType(PLPAsm.lookupInstr(opcode))) {
            case 0:
            case 1:
            case 2:
                ex.fwd_ctl_regwrite = 1;
                ex.ctl_regDst = 1;

                break;

            case 3:
                ex.fwd_ctl_branch = 1;

                break;

            case 4:
            case 5:
                ex.fwd_ctl_regwrite = 1;
                ex.ctl_aluSrc = 1;

                break;

            case 6:
                if(PLPAsm.lookupInstr(opcode).equals("lw")) {
                    ex.fwd_ctl_memtoreg = 1;
                    ex.fwd_ctl_regwrite = 1;
                    ex.fwd_ctl_memread = 1;
                    ex.ctl_aluSrc = 1;
                } else if(PLPAsm.lookupInstr(opcode).equals("sw")) {
                    ex.fwd_ctl_memwrite = 1;
                    ex.ctl_aluSrc = 1;
                }
                
                break;
        }
    }
}

// Execute stage
class ex {
    static boolean hot = false;

    // EX stage pipeline registers
    static long fwd_ctl_memtoreg;
    static long fwd_ctl_regwrite;

    static long fwd_ctl_branch;
    static long fwd_ctl_memwrite;
    static long fwd_ctl_memread;

    static long ctl_aluSrc;
    static long ctl_aluOp;
    static long ctl_regDst;

    static long ctl_pcplus4;

    static long data_alu_in;
    static long data_rt;

    static long data_imm_signExtended;
    static long ctl_rt_addr;
    static long ctl_rd_addr;

    static void eval() {
        mem.hot = true;

        mem.fwd_ctl_memtoreg = fwd_ctl_memtoreg;
        mem.fwd_ctl_regwrite = fwd_ctl_regwrite;
        mem.fwd_ctl_dest_reg_addr = (ctl_regDst == 1) ? ctl_rd_addr : ctl_rt_addr;

        mem.ctl_memwrite = fwd_ctl_memwrite;
        mem.ctl_memread = fwd_ctl_memread;

        mem.ctl_branch = fwd_ctl_branch;
        mem.ctl_branchTarget = ctl_pcplus4 + (data_imm_signExtended << 2);

        mem.data_memwritedata = data_rt;

        mem.fwd_data_alu_result =
                alu.eval(data_alu_in,
                         ((ctl_aluSrc == 1) ? data_imm_signExtended : data_rt),
                         ctl_aluOp);
    }
}

// Memory stage
class mem {
    static boolean hot = false;

    // MEM stage pipeline registers
    static long fwd_ctl_memtoreg;
    static long fwd_ctl_regwrite;
    static long fwd_ctl_dest_reg_addr;
    static long fwd_data_alu_result;

    static long ctl_branch;
    static long ctl_memwrite;
    static long ctl_memread;
    static long ctl_branchTarget;

    static long data_memwritedata;

    static long ctl_pcsrc;
    static long ctl_regwrite;

    static void eval() {
        wb.hot = true;

        wb.ctl_memtoreg = fwd_ctl_memtoreg;
        wb.ctl_regwrite = fwd_ctl_regwrite;
        wb.ctl_dest_reg_addr = fwd_ctl_dest_reg_addr;
        
        wb.data_alu_result = fwd_data_alu_result;

        if(ctl_memread == 1)
            wb.data_memreaddata = memory.data.get(fwd_data_alu_result);

        if(ctl_memwrite == 1)
            memory.data.put(fwd_data_alu_result, data_memwritedata);

        ctl_pcsrc = (fwd_data_alu_result == 0) ? 0 : 1;
        ctl_pcsrc &= ctl_branch;
    }
}

// Writeback stage
class wb {
    static boolean hot = false;

    // WB stage pipeline registers
    static long ctl_memtoreg;
    static long ctl_regwrite;
    static long ctl_dest_reg_addr;

    static long data_memreaddata;
    static long data_alu_result;
    
    static void eval() {
        if(ctl_regwrite == 1)
            memory.regFile[(int) ctl_dest_reg_addr] =
                    (ctl_memtoreg == 1) ? data_alu_result : data_memreaddata;

    }
}

class alu {
    static long eval(long a, long b, long instr) {

        return 0;
    }
}