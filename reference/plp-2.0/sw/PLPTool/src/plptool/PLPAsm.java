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

    private int[]       addrTable;
    private int[]       objectCode;
    private int         curAddr;
    private int         labelCount;
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

        labelCount = 0;
        topLevelFile = strFilePath;

        assembled = false;

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

        // Assembler directives
        instrMap.put(new String("ASM__WORD__"), new Integer(9));
        instrMap.put(new String("ASM__ORG__"),  new Integer(9));
        instrMap.put(new String("ASM__SKIP__"), new Integer(9));

        // Instruction opcodes
        opcode.put(new String("add")   , new Byte((byte) 0x20));
        opcode.put(new String("addu")  , new Byte((byte) 0x21));
        opcode.put(new String("and")   , new Byte((byte) 0x24));
        opcode.put(new String("jr")    , new Byte((byte) 0x08));
        opcode.put(new String("nor")   , new Byte((byte) 0x27));
        opcode.put(new String("or")    , new Byte((byte) 0x25));
        opcode.put(new String("slt")   , new Byte((byte) 0x2A));
        opcode.put(new String("sltu")  , new Byte((byte) 0x2B));
        opcode.put(new String("sll")   , new Byte((byte) 0x00));
        opcode.put(new String("srl")   , new Byte((byte) 0x02));
        opcode.put(new String("sub")   , new Byte((byte) 0x22));
        opcode.put(new String("subu")  , new Byte((byte) 0x23));

        opcode.put(new String("addi")  , new Byte((byte) 0x08));
        opcode.put(new String("addiu") , new Byte((byte) 0x09));
        opcode.put(new String("andi")  , new Byte((byte) 0x0C));
        opcode.put(new String("beq")   , new Byte((byte) 0x04));
        opcode.put(new String("bne")   , new Byte((byte) 0x05));
        opcode.put(new String("lui")   , new Byte((byte) 0x0F));
        opcode.put(new String("ori")   , new Byte((byte) 0x0D));
        opcode.put(new String("slti")  , new Byte((byte) 0x0A));
        opcode.put(new String("sltiu") , new Byte((byte) 0x0B));
        opcode.put(new String("lw")    , new Byte((byte) 0x23));
        opcode.put(new String("sw")    , new Byte((byte) 0x2B));

        opcode.put(new String("j")     , new Byte((byte) 0x02));
        opcode.put(new String("jal")   , new Byte((byte) 0x03));

        opcode.put(new String("ASM__ORG__")           , new Byte((byte) 0xff));
        opcode.put(new String("ASM__WORD__")          , new Byte((byte) 0xff));
        opcode.put(new String("ASM__LABEL__")         , new Byte((byte) 0xff));
        opcode.put(new String("ASM__DIRECTIVE__")     , new Byte((byte) 0xff));

        // Registers
        regs.put(new String("$0")  , new Byte((byte) 1));
        regs.put(new String("$1")  , new Byte((byte) 1));
        regs.put(new String("$2")  , new Byte((byte) 2));
        regs.put(new String("$3")  , new Byte((byte) 3));
        regs.put(new String("$4")  , new Byte((byte) 4));
        regs.put(new String("$5")  , new Byte((byte) 5));
        regs.put(new String("$6")  , new Byte((byte) 6));
        regs.put(new String("$7")  , new Byte((byte) 7));
        regs.put(new String("$8")  , new Byte((byte) 8));
        regs.put(new String("$9")  , new Byte((byte) 9));
        regs.put(new String("$10") , new Byte((byte) 10));
        regs.put(new String("$11") , new Byte((byte) 11));
        regs.put(new String("$12") , new Byte((byte) 12));
        regs.put(new String("$13") , new Byte((byte) 13));
        regs.put(new String("$14") , new Byte((byte) 14));
        regs.put(new String("$15") , new Byte((byte) 15));
        regs.put(new String("$16") , new Byte((byte) 16));
        regs.put(new String("$17") , new Byte((byte) 17));
        regs.put(new String("$18") , new Byte((byte) 18));
        regs.put(new String("$19") , new Byte((byte) 19));
        regs.put(new String("$20") , new Byte((byte) 20));
        regs.put(new String("$21") , new Byte((byte) 21));
        regs.put(new String("$22") , new Byte((byte) 22));
        regs.put(new String("$23") , new Byte((byte) 23));
        regs.put(new String("$24") , new Byte((byte) 24));
        regs.put(new String("$25") , new Byte((byte) 25));
        regs.put(new String("$26") , new Byte((byte) 26));
        regs.put(new String("$27") , new Byte((byte) 27));
        regs.put(new String("$28") , new Byte((byte) 28));
        regs.put(new String("$29") , new Byte((byte) 29));
        regs.put(new String("$30") , new Byte((byte) 30));
        regs.put(new String("$31") , new Byte((byte) 31));

        regs.put(new String("$zero") , new Byte((byte) 0));
        regs.put(new String("$at")   , new Byte((byte) 1));
        regs.put(new String("$v0")   , new Byte((byte) 2));
        regs.put(new String("$v1")   , new Byte((byte) 3));
        regs.put(new String("$a0")   , new Byte((byte) 4));
        regs.put(new String("$a1")   , new Byte((byte) 5));
        regs.put(new String("$a2")   , new Byte((byte) 6));
        regs.put(new String("$a3")   , new Byte((byte) 7));
        regs.put(new String("$t0")   , new Byte((byte) 8));
        regs.put(new String("$t1")   , new Byte((byte) 9));
        regs.put(new String("$t2")   , new Byte((byte) 10));
        regs.put(new String("$t3")   , new Byte((byte) 11));
        regs.put(new String("$t4")   , new Byte((byte) 12));
        regs.put(new String("$t5")   , new Byte((byte) 13));
        regs.put(new String("$t6")   , new Byte((byte) 14));
        regs.put(new String("$t7")   , new Byte((byte) 15));
        regs.put(new String("$s0")   , new Byte((byte) 16));
        regs.put(new String("$s1")   , new Byte((byte) 17));
        regs.put(new String("$s2")   , new Byte((byte) 18));
        regs.put(new String("$s3")   , new Byte((byte) 19));
        regs.put(new String("$s4")   , new Byte((byte) 20));
        regs.put(new String("$s5")   , new Byte((byte) 21));
        regs.put(new String("$s6")   , new Byte((byte) 22));
        regs.put(new String("$s7")   , new Byte((byte) 23));
        regs.put(new String("$t8")   , new Byte((byte) 24));
        regs.put(new String("$t9")   , new Byte((byte) 25));
        regs.put(new String("$k0")   , new Byte((byte) 26));
        regs.put(new String("$k1")   , new Byte((byte) 27));
        regs.put(new String("$gp")   , new Byte((byte) 28));
        regs.put(new String("$sp")   , new Byte((byte) 29));
        regs.put(new String("$fp")   , new Byte((byte) 30));
        regs.put(new String("$ra")   , new Byte((byte) 31));

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

        // Begin our preprocess cases
        while(i < asmLines.length) {
            j = 0;
            asmTokens = asmLines[i].split(delimiters);
            i++;

            // Include statement
            if(asmTokens[0].equals(".include")) {
                if(asmTokens.length < 2) {
                   PLPMsg.PLPError("Directive syntax error in line " + i,
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

                if(recursionRetVal != 0)
                    return recursionRetVal;
            }

            // .org directive
            else if(asmTokens[0].equals(".org")) {
                if(asmTokens.length < 2) {
                   PLPMsg.PLPError("Directive syntax error in line " + i,
                                  PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX, this);
                   return PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX;
                }

                preprocessedAsm += "ASM__ORG__ " + asmTokens[1] + "\n";

                curAddr = sanitize32bits(asmTokens[1]);
            }

            // .word directive:
            //   Initialize current memory address to a value
            else if(asmTokens[0].equals(".word")) {
                if(asmTokens.length < 2) {
                   PLPMsg.PLPError("Directive syntax error in line " + i,
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
                   PLPMsg.PLPError("Directive syntax error in line " + i,
                                  PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX, this);
                   return PLPMsg.PLP_ASM_ERROR_DIRECTIVE_SYNTAX;
                }
                
                for(j = 0; j < Integer.parseInt(asmTokens[1]); j++) {
                    preprocessedAsm += "ASM__WORD__ 0\n";
                    curAddr += 4;
                }
            }
            
            // Label handler
            //   Everything after the label is IGNORED, it has to be on its own
            //   line
            else if(asmTokens[0].charAt(asmTokens[0].length() - 1) == ':')
            {
                tempLabel = asmTokens[0].substring(0, asmTokens[0].length() - 1);
                symTable.put(tempLabel, new Integer(curAddr));
                preprocessedAsm += "ASM__SKIP__";
            }

            // Pseudo-ops

            // Comments
            else if(asmTokens[0].charAt(0) == '#') {
                preprocessedAsm += "ASM__SKIP__";
            }

            // Instructions
            else {
                if(instrMap.containsKey(asmTokens[0]) == false) {
                    PLPMsg.PLPError("Unable to process token " + asmTokens[0],
                                    PLPMsg.PLP_ASM_ERROR_INVALID_TOKEN, this);
                    return PLPMsg.PLP_ASM_ERROR_INVALID_TOKEN;
                }
                curAddr += 4;
                preprocessedAsm += asmLines[i - 1] + "\n";
            }
        }

        } catch(Exception e) {
            PLPMsg.PLPError("preprocess(): Uncaught exception in line " + i + "\n" + e,
                            PLPMsg.PLP_ERROR_GENERIC, this);
            return PLPMsg.PLP_ERROR_GENERIC;
        }

        PLPMsg.PLPDebug("First pass completed.", 1, this);

        return 0;
    }

    /**
     * Assemble all assembly sources attached to this assembler and generate
     * object codes
     *
     * @return
     */
    public int assemble() {
        int i = 0, j = 0;
        int asmPC = 0;
        int lineNumOffset = 1;
        
        String delimiters = "[(), +]";
        String lineDelim  = "\\r?\\n";

        String[] asmLines  = this.preprocessedAsm.split(lineDelim);
        String[] asmTokens;

        byte rd, rs, rt, shamt;
        int imm;
        int branchTarget;

        boolean org;

        objectCode = new int[asmLines.length];
        addrTable = new int[asmLines.length];
        curActiveFile = this.topLevelFile;

        try {

        while(i < asmLines.length) {
            asmTokens = asmLines[i].split(delimiters);
            objectCode[i] = 0;
            org = false;
            rd = rs = rt = shamt = -1;

            PLPMsg.PLPDebug("assemble(line " + (i + lineNumOffset) + "): " + asmLines[i], 5, this);

            switch((Integer) instrMap.get(asmTokens[0])) {

                // 3-op R-type
                case 0:
                    if(!checkNumberOfOperands(asmTokens, 4, i))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[2]) ||
                       !regs.containsKey(asmTokens[3])) {
                        PLPMsg.PLPError("assemble(): Invalid register in line " + (lineNumOffset + 1),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER;
                    }

                    objectCode[i] |= (rs = (Byte) regs.get(asmTokens[2])) << 21;
                    objectCode[i] |= (rt = (Byte) regs.get(asmTokens[3])) << 16;
                    objectCode[i] |= (rd = (Byte) regs.get(asmTokens[1])) << 11;
                    objectCode[i] |= (Byte) opcode.get(asmTokens[0]);

                    break;

                // Shift R-type
                case 1:
                    if(!checkNumberOfOperands(asmTokens, 3, i))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[2])) {
                        PLPMsg.PLPError("assemble(): Invalid register in line " + (lineNumOffset + 1),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER;
                    }
                    objectCode[i] |= (rt = (Byte) regs.get(asmTokens[2])) << 16;
                    objectCode[i] |= (rd = (Byte) regs.get(asmTokens[1])) << 11;
                    objectCode[i] |= (shamt = (byte) ((Byte) regs.get(asmTokens[3]) & 0x1F)) << 6;
                    objectCode[i] |= (Byte) opcode.get(asmTokens[0]);

                    break;

                // Jump R-type
                case 2:
                    if(!checkNumberOfOperands(asmTokens, 2, i))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1])) {
                        PLPMsg.PLPError("assemble(): Invalid register in line " + (lineNumOffset + 1),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER;
                    }
                    objectCode[i] |= (rs = (Byte) regs.get(asmTokens[1])) << 21;
                    objectCode[i] |= (Byte) opcode.get(asmTokens[0]);

                    break;

                // Branch I-type
                case 3:
                    if(!checkNumberOfOperands(asmTokens, 4, i))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1])) {
                        PLPMsg.PLPError("assemble(): Invalid register in line " + (lineNumOffset + 1),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER;
                    }
                    if(!symTable.containsKey(asmTokens[3])) {
                        PLPMsg.PLPError("assemble(): Invalid branch target in line " + (lineNumOffset + 1),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_BRANCH_TARGET, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_BRANCH_TARGET;
                    }
                    branchTarget = (Integer) symTable.get(asmTokens[3]);
                    branchTarget -= (asmPC + 4) / 4;
                    objectCode[i] |= branchTarget & 0xFFFF;
                    objectCode[i] |= (rt = (Byte) regs.get(asmTokens[2])) << 21;
                    objectCode[i] |= (rs = (Byte) regs.get(asmTokens[1])) << 16;
                    objectCode[i] |= (Byte) opcode.get(asmTokens[0]) << 26;

                    break;

                // Arithmetic and Logic I-type
                case 4:
                    if(!checkNumberOfOperands(asmTokens, 4, i))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[2])) {
                        PLPMsg.PLPError("assemble(): Invalid register in line " + (lineNumOffset + 1),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER;
                    }
                    objectCode[i] |= sanitize16bits(asmTokens[3]);
                    objectCode[i] |= (rt = (Byte) regs.get(asmTokens[1])) << 16;
                    objectCode[i] |= (rs = (Byte) regs.get(asmTokens[2])) << 21;
                    objectCode[i] |= (Byte) opcode.get(asmTokens[0]) << 26;

                    break;

                // Load upper immediate I-type
                case 5:
                    if(!checkNumberOfOperands(asmTokens, 3, i))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1])) {
                        PLPMsg.PLPError("assemble(): Invalid register in line " + (lineNumOffset + 1),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER;
                    }
                    objectCode[i] |= sanitize16bits(asmTokens[2]);
                    objectCode[i] |= (rt = (Byte) regs.get(asmTokens[1])) << 16;
                    objectCode[i] |= (Byte) opcode.get(asmTokens[0]) << 26;

                    break;

                // Load/Store Word I-type
                case 6:
                    if(!checkNumberOfOperands(asmTokens, 4, i))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[3])) {
                        PLPMsg.PLPError("assemble(): Invalid register in line " + (lineNumOffset + 1),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_REGISTER;
                    }
                    objectCode[i] |= sanitize16bits(asmTokens[3]);
                    objectCode[i] |= (rt = (Byte) regs.get(asmTokens[1])) << 16;
                    objectCode[i] |= (rs = (Byte) regs.get(asmTokens[3])) << 21;
                    objectCode[i] |= (Byte) opcode.get(asmTokens[0]) << 26;

                    break;

                // J-type
                case 7:
                    if(!checkNumberOfOperands(asmTokens, 2, i))
                        return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                    if(!symTable.containsKey(asmTokens[1])) {
                        PLPMsg.PLPError("assemble(): Invalid jump target in line " + (lineNumOffset + 1),
                                        PLPMsg.PLP_ASM_ERROR_INVALID_JUMP_TARGET, this);
                        return PLPMsg.PLP_ASM_ERROR_INVALID_JUMP_TARGET;
                    }

                    objectCode[i] |= ((Integer) symTable.get(asmTokens[1]) >> 2) & 0x3FFFFFF;
                    objectCode[i] |= (Byte) opcode.get(asmTokens[0]) << 26;

                    break;

                // Multiplication Intsructions
                case 8:
                    break;

                // Others
                case 9:
                    if(asmTokens[0].equals("ASM__WORD__")) {
                        if(!checkNumberOfOperands(asmTokens, 2, i))
                            return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                        objectCode[i] = sanitize32bits(asmTokens[1]);
                    }
                    else if(asmTokens[0].equals("ASM__ORG__")) {
                        if(!checkNumberOfOperands(asmTokens, 2, i))
                            return PLPMsg.PLP_ASM_ERROR_NUMBER_OF_OPERANDS;

                        asmPC = sanitize32bits(asmTokens[1]);
                        org = true;
                        i--;
                        lineNumOffset++;
			asmLines[i] = "ASM__SKIP__";
                    } else {
	            }

                    break;
                default:
                    PLPMsg.PLPError("assemble(): This should not happen. Report bug.",
                                     PLPMsg.PLP_OOPS, this);
                    return PLPMsg.PLP_OOPS;
            }

            if(!org) {
                addrTable[i] = asmPC;
                asmPC += 4;
            }
            i++;
        }

        if(PLPMsg.lastError > 0)
            PLPMsg.PLPInfo("assemble(): WARNING: Unhandled error(s) encountered.", this);
        
        PLPMsg.PLPDebug("Assembly completed.", 1, this);
        assembled = true;

        } catch(Exception e) {
            PLPMsg.PLPError("assemble(): Uncaught exception in line " + (i + 1) + "\n" + e,
                            PLPMsg.PLP_ERROR_GENERIC, this);
            return PLPMsg.PLP_ERROR_GENERIC;
        }

        return 0;
    }

    public int[] getObjectCode() {
        return objectCode;
    }

    public int[] getAddrTable() {
        return addrTable;
    }
    
    public HashMap getSymTable() {
        return symTable;
    }

    public boolean isAssembled() {
        return assembled;
    }

    private short sanitize16bits(String number) {
        try {

        if(number.startsWith("0x") || number.startsWith("0h")) {
            number = number.substring(2);
            return Short.parseShort(number, 16);
        }
        else if(number.startsWith("0b")) {
            number = number.substring(2);
            return Short.parseShort(number, 2);
        }
        else
            return Short.parseShort(number);

        } catch(Exception e) {
            PLPMsg.PLPError("Argument is not a valid number\n" + e,
                            PLPMsg.PLP_NUMBER_ERROR, this);
            return 0;
        }
    }

    private int sanitize32bits(String number) {
        try {

        if(number.startsWith("0x") || number.startsWith("0h")) {
            number = number.substring(2);
            return Integer.parseInt(number, 16);
        }
        else if(number.startsWith("0b")) {
            number = number.substring(2);
            return Integer.parseInt(number, 2);
        }
        else
            return Integer.parseInt(number);

        } catch(Exception e) {
            PLPMsg.PLPError("Argument is not a valid number\n" + e,
                            PLPMsg.PLP_NUMBER_ERROR, this);
            return 0;
        }
    }

    private boolean checkNumberOfOperands(Object iObj[], int length, int lineNum) {
        if(iObj.length != length) {
            PLPMsg.PLPError("assemble(): Invalid number of operands in line " + (lineNum + 1),
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
            PLPMsg.PLPError("Error reading file",
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
