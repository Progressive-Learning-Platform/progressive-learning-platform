/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.ffps;

import plptool.*;
import plptool.gui.PLPToolApp;
import plptool.mips.*;
import plptool.mods.MemModule;

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
    private long bus_eval_latency_start_time;

    public SimCore() {
        super();
        bus = new PLPSimBus();
        bus.enableAllModules();
        no_eval = PLPToolApp.getAttributes().containsKey("ffps_no_eval");
        if(no_eval) Msg.W("Bus evaluation is disabled.", this);
        total_bus_eval_latency = 0;
    }

    public int loadProgram(PLPAsm asm) {
        this.asm = (plptool.mips.Asm) asm;
        for(int i = 0; i < asm.getObjectCode().length; i++)
            bus.write(asm.getAddrTable()[i], asm.getObjectCode()[i], true);

        return Constants.PLP_OK;
    }

    public int step() {
        int ret = Constants.PLP_OK;
        instructionCount++;
        Long ir = (Long) bus.read(visibleAddr);
        if(ir == null)
            return Msg.E("Instruction fetch error.", Constants.PLP_SIM_INSTRUCTION_FETCH_FAILED, this);
        long instr = ir;
        byte opcode = MIPSInstr.opcode(instr);
        byte rs = MIPSInstr.rs(instr);
        byte rd = MIPSInstr.rd(instr);
        byte rt = MIPSInstr.rt(instr);
        byte funct = MIPSInstr.funct(instr);
        byte sa = MIPSInstr.sa(instr);
        long imm = MIPSInstr.imm(instr);
        long jaddr = MIPSInstr.jaddr(instr);
        long pcplus4 = visibleAddr+4;

        long s = (rs == 0) ? 0 : regfile[rs];
        long t = (rt == 0) ? 0 : regfile[rt];
        //long d = (rd == 0) ? 0 : regfile[rd];
        long s_imm =  (short) imm & ((long) 0xfffffff << 4 | 0xf);

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
}
