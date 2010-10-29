/*
 * Utility class to package / display object codes produces by PLPAsm
 */

package plptool;

import java.io.FileOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.util.HashMap;
import java.util.Iterator;

/**
 *
 * @author wira
 */
public class PLPAsmFormatter {

    // Print to stdout, prettily.
    public static int prettyPrint(PLPAsm assembler) {
        String label;
        long addrTable[] = assembler.getAddrTable();
        long objectCode[] = assembler.getObjectCode();

        if(addrTable.length != objectCode.length)
            return PLPMsg.PLP_ERROR_GENERIC;

        System.out.println("Label\t\tAddress\t\tInstruction\top     rs    rt    rd    shamt funct");
        System.out.println("-----\t\t-------\t\t-----------\t------ ----- ----- ----- ----- -----");

        for(int i = 0; i < addrTable.length; i++) {
            if((label = assembler.lookupLabel(addrTable[i])) != null) {
                System.out.print(label + "\n\t\t");
            } else {
                System.out.print("\t\t");
            }

            System.out.print("0x" + String.format("%07x", addrTable[i]) + "\t");
            System.out.print(String.format("%08x", (int) objectCode[i]) + "\t");
            System.out.print(mipsBinFormat(intBinPadder((int) objectCode[i], 32)));

            System.out.println();
        }

        return 0;
    }

    // Output packed binary file
    public static int writeBin(long[] objectCode, String outputFileName) {
        try {
            DataOutputStream out = new DataOutputStream(new FileOutputStream(new File(outputFileName + ".bin")));

            for(int i = 0; i < objectCode.length; i++)
                out.writeInt((int) objectCode[i]);

            out.close();

        } catch(Exception e) {
            return PLPMsg.E("writeBin(): Unable to write to " + outputFileName + ".bin",
                     PLPMsg.PLP_OUT_CAN_NOT_WRITE_TO_FILE, null);
        }

        return 0;
    }

    // Output COE
    public static int writeCOE(long[] objectCode, String outputFileName) {
        String binString;

        try {
            DataOutputStream out = new DataOutputStream(new FileOutputStream(new File(outputFileName + ".coe")));
            out.writeChars("memory_initialization_radix=2;\n");
            out.writeChars("memory_initialization_vector=\n");

            for(int i = 0; i < objectCode.length; i++) {
                binString = intBinPadder((int) objectCode[i], 32);
                out.writeChars(binString);

                if(i == (objectCode.length - 1))
                    out.writeChars(";\n");
                else
                    out.writeChars(",\n");
            }
            out.close();

        } catch(Exception e) {
            return PLPMsg.E("writeCOE(): Unable to write to " + outputFileName + ".coe",
                     PLPMsg.PLP_OUT_CAN_NOT_WRITE_TO_FILE, null);
        }

        return 0;
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
            PLPMsg.E("intBinpadder(): can't convert " + number,
                     PLPMsg.PLP_OUT_UNHANDLED_ERROR, null);
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

    // Symbol table pretty print
    public static int symTablePrettyPrint(HashMap symTable) {
        String key, value;

        System.out.println("\nSymbol Table" +
                           "\n============");
        Iterator iterator = symTable.keySet().iterator();

        while(iterator.hasNext()) {
            key = iterator.next().toString();
            value = String.format("0x%08x", symTable.get(key));

            System.out.println(key + "\t\t:\t" + value);
        }

        return 0;
    }
}
