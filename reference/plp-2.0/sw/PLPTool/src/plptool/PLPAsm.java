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

    private LinkedList  SourceList = new LinkedList();

    private long[]      addrTable;
    private long[]      objectCode;      // Java needs unsigned types!
    private long        curAddr;
    private int         directiveOffset;
    private String      preprocessedAsm;
    private String      curActiveFile;
    private String      topLevelFile;

    private HashMap     instrMap;
    private HashMap     symTable;
    private HashMap     opcode;
    private HashMap     regs;

    private boolean     assembled;

    public PLPAsm (String strAsm, String strFilePath, int intStartAddr) {
        PLPAsmSource plpAsmObj = new PLPAsmSource(strAsm, strFilePath, 0);
        SourceList.add(plpAsmObj);
        preprocessedAsm = new String();
        curAddr = intStartAddr;
        instrMap = new HashMap();
        symTable = new HashMap();
        opcode = new HashMap();
        regs = new HashMap();

        directiveOffset = 0;
        topLevelFile = strFilePath;

        assembled = false;

        // R-type Arithmetic and Logical instructions
        instrMap.put("addu", new Integer(0));
        instrMap.put("subu", new Integer(0));
        instrMap.put("and",  new Integer(0));
        instrMap.put("or",   new Integer(0));
        instrMap.put("nor",  new Integer(0));
        instrMap.put("slt",  new Integer(0));
        instrMap.put("sltu", new Integer(0));

        // R-type Shift instructions
        instrMap.put("sll",  new Integer(1));
        instrMap.put("srl",  new Integer(1));

        // R-type Jump instructions
        instrMap.put("jr",   new Integer(2));
        instrMap.put("jalr", new Integer(2));

        // I-type Branch instructions
        instrMap.put("beq",  new Integer(3));
        instrMap.put("bne",  new Integer(3));

        // I-type Arithmetic and Logical instructions
        instrMap.put("addiu", new Integer(4));
        instrMap.put("andi",  new Integer(4));
        instrMap.put("ori",   new Integer(4));
        instrMap.put("slti",  new Integer(4));
        instrMap.put("sltiu", new Integer(4));

        // I-type Load Upper Immediate instruction
        instrMap.put("lui",  new Integer(5));

        // I-type Load and Store word instructions
        instrMap.put("lw",   new Integer(6));
        instrMap.put("sw",   new Integer(6));

        // J-type Instructions
        instrMap.put("j",    new Integer(7));
        instrMap.put("jal",  new Integer(7));

        // Multiply instructions
        instrMap.put("multu", new Integer(8));
        instrMap.put("mfhi",  new Integer(8));
        instrMap.put("mflo",  new Integer(8));

        // Assembler directives
        instrMap.put("ASM__WORD__", new Integer(9));
        instrMap.put("ASM__ORG__",  new Integer(9));
        instrMap.put("ASM__SKIP__", new Integer(9));
        instrMap.put("ASM__LINE__", new Integer(9));

        // Instruction opcodes
        opcode.put("add"   , new Byte((byte) 0x20));
        opcode.put("addu"  , new Byte((byte) 0x21));
        opcode.put("and"   , new Byte((byte) 0x24));
        opcode.put("jr"    , new Byte((byte) 0x08));
        opcode.put("nor"   , new Byte((byte) 0x27));
        opcode.put("or"    , new Byte((byte) 0x25));
        opcode.put("slt"   , new Byte((byte) 0x2A));
        opcode.put("sltu"  , new Byte((byte) 0x2B));
        opcode.put("sll"   , new Byte((byte) 0x00));
        opcode.put("srl"   , new Byte((byte) 0x02));
        opcode.put("sub"   , new Byte((byte) 0x22));
        opcode.put("subu"  , new Byte((byte) 0x23));

        opcode.put("addi"  , new Byte((byte) 0x08));
        opcode.put("addiu" , new Byte((byte) 0x09));
        opcode.put("andi"  , new Byte((byte) 0x0C));
        opcode.put("beq"   , new Byte((byte) 0x04));
        opcode.put("bne"   , new Byte((byte) 0x05));
        opcode.put("lui"   , new Byte((byte) 0x0F));
        opcode.put("ori"   , new Byte((byte) 0x0D));
        opcode.put("slti"  , new Byte((byte) 0x0A));
        opcode.put("sltiu" , new Byte((byte) 0x0B));
        opcode.put("lw"    , new Byte((byte) 0x23));
        opcode.put("sw"    , new Byte((byte) 0x2B));

        opcode.put("j"     , new Byte((byte) 0x02));
        opcode.put("jal"   , new Byte((byte) 0x03));

        opcode.put("ASM__ORG__"           , new Byte((byte) 0xff));
        opcode.put("ASM__WORD__"          , new Byte((byte) 0xff));
        opcode.put("ASM__LABEL__"         , new Byte((byte) 0xff));
        opcode.put("ASM__DIRECTIVE__"     , new Byte((byte) 0xff));

        // Registers
        regs.put("$0"  , new Byte((byte) 1));
        regs.put("$1"  , new Byte((byte) 1));
        regs.put("$2"  , new Byte((byte) 2));
        regs.put("$3"  , new Byte((byte) 3));
        regs.put("$4"  , new Byte((byte) 4));
        regs.put("$5"  , new Byte((byte) 5));
        regs.put("$6"  , new Byte((byte) 6));
        regs.put("$7"  , new Byte((byte) 7));
        regs.put("$8"  , new Byte((byte) 8));
        regs.put("$9"  , new Byte((byte) 9));
        regs.put("$10" , new Byte((byte) 10));
        regs.put("$11" , new Byte((byte) 11));
        regs.put("$12" , new Byte((byte) 12));
        regs.put("$13" , new Byte((byte) 13));
        regs.put("$14" , new Byte((byte) 14));
        regs.put("$15" , new Byte((byte) 15));
        regs.put("$16" , new Byte((byte) 16));
        regs.put("$17" , new Byte((byte) 17));
        regs.put("$18" , new Byte((byte) 18));
        regs.put("$19" , new Byte((byte) 19));
        regs.put("$20" , new Byte((byte) 20));
        regs.put("$21" , new Byte((byte) 21));
        regs.put("$22" , new Byte((byte) 22));
        regs.put("$23" , new Byte((byte) 23));
        regs.put("$24" , new Byte((byte) 24));
        regs.put("$25" , new Byte((byte) 25));
        regs.put("$26" , new Byte((byte) 26));
        regs.put("$27" , new Byte((byte) 27));
        regs.put("$28" , new Byte((byte) 28));
        regs.put("$29" , new Byte((byte) 29));
        regs.put("$30" , new Byte((byte) 30));
        regs.put("$31" , new Byte((byte) 31));

        regs.put("$zero" , new Byte((byte) 0));
        regs.put("$at"   , new Byte((byte) 1));
        regs.put("$v0"   , new Byte((byte) 2));
        regs.put("$v1"   , new Byte((byte) 3));
        regs.put("$a0"   , new Byte((byte) 4));
        regs.put("$a1"   , new Byte((byte) 5));
        regs.put("$a2"   , new Byte((byte) 6));
        regs.put("$a3"   , new Byte((byte) 7));
        regs.put("$t0"   , new Byte((byte) 8));
        regs.put("$t1"   , new Byte((byte) 9));
        regs.put("$t2"   , new Byte((byte) 10));
        regs.put("$t3"   , new Byte((byte) 11));
        regs.put("$t4"   , new Byte((byte) 12));
        regs.put("$t5"   , new Byte((byte) 13));
        regs.put("$t6"   , new Byte((byte) 14));
        regs.put("$t7"   , new Byte((byte) 15));
        regs.put("$s0"   , new Byte((byte) 16));
        regs.put("$s1"   , new Byte((byte) 17));
        regs.put("$s2"   , new Byte((byte) 18));
        regs.put("$s3"   , new Byte((byte) 19));
        regs.put("$s4"   , new Byte((byte) 20));
        regs.put("$s5"   , new Byte((byte) 21));
        regs.put("$s6"   , new Byte((byte) 22));
        regs.put("$s7"   , new Byte((byte) 23));
        regs.put("$t8"   , new Byte((byte) 24));
        regs.put("$t9"   , new Byte((byte) 25));
        regs.put("$k0"   , new Byte((byte) 26));
        regs.put("$k1"   , new Byte((byte) 27));
        regs.put("$gp"   , new Byte((byte) 28));
        regs.put("$sp"   , new Byte((byte) 29));
        regs.put("$fp"   , new Byte((byte) 30));
        regs.put("$ra"   , new Byte((byte) 31));

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
        String savedActiveFile;
        String tempLabel;

        try {

        PLPMsg.D("lines: " + asmLines.length, 5, this);

        // Begin our preprocess cases
        while(i < asmLines.length) {
            j = 0;
            asmLines[i] = asmLines[i].trim();
            asmTokens = asmLines[i].split(delimiters);

            PLPMsg.D(i + ": " + asmLines[i] + " tl: " +
                     asmTokens.length, 5, this);
            PLPMsg.D("<<<" + asmTokens[0] + ">>>", 5, this);

            i++;

            // Include statement
            if(asmTokens[0].equals(".include")) {
                if(asmTokens.length < 2) {
                   PLPMsg.E("Directive syntax error in line " + i,
                                  PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX, this);
                   return PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX;
                }

                preprocessedAsm += "ASM__SKIP__\n";

                PLPAsmSource childAsm = new PLPAsmSource
                                            (null, asmTokens[j+1], recLevel + 1);
                SourceList.add(childAsm);
                savedActiveFile = curActiveFile;
                recursionRetVal = this.preprocess(recLevel + 1);
                curActiveFile = savedActiveFile;
                directiveOffset++;

                if(recursionRetVal != 0)
                    return recursionRetVal;
            }

            // .org directive
            else if(asmTokens[0].equals(".org")) {
                if(asmTokens.length < 2) {
                   PLPMsg.E("Directive syntax error in line " + i,
                                  PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX, this);
                   return PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX;
                }

                preprocessedAsm += "ASM__ORG__ " + asmTokens[1] + "\n";
                directiveOffset++;
                curAddr = sanitize32bits(asmTokens[1]);
            }

            // .word directive:
            //   Initialize current memory address to a value
            else if(asmTokens[0].equals(".word")) {
                if(asmTokens.length < 2) {
                   PLPMsg.E("Directive syntax error in line " + i,
                                  PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX, this);
                   return PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX;
                }

                preprocessedAsm += "ASM__WORD__ " + asmTokens[1] + "\n";
                curAddr += 4;
            }

            // .space directive
            //   Assigns space for a variable, takes number of words as an
            //   argument
            else if(asmTokens[0].equals(".space")) {
                if(asmTokens.length < 2) {
                   PLPMsg.E("Directive syntax error in line " + i,
                                  PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX, this);
                   return PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX;
                }
                
                for(j = 0; j < Integer.parseInt(asmTokens[1]); j++) {
                    preprocessedAsm += "ASM__WORD__ 0\n";
                    curAddr += 4;
                }
            }

            // Comments
            else if(asmLines[i - 1].equals("") || asmTokens[0].charAt(0) == '#') {
                preprocessedAsm += "ASM__SKIP__\n";
                directiveOffset++;
            }
            
            // Label handler
            //   Everything after the label is IGNORED, it has to be on its own
            //   line
            else if(asmTokens[0].charAt(asmTokens[0].length() - 1) == ':') {
                tempLabel = asmTokens[0].substring(0, asmTokens[0].length() - 1);
                symTable.put(tempLabel, new Long(curAddr & 0xFFFFFFFF));
                preprocessedAsm += "ASM__SKIP__\n";
                directiveOffset++;
            }

            // Pseudo-ops
            else if(asmTokens[0].equals("nop")) {
                preprocessedAsm += "sll $0,$0,0\n";
                curAddr += 4;
            }

            // Instructions
            else {
                if(instrMap.containsKey(asmTokens[0]) == false) {
                    PLPMsg.E("Unable to process token " + asmTokens[0],
                                    PLPMsg.PLP_ASM_ERROR_INVALID_TOKEN, this);
                    return PLPMsg.PLP_ASM_ERROR_INVALID_TOKEN;
                }
                PLPMsg.D("exit i: " + i, 5, this);
                curAddr += 4;
                preprocessedAsm += asmLines[i - 1] + "\n";
            }

            PLPMsg.D("pr:\n" + preprocessedAsm + ">>>", 5, this);

        }

        } catch(Exception e) {
            PLPMsg.E("preprocess(): Uncaught exception in line " + i + "\n" + e,
                            PLPMsg.PLP_ERROR_GENERIC, this);
            return PLPMsg.PLP_ERROR_GENERIC;
        }

        PLPMsg.D("First pass completed.", 1, this);

        return 0;
    }

    /**cd PLP
     * Assemble all assembly sources attached to this assembler and generate
     * object codes
     *
     * @return
     */
    public int assemble() {
        int i = 0, j = 0;
        long asmPC = 0;
        int lineNumOffset = 1;
        int s = 0;              // assembler directive line offsets
        
        String delimiters = ",[ ]+|,|[ ]+|[()]";
        String lineDelim  = "\\r?\\n";

        String[] asmLines  = this.preprocessedAsm.split(lineDelim);
        String[] asmTokens;
        String[] stripComments;

        byte rd, rs, rt, shamt;
        int imm;
        int branchTarget;
        boolean skip;

        objectCode = new long[asmLines.length - directiveOffset];
        addrTable = new long[asmLines.length - directiveOffset];
        curActiveFile = this.topLevelFile;

        try {

        while(i < asmLines.length) {
            stripComments = asmLines[i].split("#");
            asmTokens = stripComments[0].split(delimiters);

            objectCode[i - s] = 0;
            skip = false;
            rd = rs = rt = shamt = -1;

            PLPMsg.D("assemble(line " + (i + lineNumOffset) + "): " + asmLines[i], 3, this);

            if(PLPMsg.debugLevel == 5)
                for(j = 0; j < asmTokens.length; j++)
                    PLPMsg.D(asmTokens[j], 5, this);

            switch((Integer) instrMap.get(asmTokens[0])) {

                // 3-op R-type
                case 0:
                    if(!checkNumberOfOperands(asmTokens, 4, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[2]) ||
                       !regs.containsKey(asmTokens[3])) {
                        PLPMsg.E("assemble(): Invalid register in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER;
                    }

                    objectCode[i - s] |= (rs = (Byte) regs.get(asmTokens[2])) << 21;
                    objectCode[i - s] |= (rt = (Byte) regs.get(asmTokens[3])) << 16;
                    objectCode[i - s] |= (rd = (Byte) regs.get(asmTokens[1])) << 11;
                    objectCode[i - s] |= (Byte) opcode.get(asmTokens[0]);

                    break;

                // Shift R-type
                case 1:
                    if(!checkNumberOfOperands(asmTokens, 4, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[2])) {
                        PLPMsg.E("assemble(): Invalid register in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER;
                    }

                    objectCode[i - s] |= (rt = (Byte) regs.get(asmTokens[2])) << 16;
                    objectCode[i - s] |= (rd = (Byte) regs.get(asmTokens[1])) << 11;
                    objectCode[i - s] |= (shamt = (byte) (Byte.parseByte(asmTokens[3]) & 0x1F)) << 6;
                    objectCode[i - s] |= (Byte) opcode.get(asmTokens[0]);

                    break;

                // Jump R-type
                case 2:
                    if(!checkNumberOfOperands(asmTokens, 2, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1])) {
                        PLPMsg.E("assemble(): Invalid register in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER;
                    }
                    objectCode[i - s] |= (rs = (Byte) regs.get(asmTokens[1])) << 21;
                    objectCode[i - s] |= (Byte) opcode.get(asmTokens[0]);

                    break;

                // Branch I-type
                case 3:
                    if(!checkNumberOfOperands(asmTokens, 4, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1])) {
                        PLPMsg.E("assemble(): Invalid register in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER;
                    }
                    if(!symTable.containsKey(asmTokens[3])) {
                        PLPMsg.E("assemble(): Invalid branch target in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_BRANCH_TARGET, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_BRANCH_TARGET;
                    }
                    branchTarget = (Integer) symTable.get(asmTokens[3]);
                    branchTarget -= (asmPC + 4) / 4;
                    objectCode[i - s] |= branchTarget & 0xFFFF;
                    objectCode[i - s] |= (rt = (Byte) regs.get(asmTokens[2])) << 21;
                    objectCode[i - s] |= (rs = (Byte) regs.get(asmTokens[1])) << 16;
                    objectCode[i - s] |= (Byte) opcode.get(asmTokens[0]) << 26;

                    break;

                // Arithmetic and Logic I-type
                case 4:
                    if(!checkNumberOfOperands(asmTokens, 4, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[2])) {
                        PLPMsg.E("assemble(): Invalid register in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER;
                    }
                    objectCode[i - s] |= sanitize16bits(asmTokens[3]);
                    objectCode[i - s] |= (rt = (Byte) regs.get(asmTokens[1])) << 16;
                    objectCode[i - s] |= (rs = (Byte) regs.get(asmTokens[2])) << 21;
                    objectCode[i - s] |= (Byte) opcode.get(asmTokens[0]) << 26;

                    break;

                // Load upper immediate I-type
                case 5:
                    if(!checkNumberOfOperands(asmTokens, 3, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1])) {
                        PLPMsg.E("assemble(): Invalid register in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER;
                    }
                    objectCode[i - s] |= sanitize16bits(asmTokens[2]);
                    objectCode[i - s] |= (rt = (Byte) regs.get(asmTokens[1])) << 16;
                    objectCode[i - s] |= (Byte) opcode.get(asmTokens[0]) << 26;

                    break;

                // Load/Store Word I-type
                case 6:
                    if(!checkNumberOfOperands(asmTokens, 4, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[3])) {
                        PLPMsg.E("assemble(): Invalid register in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER;
                    }
                    objectCode[i - s] |= sanitize16bits(asmTokens[3]);
                    objectCode[i - s] |= (rt = (Byte) regs.get(asmTokens[1])) << 16;
                    objectCode[i - s] |= (rs = (Byte) regs.get(asmTokens[3])) << 21;
                    objectCode[i - s] |= (Byte) opcode.get(asmTokens[0]) << 26;

                    break;

                // J-type
                case 7:
                    if(!checkNumberOfOperands(asmTokens, 2, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!symTable.containsKey(asmTokens[1])) {
                        PLPMsg.E("assemble(): Invalid jump target in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_JUMP_TARGET, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_JUMP_TARGET;
                    }

                    objectCode[i - s] |= ((Integer) symTable.get(asmTokens[1]) >> 2) & 0x3FFFFFF;
                    objectCode[i - s] |= (Byte) opcode.get(asmTokens[0]) << 26;

                    break;

                // Multiplication Intsructions
                case 8:
                    break;

                // Others
                case 9:
                    if(asmTokens[0].equals("ASM__WORD__")) {
                        if(!checkNumberOfOperands(asmTokens, 2, i + lineNumOffset))
                            return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                        objectCode[i - s] = sanitize32bits(asmTokens[1]);
                    }
                    else if(asmTokens[0].equals("ASM__ORG__")) {
                        if(!checkNumberOfOperands(asmTokens, 2, i + lineNumOffset))
                            return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                        asmPC = sanitize32bits(asmTokens[1]);
                        s++;
                        skip = true;
                    } else {
                        s++;
                        skip = true;
	            }

                    break;
                default:
                    PLPMsg.E("assemble(): This should not happen. Report bug.",
                                     PLPMsg.PLP_OOPS, this);
                    return PLPMsg.PLP_OOPS;
            }

            // Update address table and assembler PC if this line is a valid
            // instruction / .word directive
            if(!skip) {
                addrTable[i - s] = asmPC;
                asmPC += 4;
            }
            i++;
        }

        if(PLPMsg.lastError > 0)
            PLPMsg.I("assemble(): WARNING: Unhandled error(s) encountered.", this);
        
        PLPMsg.D("Assembly completed.", 1, this);
        assembled = true;

        } catch(Exception e) {
            PLPMsg.E("assemble(): Uncaught exception in line " +
                            (i + lineNumOffset) + "\n" + e,
                            PLPMsg.PLP_ERROR_GENERIC, this);
            return PLPMsg.PLP_ERROR_GENERIC;
        }

        return 0;
    }

    public long[] getObjectCode() {
        return objectCode;
    }

    public long[] getAddrTable() {
        return addrTable;
    }
    
    public HashMap getSymTable() {
        return symTable;
    }

    public boolean isAssembled() {
        return assembled;
    }

    private int sanitize16bits(String number) {
        try {

        if(number.startsWith("0x") || number.startsWith("0h")) {
            number = number.substring(2);
            return Integer.parseInt(number, 16) & 0xFFFF;
        }
        else if(number.startsWith("0b")) {
            number = number.substring(2);
            return Integer.parseInt(number, 2) & 0xFFFF;
        }
        else
            return Integer.parseInt(number) & 0xFFFF;

        } catch(Exception e) {
            PLPMsg.E("sanitize16bits(): Argument is not a valid number\n" + e,
                            PLPMsg.PLP_NUMBER_ERROR, this);
            return 0;
        }
    }

    private long sanitize32bits(String number) {
        try {

        if(number.startsWith("0x") || number.startsWith("0h")) {
            number = number.substring(2);
            return Long.parseLong(number, 16) & 0xFFFFFFFF;
        }
        else if(number.startsWith("0b")) {
            number = number.substring(2);
            return Long.parseLong(number, 2) & 0xFFFFFFFF;
        }
        else
            return Long.parseLong(number) & 0xFFFFFFFF;

        } catch(Exception e) {
            PLPMsg.E("sanitize32bits(): Argument is not a valid number\n" + e,
                            PLPMsg.PLP_NUMBER_ERROR, this);
            return 0;
        }
    }

    private boolean checkNumberOfOperands(Object iObj[], int length, int lineNum) {
        if(iObj.length != length) {
            PLPMsg.E("assemble(): Invalid number of operands in line " + lineNum,
                            PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS, this);
            return false;
        }
        return true;
    }

    @Override public String toString() {
        return "PLPAsm(" + this.curActiveFile + ")";
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
        asmString = new String();
        
        try {
        if(strAsm == null) {
            Scanner fScan = new Scanner(new File(strFilePath));
            while(fScan.hasNextLine())
                asmString += fScan.nextLine() + "\n";
            fScan.close();
        } else
            asmString = strAsm;

        asmFilePath = new String(strFilePath);
        recursionLevel = intLevel;

        } catch(Exception e) {
            PLPMsg.E("Error reading file",
                            PLPMsg.PLP_ERROR_GENERIC, this);
        }
    }

    public int setRefSource(PLPAsmSource plpRefSource) {
        refSource = plpRefSource;

        if(refSource != null)
            return 0;
        else {
            return PLPMsg.PLP_ASM_ERROR_INVALID_REFSOURCE;
        }
    }

    @Override public String toString() {
        return "PLPAsmSource(" + this.asmFilePath + ")";
    }

}
