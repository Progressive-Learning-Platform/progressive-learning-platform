package plptool;

import java.util.*;
import java.io.*;

/**
 * This class implements the modular PLP assembler object. The default
 * constructor will initialize required symbols for the assembler.
 * The assembler class can handle multiple assembly sources and
 * cross-references.
 *
 * @author wira
 */
public class PLPAsm {

    private LinkedList<PLPAsmSource>  SourceList = new LinkedList<PLPAsmSource>();

    /**
     * The address table attached to this assembler.
     *
     * @see objectCode[]
     */
    private long[]      addrTable;

    /**
     * The object code attached to this assembler.
     *
     * @see preprocess(int)
     * @see assemble()
     * @see getObjectCode()
     */
    private long[]      objectCode;      // Java needs unsigned types!
                                         // higher 4 bytes are useless, cast
                                         // to int before using.
    private HashMap<String, Long> symTable;

    private long        curAddr;
    private int         directiveOffset;
    private String      preprocessedAsm;
    private String      curActiveFile;
    private String      topLevelFile;

    private static HashMap<String, Integer>     instrMap;
    private static HashMap<String, Byte>        opcode;
    private static HashMap<String, Byte>        regs;

    private boolean     assembled;

    public PLPAsm (String strAsm, String strFilePath, int intStartAddr) {
        PLPAsmSource plpAsmObj = new PLPAsmSource(strAsm, strFilePath, 0);
        SourceList.add(plpAsmObj);
        preprocessedAsm = new String();
        curAddr = intStartAddr;
        instrMap = new HashMap<String, Integer>();
        symTable = new HashMap<String, Long>();
        opcode = new HashMap<String, Byte>();
        regs = new HashMap<String, Byte>();

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
        instrMap.put("ASM__LINE_OFFSET__", new Integer(9));
        instrMap.put("ASM__POINTER__", new Integer(9));
        
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

        // Registers
        regs.put("$0"  , new Byte((byte) 0));
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
     * attached to this assembler. Resolves assembler directives, pseudo-ops and
     * populates the symbol table.
     *
     * @return Returns 0 on completion, error code otherwise
     */
    public int preprocess(int recLevel) {
        int i = 0, j = 0;
        int recursionRetVal;

        PLPAsmSource topLevelAsm = (PLPAsmSource) SourceList.get(recLevel);
        curActiveFile = topLevelAsm.asmFilePath;

        String delimiters = ",[ ]+|,|[ ]+|[()]";
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
                   return PLPMsg.E("Directive syntax error in line " + i,
                                   PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX, this);
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
                   return PLPMsg.E("Directive syntax error in line " + i,
                                  PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX, this);
                }

                preprocessedAsm += "ASM__ORG__ " + asmTokens[1] + "\n";
                directiveOffset++;
                curAddr = sanitize32bits(asmTokens[1]);
            }

            // .word directive:
            //   Initialize current memory address to a value
            else if(asmTokens[0].equals(".word")) {
                if(asmTokens.length < 2) {
                   return PLPMsg.E("Directive syntax error in line " + i,
                                   PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX, this);
                }

                preprocessedAsm += "ASM__WORD__ " + asmTokens[1] + "\n";
                curAddr += 4;
            }

            // .space directive
            //   Assigns space for a variable, takes number of words as an
            //   argument
            else if(asmTokens[0].equals(".space")) {
                if(asmTokens.length < 2) {
                   return  PLPMsg.E("Directive syntax error in line " + i,
                                    PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX, this);
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
                if(symTable.containsKey(tempLabel)) {
                    return PLPMsg.E("Line " + i + ", label \"" + tempLabel + "\" already defined.",
                                    PLPMsg.PLP_ASM_ERROR_DUPLICATE_LABEL, this);
                }
                symTable.put(tempLabel, new Long((int) curAddr));
                preprocessedAsm += "ASM__SKIP__\n";
                directiveOffset++;
            }

            // Text handler
            else if(asmTokens[0].equals(".ascii") || asmTokens[0].equals(".asciiz")) {
                if(asmTokens.length < 2) {
                   return  PLPMsg.E("Directive syntax error in line " + i,
                                    PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX, this);
                }

                String tString[] = asmLines[i - 1].split("[ ]+", 2);

                PLPMsg.D("l: " + tString.length + " :" + tString[tString.length - 1], 5, this);
                
                if(tString[1].charAt(0) == '\"') {
                    tString[1] = tString[1].substring(1, tString[1].length());

                    if(tString[1].charAt(tString[1].length() - 1) != '\"')
                        return PLPMsg.E("Invalid string literal at line " + i,
                                        PLPMsg.PLP_ASM_ERROR_INVALID_STRING, this);

                    tString[1] = tString[1].substring(0, tString[1].length() - 1);
                }
                PLPMsg.D("pr: " + tString[1] + " l: " + tString[1].length(), 5, this);

                int strLen = tString[1].length() + ((asmTokens[0].equals(".asciiz")) ? 1 : 0);

                if(strLen > tString[1].length())
                    tString[1] += '\0';

                PLPMsg.D("pr: " + tString[1] + " l: " + tString[1].length(), 5, this);

                if(strLen % 4 != 0) {
                    strLen = strLen + 4 - (strLen % 4);
                    
                    for(j = 0; j < (4 - (strLen % 4)); j++)
                        tString[1] += '\0';
                }

                for(j = 0; j < strLen; j++) {
                    if(j % 4 == 0)
                        preprocessedAsm += "ASM__WORD__ 0x";

                    preprocessedAsm += String.format("%02x", (int) tString[1].charAt(j));

                    if((j + 1) % 4 == 0 && j > 0)
                        preprocessedAsm += "\n";
                }

                PLPMsg.D("pr: " + tString[1], 5, this);

            }

            // Pseudo-ops
            else if(asmTokens[0].equals("nop")) {
                preprocessedAsm += "sll $0,$0,0\n";
                curAddr += 4;
            }

            // load-immediate
            else if(asmTokens[0].equals("li")) {
                if(!checkNumberOfOperands(asmTokens, 3, i))
                    return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;
                
                preprocessedAsm += "lui " + asmTokens[1] + ",$_hi:" + asmTokens[2] + "\n";
                preprocessedAsm += "ori " + asmTokens[1] + "," + asmTokens[1] + ",$_lo:" + asmTokens[2] + "\n";
                curAddr += 8;
            }

            // Instructions
            else {
                if(instrMap.containsKey(asmTokens[0]) == false) {
                    return PLPMsg.E("Unable to process token " + asmTokens[0] + " in line " + i,
                                    PLPMsg.PLP_ASM_ERROR_INVALID_TOKEN, this);
                }
                PLPMsg.D("exit i: " + i, 5, this);
                curAddr += 4;
                preprocessedAsm += asmLines[i - 1] + "\n";
            }

            PLPMsg.D("pr:\n" + preprocessedAsm + ">>>", 10, this);

        }

        } catch(Exception e) {
            return PLPMsg.E("preprocess(): Uncaught exception in line " + i + "\n" + e,
                            PLPMsg.PLP_ERROR_GENERIC, this);
        }

        PLPMsg.D("First pass completed.", 1, this);

        return 0;
    }

    /**
     * Assemble all assembly sources attached to this assembler and generate
     * object codes. Populates objectCodes[] and addrTable[] and sets
     * the boolean assembled on successful execution.
     *
     * @return Returns 0 on completion, error code otherwise
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
        long branchTarget;
        boolean skip;

        objectCode = new long[asmLines.length - directiveOffset];
        addrTable = new long[asmLines.length - directiveOffset];
        curActiveFile = this.topLevelFile;

        // clear error
        PLPMsg.lastError = 0;

        try {

        while(i < asmLines.length) {
            stripComments = asmLines[i].split("#");
            asmTokens = stripComments[0].split(delimiters);

            objectCode[i - s] = 0;
            skip = false;
            rd = rs = rt = shamt = -1;

            PLPMsg.D("assemble(line " + (i + lineNumOffset) + "): " + asmLines[i], 3, this);

            // resolve symbols
            String tSymbol;
            int   tValue = 0;

            for(j = 0; j < asmTokens.length; j++) {

                PLPMsg.D(asmTokens[j] + " l:" + asmTokens[j].length(), 15, this);
                
                if(asmTokens[j].length() > 4)  {

                    PLPMsg.D(asmTokens[j].substring(0, 5), 15, this);

                    if(asmTokens[j].substring(0, 5).equals("$_hi:")) {

                        tSymbol = asmTokens[j].substring(5, asmTokens[j].length());

                        if(symTable.containsKey(tSymbol)) 
                            tValue = (int) (symTable.get(tSymbol) >> 16);
                        else
                            tValue = (int) (sanitize32bits(tSymbol) >> 16);

                        asmTokens[j] = new Integer(tValue).toString();

                    }
                    else if(asmTokens[j].substring(0, 5).equals("$_lo:")) {


                        tSymbol = asmTokens[j].substring(5, asmTokens[j].length());
                    
                        if(symTable.containsKey(tSymbol))
                            tValue = (int) (symTable.get(tSymbol) & 0xFFFF);
                        else
                            tValue = (int) (sanitize32bits(tSymbol) & 0xFFFF);

                        asmTokens[j] = new Integer(tValue).toString();

                    }

                    PLPMsg.D("pr: " + Integer.toHexString(tValue), 15, this);
                }
            }

            switch((Integer) instrMap.get(asmTokens[0])) {

                // 3-op R-type
                case 0:
                    if(!checkNumberOfOperands(asmTokens, 4, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[2]) ||
                       !regs.containsKey(asmTokens[3])) {
                        return PLPMsg.E("assemble(): Invalid register in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                    }

                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[2])) << 21;
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[3])) << 16;
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[1])) << 11;
                    objectCode[i - s] |= (Byte) opcode.get(asmTokens[0]);

                    break;

                // Shift R-type
                case 1:
                    if(!checkNumberOfOperands(asmTokens, 4, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[2])) {
                        return PLPMsg.E("assemble(): Invalid register in line " + (i + lineNumOffset),
                                         PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                    }

                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[2])) << 16;
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[1])) << 11;
                    objectCode[i - s] |= ((byte) (sanitize16bits(asmTokens[3]) & 0x1F)) << 6;
                    objectCode[i - s] |= (Byte) opcode.get(asmTokens[0]);

                    break;

                // Jump R-type
                case 2:
                    if(!checkNumberOfOperands(asmTokens, 2, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1])) {
                        return PLPMsg.E("assemble(): Invalid register in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                    }
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[1])) << 21;
                    objectCode[i - s] |= (Byte) opcode.get(asmTokens[0]);

                    break;

                // Branch I-type
                case 3:
                    if(!checkNumberOfOperands(asmTokens, 4, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1])) {
                        return PLPMsg.E("assemble(): Invalid register in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                    }
                    if(!symTable.containsKey(asmTokens[3])) {
                        return PLPMsg.E("assemble(): Invalid branch target in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_BRANCH_TARGET, this);
                    }
                    branchTarget = symTable.get(asmTokens[3]) - (asmPC + 4);
                    branchTarget /= 4;
                    objectCode[i - s] |= branchTarget & 0xFFFF;
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[2])) << 21;
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[1])) << 16;
                    objectCode[i - s] |= (Byte) opcode.get(asmTokens[0]) << 26;

                    break;

                // Arithmetic and Logic I-type
                case 4:
                    if(!checkNumberOfOperands(asmTokens, 4, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[2])) {
                        return PLPMsg.E("assemble(): Invalid register in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                    }
                    objectCode[i - s] |= sanitize16bits(asmTokens[3]);
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[1])) << 16;
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[2])) << 21;
                    objectCode[i - s] |= (Byte) opcode.get(asmTokens[0]) << 26;

                    break;

                // Load upper immediate I-type
                case 5:
                    if(!checkNumberOfOperands(asmTokens, 3, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1])) {
                        return PLPMsg.E("assemble(): Invalid register in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                    }
                    objectCode[i - s] |= sanitize16bits(asmTokens[2]);
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[1])) << 16;
                    objectCode[i - s] |= (Byte) opcode.get(asmTokens[0]) << 26;

                    break;

                // Load/Store Word I-type
                case 6:
                    if(!checkNumberOfOperands(asmTokens, 4, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[3])) {
                        return PLPMsg.E("assemble(): Invalid register in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                    }
                    objectCode[i - s] |= sanitize16bits(asmTokens[2]);
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[1])) << 16;
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[3])) << 21;
                    objectCode[i - s] |= (Byte) opcode.get(asmTokens[0]) << 26;

                    break;

                // J-type
                case 7:
                    if(!checkNumberOfOperands(asmTokens, 2, i + lineNumOffset))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!symTable.containsKey(asmTokens[1])) {
                        return PLPMsg.E("assemble(): Invalid jump target in line " + (i + lineNumOffset),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_JUMP_TARGET, this);
                    }

                    objectCode[i - s] |= (int) (symTable.get(asmTokens[1]) >> 2) & 0x3FFFFFF;
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
                    }
                    else {
                        s++;
                        skip = true;
	            }

                    break;

                // Pass-2 pseudo ops
                case 10:

                    break;
                    
                default:
                    return PLPMsg.E("assemble(): This should not happen. Report bug.",
                                     PLPMsg.PLP_OOPS, this);
            }

            if(PLPMsg.lastError > 0) {
                return PLPMsg.E("assemble(): Unhandled error(s) encountered in line " + (i + lineNumOffset) + ". Assembly failed.",
                                 PLPMsg.PLP_ERROR_GENERIC, this);
            }

            // Update address table and assembler PC if this line is a valid
            // instruction / .word directive
            if(!skip) {
                addrTable[i - s] = asmPC;
                asmPC += 4;
            }
            i++;
        }

        PLPMsg.D("Assembly completed.", 1, this);
        assembled = true;
        
        } catch(Exception e) {
            return PLPMsg.E("assemble(): Uncaught exception in line " +
                            (i + lineNumOffset) + "\n" + e,
                            PLPMsg.PLP_ERROR_GENERIC, this);
        }

        return PLPMsg.PLP_OK;
    }
    
    /**
     * Returns the object code array attached to this assembler object.
     * 
     * @return Returns the object code as array of longs.
     */
    public long[] getObjectCode() {
        return objectCode;
    }

    /**
     * Returns the instruction addresses array attached to this assembler object.
     *
     * @return Returns the address table as array of longs.
     */
    public long[] getAddrTable() {
        return addrTable;
    }

    /**
     * Returns the symbol table attached to this assembler object.
     *
     * @return Returns the symbol table as a HashMap.
     */
    public HashMap getSymTable() {
        return symTable;
    }

    public boolean isAssembled() {
        return assembled;
    }

    public static String lookupInstr(byte instrOpCode) {
        String key;

        if(opcode.containsValue(instrOpCode)) {
            Iterator iterator = opcode.keySet().iterator();
            while(iterator.hasNext()) {
                key = (String) iterator.next();
                if(opcode.get(key).equals(instrOpCode)) {
                    return key;
                }
            }
        }
        return null;
    }

    public String lookupLabel(long address) {
        String key;

        if(symTable.containsValue(address)) {
            Iterator iterator = symTable.keySet().iterator();
            while(iterator.hasNext()) {
                key = (String) iterator.next();
                if(symTable.get(key).equals(address)) {
                    return key;
                }
            }
        }
        return null;
    }

    public static Integer lookupInstrType(String instrOpCode) {
        if(instrMap.containsKey(instrOpCode)) {
            return (Integer) instrMap.get(instrOpCode);
        }
        return null;
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
            return PLPMsg.E("sanitize16bits(): Argument is not a valid number\n" + e,
                            PLPMsg.PLP_NUMBER_ERROR, this);
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
            return PLPMsg.E("sanitize32bits(): Argument is not a valid number\n" + e,
                            PLPMsg.PLP_NUMBER_ERROR, this);
        }
    }

    private boolean checkNumberOfOperands(Object iObj[], int length, int lineNum) {
        if(iObj.length != length) {
            PLPMsg.E("checkNumberOfOperands(): Invalid number of operands in line " + lineNum,
                            PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS, this);
            return false;
        }
        return true;
    }

    public LinkedList getAsmList() {
        return SourceList;
    }

    @Override public String toString() {
        return "PLPAsm(" + this.curActiveFile + ")";
    }
}

/**
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
        asmFilePath = strFilePath;
        
        try {
        if(strAsm == null) {
            Scanner fScan = new Scanner(new File(strFilePath));
            while(fScan.hasNextLine())
                asmString += fScan.nextLine() + "\n";
            fScan.close();
        } else
            asmString = strAsm;

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

class MIPSInstruction {

    int     imm;
    int     sa;
    int     rs;
    int     rd;
    int     rt;
    int     funct;
    int     opcode;
    String  strOpCode;
    char    type;
    int     plptype;
    long    instr;

    public MIPSInstruction(long instr) {
        this.instr = instr;
        imm     = (int) (instr & PLPMIPSEmu.consts.C_MASK);
        funct   = (int) (instr & PLPMIPSEmu.consts.V_MASK);
        sa      = (int) ((instr >> 5) & PLPMIPSEmu.consts.R_MASK);
        rd      = (int) ((instr >> 11) & PLPMIPSEmu.consts.R_MASK);
        rt      = (int) ((instr >> 16) & PLPMIPSEmu.consts.R_MASK);
        rs      = (int) ((instr >> 21) & PLPMIPSEmu.consts.R_MASK);
        opcode  = (int) ((instr >> 26) & PLPMIPSEmu.consts.V_MASK);
        strOpCode = PLPAsm.lookupInstr((byte) opcode);
    }

    public static int imm(long instr) {
        return (int) (instr & PLPMIPSEmu.consts.C_MASK); }

    public static int funct(long instr) {
        return (int) (instr & PLPMIPSEmu.consts.V_MASK); }

    public static int sa(long instr) {
        return (int) ((instr >> 5) & PLPMIPSEmu.consts.R_MASK);
    }

    public static int rd(long instr) {
        return (int) ((instr >> 11) & PLPMIPSEmu.consts.R_MASK);
    }

    public static int rt(long instr) {
        return (int) ((instr >> 16) & PLPMIPSEmu.consts.R_MASK);
    }

    public static int rs(long instr) {
        return (int) ((instr >> 21) & PLPMIPSEmu.consts.R_MASK);
    }

    public static int opcode(long instr) {
        return (int) ((instr >> 26) & PLPMIPSEmu.consts.V_MASK);
    }
}
