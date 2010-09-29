package plptool;

import java.util.*;
import java.io.*;

/**
 * PLPAsm Class
 *
 * This class implements the modular PLP assembler object. The default
 * constructor will initialize required symbols for the assembler.
 * The assembler class can handle multiple assembly sources and
 * cross-references.
 *
 * @author wira
 */
public class PLPAsm {

    LinkedList  SourceList = new LinkedList();

    int[]       addrTable;
    int[]       symSizeTable;
    int         curAddr;
    String      preprocessedAsm;
    String      objCode;
    String      curActiveFile;

    HashMap     instrMap;
    HashMap     symTable;

    public PLPAsm (String strAsm, String strFilePath, int intStartAddr) {
        PLPAsmSource plpAsmObj = new PLPAsmSource(strAsm, strFilePath, 0);
        SourceList.add(plpAsmObj);
        preprocessedAsm = new String();
        objCode = new String();
        curAddr = intStartAddr;
        instrMap = new HashMap();
        symTable = new HashMap();

        // R-type Arithmetic and Logical instructions
        instrMap.put(new String("addu"), new Integer(0));
        instrMap.put(new String("subu"), new Integer(0));
        instrMap.put(new String("and"),  new Integer(0));
        instrMap.put(new String("or"),   new Integer(0));
        instrMap.put(new String("nor"),  new Integer(0));
        instrMap.put(new String("slt"),  new Integer(0));
        instrMap.put(new String("sltu"), new Integer(0));

        // R-type Shift instructions
        instrMap.put(new String("sll"),  new Integer(1));
        instrMap.put(new String("srl"),  new Integer(1));

        // R-type Jump instructions
        instrMap.put(new String("jr"),   new Integer(2));
        instrMap.put(new String("jalr"), new Integer(2));

        // I-type Branch instructions
        instrMap.put(new String("beq"),  new Integer(3));
        instrMap.put(new String("bne"),  new Integer(3));

        // I-type Arithmetic and Logical instructions
        instrMap.put(new String("addiu"), new Integer(4));
        instrMap.put(new String("andi"),  new Integer(4));
        instrMap.put(new String("ori"),   new Integer(4));
        instrMap.put(new String("slti"),  new Integer(4));
        instrMap.put(new String("sltiu"), new Integer(4));

        // I-type Load Upper Immediate instruction
        instrMap.put(new String("lui"),  new Integer(5));

        // I-type Load and Store word instructions
        instrMap.put(new String("lw"),   new Integer(6));
        instrMap.put(new String("sw"),   new Integer(6));

        // J-type Instructions
        instrMap.put(new String("j"),    new Integer(7));
        instrMap.put(new String("jal"),  new Integer(7));

        // Multiply instructions
        instrMap.put(new String("multu"), new Integer(8));
        instrMap.put(new String("mfhi"),  new Integer(8));
        instrMap.put(new String("mflo"),  new Integer(8));

        // Pseudo-ops

    }

    /**
     * Pre-process / perform 1st pass assembly on all assembly sources
     * attached to this assembler
     *
     * @return
     */
    public int preprocess(int recLevel) {
        int i = 0, j = 0;
        int recursionRetVal;

        PLPAsmSource topLevelAsm = (PLPAsmSource) SourceList.get(recLevel);
        curActiveFile = topLevelAsm.asmFilePath;

        String delimiters = "[,]|[ ]+";
        String lineDelim  = "\\r?\\n";
        String[] asmLines  = topLevelAsm.asmString.split(lineDelim);
        String[] asmTokens;

        // Begin our preprocess cases
        while(i < asmLines.length) {
            j = 0;
            asmTokens = asmLines[i].split(delimiters);
            i++;

            // Include statement
            if(asmTokens[j].equals(".include")) {
                if(asmTokens.length < 2) {
                   PLPMsg.PLPInfo(topLevelAsm.asmFilePath + "(" + i + "):");
                   return PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX;
                }

                preprocessedAsm += "ASM__DIRECTIVE__\n";

                PLPAsmSource childAsm = new PLPAsmSource
                                            (null, asmTokens[j+1], recLevel + 1);
                SourceList.add(childAsm);
                recursionRetVal = this.preprocess(recLevel + 1);

                if(recursionRetVal != 0)
                    return recursionRetVal;
            }

            // .word directive:
            //   Initialize current memory address to a value
            else if(asmTokens[j].equals(".word")) {

            }
            
            // Label handler
            //   Everything after the label is IGNORED, it has to be on its own
            //   line
            else if(asmTokens[j].charAt(asmTokens[j].length() - 1) == ':')
            {
                symTable.put(new String(asmTokens[j].substring(0, asmTokens[j].length() - 2)),
                             new Integer(curAddr));
                preprocessedAsm += "ASM__LABEL__\n";
            } else {
                if(instrMap.containsKey(asmTokens[j]) == false) {
                    PLPMsg.PLPInfo(topLevelAsm.asmFilePath + "(" + i + "): Unable to process token " + asmTokens[j]);
                    return PLPMsg.PLP_ASM_ERROR_INVALID_TOKEN;
                }
                curAddr += 4;
                preprocessedAsm += asmLines[i - 1] + "\n";
            }
        }

        return 0;
    }

    /**
     * Assemble all assembly sources attached to this assembler and generate
     * object codes
     *
     * @return
     */
    public int assemble() {

        return 0;
    }
}

/**
 * PLPAsmSource Class
 * 
 * This class implements an assembly source and its data structures used by
 * PLPAssembler.
 * 
 * @author wira
 */
class PLPAsmSource {

    PLPAsmSource    refSource;
    String          asmString;
    String          asmFilePath;
    
    int             recursionLevel;
    
    public PLPAsmSource(String strAsm, String strFilePath, int intLevel) {
        if(strAsm != null)
            asmString = new String(strAsm);

        asmFilePath = new String(strFilePath);
        recursionLevel = intLevel;
    }

    public int setRefSource(PLPAsmSource plpRefSource) {
        refSource = plpRefSource;

        if(refSource != null)
            return 0;
        else {
            return PLPMsg.PLP_ASM_ERROR_INVALID_REFSOURCE;
        }
    }

}
