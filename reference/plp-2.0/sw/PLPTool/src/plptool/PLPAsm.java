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
    String[]    symTable;
    int[]       symSizeTable;
    String      preprocessedAsm;
    String      objCode;
    String      curActiveFile;

    int         startAddr;

    public PLPAsm (String strAsm, String strFilePath, int intStartAddr) {
        PLPAsmSource plpAsmObj = new PLPAsmSource(strAsm, strFilePath, 0);
        SourceList.add(plpAsmObj);
        preprocessedAsm = new String();
        objCode = new String();
        startAddr = intStartAddr;
    }

    /**
     * Pre-process / perform 1st pass assembly on all assembly sources
     * attached to this assembler
     *
     * @return
     */
    public int preprocess(int recLevel) {
        int i = 0;
        int recursionRetVal;

        PLPAsmSource topLevelAsm = (PLPAsmSource) SourceList.get(recLevel);
        curActiveFile = topLevelAsm.asmFilePath;

        String delimiters = "[ ]+";
        String[] asmTokens = topLevelAsm.asmString.split(delimiters);

        // Begin our preprocess cases
        while(i < asmTokens.length) {
            if(asmTokens[i].equals(".include")) {
                i++;
                PLPAsmSource childAsm = new PLPAsmSource
                                            (null, asmTokens[i], recLevel + 1);
                SourceList.add(childAsm);
                recursionRetVal = this.preprocess(recLevel + 1);

                if(recursionRetVal != 0)
                    return recursionRetVal;
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