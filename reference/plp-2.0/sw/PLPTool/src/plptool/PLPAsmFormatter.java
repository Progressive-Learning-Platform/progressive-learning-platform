/*
 * Utility class to package / display object codes produces by PLPAsm
 */

package plptool;

import java.io.FileOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;

import org.apache.commons.compress.archivers.tar.TarArchiveOutputStream;
import org.apache.commons.compress.archivers.tar.TarArchiveEntry;

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

        return PLPMsg.PLP_OK;
    }

    // Output packed binary file
    public static int writeBin(long[] objectCode, String outputFileName) {
        try {
            DataOutputStream out = new DataOutputStream(new FileOutputStream(new File(outputFileName)));

            for(int i = 0; i < objectCode.length; i++)
                out.writeInt((int) objectCode[i]);

            out.close();

        } catch(Exception e) {
            return PLPMsg.E("writeBin(): Unable to write to " + outputFileName,
                     PLPMsg.PLP_OUT_CAN_NOT_WRITE_TO_FILE, null);
        }

        return PLPMsg.PLP_OK;
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
        String out = "";

        for(int i = 0; i < objectCode.length; i++) {
            out += "32'h" + String.format("%08x", (int) objectCode[i]) + "\n";
        }
        
        return out;
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

    // MIPS instruction string formatter {
    public static String mipsInstrStr(long instr) {
        String ret = "";

        ret += PLPAsm.lookupInstr((byte) MIPSInstruction.opcode(instr)) + " ";
        ret += MIPSInstruction.rs(instr) + " ";
        ret += MIPSInstruction.rt(instr) + " ";
        ret += MIPSInstruction.rd(instr) + " ";
        ret += MIPSInstruction.sa(instr) + " ";
        ret += MIPSInstruction.imm(instr);

        return ret;
    }

    // Generate PLP file
    public static int genPLP(String input, String output, boolean forceWrite) {
        LinkedList sourceList;
        PLPAsm assembler = new PLPAsm(null, input, 0);
        String metafileStr = "";
        String verilogHex = "";
        long[] objCode = null;
        PLPAsmSource temp;
        int i;

        try {

        File outFile = new File(output + ".plp");

        if(outFile.exists() && !forceWrite) {
            return PLPMsg.E("genPLP(): " + output + ".plp already exists. Use -af to overwrite.",
                            PLPMsg.PLP_OUT_FILE_EXISTS, null);
        }

        metafileStr += "PLP-2.0\n";

        PLPMsg.I("genPLP(): Assembling " + input + ".", null);
        if(assembler.preprocess(0) == PLPMsg.PLP_OK)
            assembler.assemble();

        if(assembler.isAssembled()) {
            objCode = assembler.getObjectCode();
            verilogHex = writeVerilogHex(objCode);
            metafileStr += "DIRTY=0\n\n";
            PLPMsg.I("genPLP(): Assembly completed.", null);
        }
        else {
            metafileStr += "DIRTY=1\n\n";
            PLPMsg.I("genPLP(): Assembly failed.", null);
        }

        sourceList = assembler.getAsmList();

        for(i = 0; i < sourceList.size(); i++) {
            temp = (PLPAsmSource) sourceList.get(i);
            metafileStr += temp.asmFilePath + "\n";
        }


        // Create <output>.plp (a tar archive)
        TarArchiveOutputStream tOut = new TarArchiveOutputStream(new FileOutputStream(outFile));
        
        TarArchiveEntry entry = new TarArchiveEntry("plp.metafile");
        entry.setSize(metafileStr.length());
        tOut.putArchiveEntry(entry);
        byte[] data = new byte[metafileStr.length()];
        for(i = 0; i < metafileStr.length(); i++) {
            data[i] = (byte) metafileStr.charAt(i);
        }
        tOut.write(data);
        tOut.flush();
        tOut.closeArchiveEntry();

        if(assembler.isAssembled()) {
            // Write hex image
            entry = new TarArchiveEntry("plp.hex");
            entry.setSize(verilogHex.length());
            tOut.putArchiveEntry(entry);
            data = new byte[verilogHex.length()];
            for(i = 0; i < verilogHex.length(); i++) {
                data[i] = (byte) verilogHex.charAt(i);
            }
            tOut.write(data);
            tOut.flush();
            tOut.closeArchiveEntry();

            // Write binary image, 4-byte big-endian packs
            entry = new TarArchiveEntry("plp.image");
            entry.setSize(objCode.length * 4);
            tOut.putArchiveEntry(entry);
            data = new byte[objCode.length * 4];
            for(i = 0; i < objCode.length; i++) {
                data[4*i] = (byte) (objCode[i] >> 24);
                data[4*i+1] = (byte) (objCode[i] >> 16);
                data[4*i+2] = (byte) (objCode[i] >> 8);
                data[4*i+3] = (byte) (objCode[i]);
            }
            tOut.write(data);
            tOut.flush();
            tOut.closeArchiveEntry();
        }

        tOut.close();

        PLPMsg.I("genPLP(): " + output + ".plp written", null);

        } catch(Exception e) {
            return PLPMsg.E("genPLP(): Unable to write to <" + output + ">\n" +
                     e, PLPMsg.PLP_OUT_CAN_NOT_WRITE_TO_FILE, null);
        }

        return PLPMsg.PLP_OK;
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
