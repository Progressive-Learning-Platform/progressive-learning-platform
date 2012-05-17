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

/**
 *
 * @author wira
 */
public class MIPSInstr {

    class consts {
        final static  int   R_MASK = 0x1F;
        final static  int   V_MASK = 0x3F;
        final static  int   C_MASK = 0xFFFF;
        final static  int   J_MASK = 0x3FFFFFF;
    }

    public static int imm(long instr) {
        return (int) (instr & consts.C_MASK); }

    public static byte funct(long instr) {
        return (byte) (instr & consts.V_MASK); }

    public static byte sa(long instr) {
        return (byte) ((instr >> 6) & consts.R_MASK);
    }

    public static byte rd(long instr) {
        return (byte) ((instr >> 11) & consts.R_MASK);
    }

    public static byte rt(long instr) {
        return (byte) ((instr >> 16) & consts.R_MASK);
    }

    public static byte rs(long instr) {
        return (byte) ((instr >> 21) & consts.R_MASK);
    }

    public static byte opcode(long instr) {
        return (byte) ((instr >> 26) & consts.V_MASK);
    }

    public static int jaddr(long instr) {
        return (int) (instr & consts.J_MASK);
    }

    public static long resolve_jaddr(long pc, long instr) { //pc=address of jump instruction
        return ((pc+8) & 0xff000000L) | (jaddr(instr)<<2);
    }

    public static long resolve_baddr(long pc, long instr) { //pc=address of branch instruction
        return (pc+4 + (imm(instr)<<2)) & ((long) 0xfffffff << 4 | 0xf);
    }

    public static String mnemonic(long instr) {
        if(opcode(instr) != 0)
            return Asm.lookupInstrOpcode(opcode(instr));
        else
            return Asm.lookupInstrFunct(funct(instr));
    }

    public static String format(long instr) {
        String ret = "";
        int instrType;

        if(opcode(instr) != 0) {
            instrType = Asm.lookupInstrType(Asm.lookupInstrOpcode(opcode(instr)));
            ret = Asm.lookupInstrOpcode(opcode(instr)) + " ";
        }
        else {
            instrType = Asm.lookupInstrType(Asm.lookupInstrFunct(funct(instr)));
            ret = Asm.lookupInstrFunct(funct(instr)) + " ";
        }

        switch(instrType) {
            case 0:
            case 8:
                ret += "$" + rd(instr) + ",$" + rs(instr) + ",$" + rt(instr);
                break;
            case 1:
                ret += "$" + rd(instr) + ",$" + rt(instr) + "," + sa(instr);
                break;
            case 2:
                ret += "$" + rs(instr);
                break;
            case 3:
                ret += "$" + rs(instr) + ",$" + rt(instr) + ",0x" + String.format("%x", imm(instr));
                break;
            case 4:
                ret += "$" + rt(instr) + ",$" + rs(instr) + ",0x" + String.format("%x", imm(instr));
                break;
            case 5:
                ret += "$" + rt(instr) + ",0x" + String.format("%x", imm(instr));
                break;
            case 6:
                ret += "$" + rt(instr) + "," + imm(instr) + "($" + rs(instr) + ")";
                break;
            case 7:
                ret += String.format("%08x", jaddr(instr));
                ret += " + instrAddr[31:28]";
                break;
            case 9:
                ret += "$" + rd(instr) + ",$" + rs(instr);
                break;
        }

        if(plptool.Config.cfgInstrPretty && instr == 0)
            return "nop";

        return ret;
    }
}
