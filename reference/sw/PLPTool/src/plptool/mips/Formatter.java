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

import java.io.FileOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.util.HashMap;
import java.util.Iterator;

import plptool.Msg;
import plptool.Constants;
import plptool.PLPToolbox;

/**
 * Utility class to package / display object codes produces by PLPAsm
 *
 * @author wira
 */
public class Formatter {

    // Print to stdout, prettily.
    public static int prettyPrint(plptool.PLPAsm asm) {
        Msg.P(prettyString(asm));

        return Constants.PLP_OK;
    }

    public static String prettyString(plptool.PLPAsm asm) {
        String label;
        String out = "";
        long addrTable[] = asm.getAddrTable();
        long objectCode[] = asm.getObjectCode();
        long tVal;

        out += "Label\t\tAddress\t\tInstruction\top     rs    rt    rd    shamt funct\tASCII\n";
        out += "-----\t\t-------\t\t-----------\t------ ----- ----- ----- ----- -----\t-----\n";

        for(int i = 0; i < addrTable.length; i++) {
            if((label = asm.lookupLabel(addrTable[i])) != null) {
                out += label + "\n\t\t";
            } else {
                out += "\t\t";
            }

            out += "0x" + String.format("%07x", addrTable[i]) + "\t";
            out += String.format("%08x", (int) objectCode[i]) + "\t";
            out += mipsBinFormat(intBinPadder((int) objectCode[i], 32)) + "\t";

            for(int j = 3; j >= 0; j--) {
                tVal = objectCode[i] >> (8 * j);
                tVal &= 0xFF;
                if(tVal >= 0x21 && tVal <= 0x7E)
                    out += (char) tVal + " ";
                else
                    out += ". ";
            }

            out += "\n";
        }

        return out;
    }

    // Output packed binary file
    public static int writeBin(long[] objectCode, String outputFileName) {
        try {
            DataOutputStream out = new DataOutputStream(new FileOutputStream(new File(outputFileName)));

            for(int i = 0; i < objectCode.length; i++)
                out.writeInt((int) objectCode[i]);

            out.close();

        } catch(Exception e) {
            return Msg.E("writeBin(): Unable to write to " + outputFileName,
                     Constants.PLP_OUT_CAN_NOT_WRITE_TO_FILE, null);
        }

        return Constants.PLP_OK;
    }

    // Output COE
    public static String writeCOE(long[] objectCode) {
        String out = "";
        String binString;


        out += "memory_initialization_radix=2;\n";
        out += "memory_initialization_vector=\n";

        for(int i = 0; i < objectCode.length; i++) {
            binString = intBinPadder((int) objectCode[i], 32);
            out += binString;

            if(i == (objectCode.length - 1))
                out += ";\n";
            else
                out += ",\n";
            }

        return out;
    }

    // Output hex to be imported to verilog
    public static String writeVerilogHex(long[] objectCode) {
        StringBuilder outStr = new StringBuilder(objectCode.length * 32);

        for(int i = 0; i < objectCode.length; i++) {
           outStr.append("32'h");
           outStr.append(String.format("%08x", (int) objectCode[i]));
           outStr.append("\n");
        }

        return  outStr.toString();
    }

    // 32-bit binary string padder
    public static String intBinPadder(int number, int length) {
        try {

        String ret = Integer.toBinaryString(number);
        int offset;

        if((offset = length - ret.length()) > 0 ) {
            for(int j = 0; j < offset; j++)
                ret = '0' + ret;
        }

        return ret;

        } catch(Exception e) {
            Msg.E("intBinpadder(): can't convert " + number,
                     Constants.PLP_OUT_UNHANDLED_ERROR, null);
            return null;
        }
    }

    // MIPS binary string formatter
    public static String mipsBinFormat(String instr) {
        String out = "";

        if(instr.length() == 32) {
            for(int i = 0; i < 32; i++) {
                if(i == 6 | i == 11 | i == 16 | i == 21 | i == 26)
                    out += ' ';

                out += instr.charAt(i);
            }

            return out;
        }

        return null;
    }

    // MIPS instruction string formatter {
    public static String mipsInstrStr(long instr) {
        String ret = "";
        if(MIPSInstr.opcode(instr) != 0)
            ret += Asm.lookupInstrOpcode((byte) MIPSInstr.opcode(instr)) + " ";
        else
            ret += Asm.lookupInstrFunct((byte) MIPSInstr.funct(instr)) + " ";
        ret += MIPSInstr.rs(instr) + " ";
        ret += MIPSInstr.rt(instr) + " ";
        ret += MIPSInstr.rd(instr) + " ";
        ret += MIPSInstr.sa(instr) + " ";
        ret += MIPSInstr.imm(instr);

        return ret;
    }

    // Symbol table pretty print
    public static int symTablePrettyPrint(HashMap symTable) {
        String key, value;

        Msg.M("\nSymbol Table" +
                           "\n============");
        Object[][] sortedTbl = PLPToolbox.getSortedStringByLongValue(symTable);

        for(int i = 0; i < sortedTbl.length; i++) {
            key = (String) sortedTbl[i][0];
            value = String.format("0x%08x", (Long) sortedTbl[i][1]);

            Msg.M(value + "\t:\t" + key);
        }

        return 0;
    }
}


