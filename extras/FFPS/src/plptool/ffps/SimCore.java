/*
    Copyright 2012 PLP Contributors

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

package plptool.ffps;

import plptool.*;
import plptool.gui.PLPToolApp;
import plptool.mips.*;
import java.util.HashMap;

/**
 * we use visibleAddr from PLPSimCore as our pc
 *
 * @author wira
 */
public class SimCore extends PLPSimCore {
    private plptool.mips.Asm asm;
    public long[] regfile = new long[32];
    private long branch_destination;
    private boolean branch;
    private boolean no_eval;	
    public long total_bus_eval_latency;
    private boolean pre_disassemble_program;
    byte[] opcode_array;
    byte[] funct_array;
    byte[] rs_array;
    byte[] rd_array;
    byte[] rt_array;
    byte[] sa_array;
    long[] imm_array;
    long[] jaddr_array;

    private byte opcode, rs, rd, rt, funct, sa;
    private long imm, jaddr, pcplus4, instr;
    private long s, t, s_imm;
    private Long ir;
    private HashMap<Long, Integer> disassembly_addr_map;
    private int disassembly_index;

    public SimCore() {
        super();
        bus = new PLPSimBus();
        bus.enableAllModules();
        no_eval = PLPToolApp.getAttributes().containsKey("ffps_no_eval");
        pre_disassemble_program = PLPToolApp.getAttributes().containsKey("ffps_predisassemble");
        if(no_eval) Msg.W("Bus evaluation is disabled.", this);
        if(pre_disassemble_program) Msg.I("Pre-disassembling of the program is enabled", this);
        total_bus_eval_latency = 0;
    }

    public int loadProgram(PLPAsm asm) {
        this.asm = (plptool.mips.Asm) asm;
        if(pre_disassemble_program) {
            int programLength = asm.getObjectCode().length;
            opcode_array = new byte[programLength];
            funct_array = new byte[programLength];
            rs_array = new byte[programLength];
            rd_array = new byte[programLength];
            rt_array = new byte[programLength];
            sa_array = new byte[programLength];
            imm_array = new long[programLength];
            jaddr_array = new long[programLength];
            disassembly_addr_map = new HashMap<Long, Integer>();
        }
        for(int i = 0; i < asm.getObjectCode().length; i++) {
            bus.write(asm.getAddrTable()[i], asm.getObjectCode()[i], true);
            if(pre_disassemble_program) {
                instr = asm.getObjectCode()[i];
                opcode_array[i] = MIPSInstr.opcode(instr);
                funct_array[i] = MIPSInstr.funct(instr);
                rs_array[i] = MIPSInstr.rs(instr);
                rd_array[i] = MIPSInstr.rd(instr);
                rt_array[i] = MIPSInstr.rt(instr);
                sa_array[i] = MIPSInstr.sa(instr);
                imm_array[i] = MIPSInstr.imm(instr);
                jaddr_array[i] = MIPSInstr.jaddr(instr);
                disassembly_addr_map.put(asm.getAddrTable()[i], i);
            }
        }

        return Constants.PLP_OK;
    }

    public int step() {
        int ret = Constants.PLP_OK;
        instructionCount++;
        if(!pre_disassemble_program) {
            ir = (Long) bus.read(visibleAddr);
            if(ir == null)
                return Msg.E("Instruction fetch error.", Constants.PLP_SIM_INSTRUCTION_FETCH_FAILED, this);
            instr = ir;
            opcode = MIPSInstr.opcode(instr);
            rs = MIPSInstr.rs(instr);
            rd = MIPSInstr.rd(instr);
            rt = MIPSInstr.rt(instr);
            funct = MIPSInstr.funct(instr);
            sa = MIPSInstr.sa(instr);
            imm = MIPSInstr.imm(instr);
            jaddr = MIPSInstr.jaddr(instr);            
        } else {
            disassembly_index = disassembly_addr_map.get(visibleAddr);
            instr = asm.getObjectCode()[disassembly_index];
            opcode = opcode_array[disassembly_index];
            funct = funct_array[disassembly_index];
            rs = rs_array[disassembly_index];
            rd = rd_array[disassembly_index];
            rt = rt_array[disassembly_index];
            sa = sa_array[disassembly_index];
            imm = imm_array[disassembly_index];
            jaddr = jaddr_array[disassembly_index];
        }

        pcplus4 = visibleAddr+4;
        s = (rs == 0) ? 0 : regfile[rs];
        t = (rt == 0) ? 0 : regfile[rt];
        s_imm =  (short) imm & ((long) 0xfffffff << 4 | 0xf);

        // are we in the branch delay slot
        if(!branch)
            visibleAddr = pcplus4;
        else {
            visibleAddr = branch_destination;
            branch = false;
        }
        
        // execute instruction
        switch(opcode) {
            case 0: // r-type instructions
                switch(funct) {
                    case 0x21:
                        regfile[rd] = s+t;
                        break;
                        
                    case 0x23:
                        regfile[rd] = s-t;
                        break;
                        
                    case 0x24:
                        regfile[rd] = s&t;
                        break;

                    case 0x25:
                        regfile[rd] = s|t;
                        break;

                    case 0x27:
                        regfile[rd] = ~(s|t);
                        break;

                    case 0x2A:
                        int s_signed = (int) s;
                        int t_signed = (int) t;
                        regfile[rd] = (s_signed < t_signed) ? 1 : 0;
                        break;

                    case 0x2B:
                        regfile[rd] = (s < t) ? 1 : 0;
                        break;

                    case 0x10:
                        regfile[rd] = ((long)(int) s * (long)(int)t) & 0xffffffffL;
                        break;

                    case 0x11:
                        regfile[rd] = (((long)(int) s * (long)(int) t) & 0xffffffff00000000L) >> 32;
                        break;

                    case 0x00:
                        regfile[rd] = t << sa;
                        break;

                    case 0x02:
                        regfile[rd] = t >>> sa;
                        break;

                    case 0x08:
                        branch = true;
                        branch_destination = s;
                        break;

                    case 0x09:
                        branch = true;
                        branch_destination = s;
                        regfile[31] = pcplus4+4;
                        break;

                    default:
                        return Msg.E("Unhandled instruction: invalid function field: " + PLPToolbox.format32Hex(funct),
                            Constants.PLP_SIM_UNHANDLED_INSTRUCTION_TYPE, this);
                }
                break;

            case 0x04: // beq
                if(s == t) {
                    branch = true;
                    branch_destination = (pcplus4 + (s_imm<<2)) & 0xffffffffL;
                }
                break;

            case 0x05: // bne
                if(s != t) {
                    branch = true;
                    branch_destination = (pcplus4 + (s_imm<<2)) & 0xffffffffL;
                }
                break;

            case 0x09: // addiu
                regfile[rt] = s + s_imm;
                break;

            case 0x0C: // andi
                regfile[rt] = s & imm;
                break;

            case 0x0D: // ori
                regfile[rt] = s | imm;
                break;

            case 0x0A: // slti
                int s_signed = (int) s;
                regfile[rt] = (s_signed < s_imm) ? 1 : 0;
                break;

            case 0x0B: // sltiu
                regfile[rt] = (s < s_imm) ? 1 : 0;
                break;

            case 0x0F: // lui
                regfile[rt] = imm << 16;
                break;

            case 0x23: // lw
                Long data = (Long) bus.read(s + s_imm);
                if(data == null)
                    return Msg.E("Bus read error.", Constants.PLP_SIM_BUS_ERROR, this);
                regfile[rt] = data;
                break;

            case 0x2B: // sw
                ret = bus.write(s + s_imm, regfile[rt], false);
                if(pre_disassemble_program && disassembly_addr_map.containsKey(s + s_imm)) {
                    disassembly_index = disassembly_addr_map.get(s + s_imm);
                    opcode_array[disassembly_index] = MIPSInstr.opcode(instr);
                    funct_array[disassembly_index] = MIPSInstr.funct(instr);
                    rs_array[disassembly_index] = MIPSInstr.rs(instr);
                    rd_array[disassembly_index] = MIPSInstr.rd(instr);
                    rt_array[disassembly_index] = MIPSInstr.rt(instr);
                    sa_array[disassembly_index] = MIPSInstr.sa(instr);
                    imm_array[disassembly_index] = MIPSInstr.imm(instr);
                    jaddr_array[disassembly_index] = MIPSInstr.jaddr(instr);
                }
                if(ret > 0)
                    return Msg.E("Bus write error.", Constants.PLP_SIM_BUS_ERROR, this);
                break;

            case 0x03: // jal
                regfile[31] = pcplus4+4;
            case 0x02: // j
                branch = true;
                branch_destination = jaddr<<2 | (pcplus4 & 0xf0000000L);
                break;

            default:
                return Msg.E("Unhandled instruction: invalid op-code",
                        Constants.PLP_SIM_UNHANDLED_INSTRUCTION_TYPE, this);
        }

        if(!no_eval) {
            bus.eval(); 
	}

        return ret;
    }

    public int reset() {
        visibleAddr = asm.getEntryPoint();
        bus.reset();
        this.loadProgram(asm);
        instructionCount = 0;
        total_bus_eval_latency = 0;
        branch = false;
        branch_destination = 0;
        for(int i = 0; i < 32; i++)
            regfile[i] = 0;
        return Constants.PLP_OK;
    }

    @Override
    public String toString() {
        return "FFPS SimCore";
    }
}
