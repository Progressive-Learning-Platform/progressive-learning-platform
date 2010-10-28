/*
 * Utility class to package / display object codes produces by PLPAsm
 */

package plptool;

import java.util.Formatter;

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

        System.out.println("Label\t\tAddress\t\tInstruction");
        System.out.println("-----\t\t-------\t\t-----------");

        for(int i = 0; i < addrTable.length; i++) {
            if((label = assembler.lookupLabel(addrTable[i])) != null) {
                System.out.print(label + "\n\t\t");
            } else {
                System.out.print("\t\t");
            }

            System.out.print("0x" + String.format("%07x", addrTable[i]) + "\t");
            System.out.print(String.format("%08x", (int) objectCode[i]) + "\t");

            System.out.println();
        }

        return 0;
    }
}
