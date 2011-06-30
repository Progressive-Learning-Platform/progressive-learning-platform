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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import plptool.PLPAsmSource;
import plptool.Msg;
import plptool.Constants;


/**
 * This class implements the modular PLP assembler object. The default
 * constructor will initialize required symbols for the assembler.
 * The assembler class can handle multiple assembly sources and
 * cross-references.
 *
 * @author wira
 */
public class Asm extends plptool.PLPAsm {

    private int[]       entryType;

    private long        curAddr;
    private long        curTextAddr;
    private long        curDataAddr;
    private int         directiveOffset;
    private String      curActiveFile;
    private String      topLevelFile;

    private static HashMap<String, Integer>     instrMap;
    private static HashMap<String, Byte>        opcode;
    private static HashMap<String, Byte>        funct;
    private static HashMap<String, Byte>        regs;

    /**
     * Current active region being populated
     */
    private int         curRegion;

    /**
     * PLPAsm constructor. Reads file described by strFilePath if strAsm is
     * null, will attach strAsm to be assembled otherwise.
     *
     * @see preprocess(int)
     * @see assemble()
     * @param strAsm       String to assemble, null to load file instead
     * @param strFilePath  Top level assembly source to attach
     * @param intStartAddr Default starting address
     */
    public Asm (String strAsm, String strFilePath) {
        super(strAsm, strFilePath);

        curAddr = 0;
        curTextAddr = -1;
        curDataAddr = -1;
        entryPoint = -1;
        instrMap = new HashMap<String, Integer>();
        symTable = new HashMap<String, Long>();
        regionMap = new ArrayList<Integer>();
        opcode = new HashMap<String, Byte>();
        funct = new HashMap<String, Byte>();
        regs = new HashMap<String, Byte>();

        directiveOffset = 0;
        topLevelFile = strFilePath;


        defineArch();
    }

    public Asm (ArrayList<PLPAsmSource> asms) {
        super(asms);

        curAddr = 0;
        curTextAddr = -1;
        curDataAddr = -1;
        entryPoint = -1;
        instrMap = new HashMap<String, Integer>();
        symTable = new HashMap<String, Long>();
        regionMap = new ArrayList<Integer>();
        opcode = new HashMap<String, Byte>();
        funct = new HashMap<String, Byte>();
        regs = new HashMap<String, Byte>();

        directiveOffset = 0;
        topLevelFile = asms.get(0).getAsmFilePath();

        defineArch();
    }

    /**
     * Define instruction->opcode mappings, instruction type mapping and
     * register mappings.
     */
    private void defineArch() {
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

        // I-type Branch instructions
        instrMap.put("beq",  new Integer(3));
        instrMap.put("bne",  new Integer(3));

        // I-type Arithmetic and Logical instructions
        instrMap.put("addi",  new Integer(4));
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
        instrMap.put("mulhi",  new Integer(8));
        instrMap.put("mullo",  new Integer(8));

        // jalr Instruction
         instrMap.put("jalr", new Integer(9));

        // Assembler directives
        instrMap.put("ASM__WORD__", new Integer(10));
        instrMap.put("ASM__ORG__",  new Integer(10));
        instrMap.put("ASM__SKIP__", new Integer(10));
        instrMap.put("ASM__LINE_OFFSET__", new Integer(10));
        instrMap.put("ASM__POINTER__", new Integer(10));

        // Instruction opcodes
        //opcode.put("add"   , new Byte((byte) 0x20));
        funct.put("addu"  , new Byte((byte) 0x21));
        funct.put("and"   , new Byte((byte) 0x24));
        funct.put("jr"    , new Byte((byte) 0x08));
        funct.put("jalr"  , new Byte((byte) 0x09));
        funct.put("nor"   , new Byte((byte) 0x27));
        funct.put("or"    , new Byte((byte) 0x25));
        funct.put("slt"   , new Byte((byte) 0x2A));
        funct.put("sltu"  , new Byte((byte) 0x2B));
        funct.put("sll"   , new Byte((byte) 0x00));
        funct.put("srl"   , new Byte((byte) 0x02));
        //funct.put("sub"   , new Byte((byte) 0x22));
        funct.put("subu"  , new Byte((byte) 0x23));
	funct.put("mullo" , new Byte((byte) 0x10));
	funct.put("mulhi" , new Byte((byte) 0x11));

        opcode.put("_RTYPE", new Byte((byte) 0x00));
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
     * @param asmIndex asm source index to preprocess
     */
    public int preprocess(int index) {
        int i = 0, j = 0;
        int error = 0;
        int recursionRetVal;

        PLPAsmSource topLevelAsm = (PLPAsmSource) sourceList.get(index);
        curActiveFile = topLevelAsm.getAsmFilePath();
        asmIndex = index;

        if(index == 0)
            curRegion = 0;

        Msg.D("preprocess: splitting...", 2, this);

        String delimiters = "[ ,\t]+|[()]";
        String lineDelim  = "\\r?\\n";
        String[] asmLines  = topLevelAsm.getAsmString().split(lineDelim);
        String[] asmTokens;
        String savedActiveFile;
        String tempLabel;

        int prevAsmIndex;

        if(pass1Str == null)
            pass1Str = new StringBuilder(asmLines.length * 80);

        if(lineNumMap == null) {
            lineNumMap = new int[asmLines.length];
            asmFileMap = new int[asmLines.length];
        }

        try {

        Msg.D("pp(" + asmIndex + ") begin loop...", 2, this);
        Msg.D("lines: " + asmLines.length, 2, this);

        // Begin our preprocess cases
        while(i < asmLines.length) {
            j = 0;
            asmLines[i] = asmLines[i].trim();
            asmTokens = asmLines[i].split(delimiters);

            Msg.D(i + ": " + asmLines[i] + " tl: " +
                     asmTokens.length, 5, this);
            Msg.D("<<<" + asmTokens[0] + ">>>", 5, this);

            i++;

            // Include statement
            if(asmTokens[0].equals(".include")) {
                if(asmTokens.length < 2) {
                   error++; Msg.E("preprocess(" + curActiveFile + ":" + i + "): " +
                                     "Directive syntax error",
                                     Constants.PLP_ASM_DIRECTIVE_SYNTAX_ERROR, this);
                } else {

                appendPreprocessedAsm("ASM__SKIP__", i, true);
                boolean found = false;
                prevAsmIndex = asmIndex;
                recursionRetVal = 0;
                for(int k = 0; k < sourceList.size(); k++) {
                    if(asmTokens[1].equals(sourceList.get(k).getAsmFilePath())) {
                        asmIndex = k;
                        found = true;
                    }
                }
                if(!found) {
                    asmIndex = sourceList.size();
                    PLPAsmSource childAsm = new PLPAsmSource
                                                (null, asmTokens[1], asmIndex);
                    if(childAsm.getAsmString() != null) {
                        sourceList.add(childAsm);
                        found = true;
                    }
                }
                savedActiveFile = curActiveFile;
                if(found) { recursionRetVal = this.preprocess(asmIndex); }
                curActiveFile = savedActiveFile;
                asmIndex = prevAsmIndex;
                directiveOffset++;

                if(recursionRetVal != 0) {
                    Msg.errorCounter++;
                    return recursionRetVal;
                }
                }
            }

            // .org directive
            else if(asmTokens[0].equals(".org")) {
                if(asmTokens.length < 2) {
                   error++; Msg.E("preprocess(" + curActiveFile + ":" + i + "): " +
                                     "Directive syntax error",
                                     Constants.PLP_ASM_DIRECTIVE_SYNTAX_ERROR, this);
                } else {

                appendPreprocessedAsm("ASM__ORG__ " + asmTokens[1], i, true);
                directiveOffset++;
                curAddr = sanitize32bits(asmTokens[1]);
                if(entryPoint < 0)
                    entryPoint = curAddr;
                Msg.D("curAddr is now " + String.format("0x%08x", curAddr), 4, this);
                }
            }

            // .text directive
            else if(asmTokens[0].equals(".text")) {
                if(asmTokens.length < 1 || asmTokens.length > 2) {
                   error++; Msg.E("preprocess(" + curActiveFile + ":" + i + "): " +
                                     "Directive syntax error",
                                     Constants.PLP_ASM_DIRECTIVE_SYNTAX_ERROR, this);
                
                
                } else if(curRegion != 1) {
                    directiveOffset++;

                    if(curRegion == 2)
                        curDataAddr = curAddr;

                    curRegion = 1;
                    curAddr = curTextAddr;

                    if (asmTokens.length == 2) {
                        appendPreprocessedAsm("ASM__ORG__ " + asmTokens[1], i, true);
                        curAddr = sanitize32bits(asmTokens[1]);
                        entryPoint = curAddr;
                        curTextAddr = entryPoint;
                    } else {
                        appendPreprocessedAsm("ASM__ORG__ " +  String.format("0x%08x", curTextAddr), i, true);
                    }

                    if(curAddr < 0) {
                        error++; Msg.E("preprocess(" + curActiveFile + ":" + i + "): " +
                                     "Starting address for .text is not defined.",
                                     Constants.PLP_ASM_MEM_REGION_START_NOT_INIT, this);
                    }

                    Msg.D("curAddr is now " + String.format("0x%08x", curAddr), 4, this);
                }
            }

            // .data directive
            else if(asmTokens[0].equals(".data")) {
                if(asmTokens.length < 1 || asmTokens.length > 2) {
                   error++; Msg.E("preprocess(" + curActiveFile + ":" + i + "): " +
                                     "Directive syntax error",
                                     Constants.PLP_ASM_DIRECTIVE_SYNTAX_ERROR, this);
              
                } else if(curRegion != 2) {
                    directiveOffset++;

                    if(curRegion == 1)
                        curTextAddr = curAddr;
                    curRegion = 2;

                    curAddr = curDataAddr;

                    if (asmTokens.length == 2) {
                        appendPreprocessedAsm("ASM__ORG__ " + asmTokens[1], i, true);
                        curAddr = sanitize32bits(asmTokens[1]);
                        curDataAddr = curAddr;
                    } else {
                        appendPreprocessedAsm("ASM__ORG__ " +  String.format("0x%08x", curDataAddr), i, true);
                    }

                    if(curAddr < 0) {
                        error++; Msg.E("preprocess(" + curActiveFile + ":" + i + "): " +
                                     "Starting address for .data is not defined.",
                                     Constants.PLP_ASM_MEM_REGION_START_NOT_INIT, this);
                    }

                    Msg.D("curAddr is now " + String.format("0x%08x", curAddr), 4, this);
                }
            }

            // .word directive:
            //   Initialize current memory address to a value
            else if(asmTokens[0].equals(".word")) {
                if(asmTokens.length < 2) {
                   error++; Msg.E("preprocess(" + curActiveFile + ":" + i + "): " +
                                     "Directive syntax error",
                                     Constants.PLP_ASM_DIRECTIVE_SYNTAX_ERROR, this);
                } else {
                appendPreprocessedAsm("ASM__WORD__ " + asmTokens[1], i, true);
                regionMap.add(curRegion);
                curAddr += 4;
                }
            }

            // .space directive
            //   Assigns space for a variable, takes number of words as an
            //   argument
            else if(asmTokens[0].equals(".space")) {
                if(asmTokens.length < 2) {
                   error++; Msg.E("preprocess(" + curActiveFile + ":" + i + "): " +
                                     "Directive syntax error",
                                     Constants.PLP_ASM_DIRECTIVE_SYNTAX_ERROR, this);
                } else {
                
                for(j = 0; j < Integer.parseInt(asmTokens[1]); j++) {
                    appendPreprocessedAsm("ASM__WORD__ 0", i, true);
                    regionMap.add(curRegion);
                    curAddr += 4;
                }
                }
            }

            // Comments
            else if(asmLines[i - 1].equals("") || asmTokens[0].charAt(0) == '#') {
                appendPreprocessedAsm("ASM__SKIP__", i, true);
                directiveOffset++;
            }
            
            // Label handler
            //   Everything after the label is IGNORED, it has to be on its own
            //   line
            else if(asmTokens[0].charAt(asmTokens[0].length() - 1) == ':') {
                tempLabel = asmTokens[0].substring(0, asmTokens[0].length() - 1);
                if(symTable.containsKey(tempLabel)) {
                    error++; Msg.E("preprocess(" + curActiveFile + ":" + i + "): " +
                                      "label \"" + tempLabel + "\" already defined.",
                                      Constants.PLP_ASM_DUPLICATE_LABEL, this);
                } else {
                symTable.put(tempLabel, new Long((int) curAddr));
                appendPreprocessedAsm("ASM__SKIP__", i, true);
                directiveOffset++;
                }
            }

            // Text handler
            else if(asmTokens[0].equals(".ascii") || asmTokens[0].equals(".asciiz")) {
                if(asmTokens.length < 2) {
                   error++; Msg.E("preprocess(" + curActiveFile + ":" + i + "): " +
                                     "Directive syntax error",
                                     Constants.PLP_ASM_DIRECTIVE_SYNTAX_ERROR, this);
                } else {

                String tString[] = asmLines[i - 1].split("[ \t]+", 2);

                Msg.D("l: " + tString.length + " :" + tString[tString.length - 1], 5, this);

                // strip quotes
                if(tString[1].charAt(0) == '\"') {
                    tString[1] = tString[1].substring(1, tString[1].length());

                    if(tString[1].charAt(tString[1].length() - 1) != '\"') {
                        error++; Msg.E("preprocess(" + curActiveFile + ":" + i + "): "
                                          + "Invalid string literal.",
                                          Constants.PLP_ASM_INVALID_STRING, this);
                    }

                    tString[1] = tString[1].substring(0, tString[1].length() - 1);
                }
                Msg.D("pr: " + tString[1] + " l: " + tString[1].length(), 5, this);

                // check for escaped characters
                for(j = 0; j < tString[1].length(); j++) {
                    if(tString[1].charAt(j) == '\\' && j != tString[1].length() - 1) {
                        switch(tString[1].charAt(j + 1)) {

                            // Linefeed (0xA)
                            case 'n':
                                tString[1] = new StringBuffer(tString[1]).replace(j, j + 2, "\n").toString();
                                break;
                                
                            // Carriage return (0xD)
                            case 'r':
                                tString[1] = new StringBuffer(tString[1]).replace(j, j + 2, "\r").toString();
                                break;

                            // Tab
                            case 't':
                                tString[1] = new StringBuffer(tString[1]).replace(j, j + 2, "\t").toString();
                                break;

                            // Backslash
                            case '\\':
                                tString[1] = new StringBuffer(tString[1]).replace(j, j + 2, "\\").toString();
                                break;

                                
                            default:
                                Msg.W("preprocess(" + curActiveFile + ":" + i + "): "
                                        + "Unable to escape character \\" + tString[1].charAt(j + 1), this);
                        }
                    }
                }

                // check if we need to append a null character for .asciiz ...
                int strLen = tString[1].length() + ((asmTokens[0].equals(".asciiz")) ? 1 : 0);

                // ... and go ahead and do that if we do
                if(strLen > tString[1].length())
                    tString[1] += '\0';

                Msg.D("pr: " + tString[1] + " l: " + tString[1].length(), 5, this);

                // pad with zeroes if we the string length is not word-aligned
                if(strLen % 4 != 0) {
                    strLen = strLen + 4 - (strLen % 4);
                    
                    for(j = 0; j < (4 - (strLen % 4)); j++)
                        tString[1] += '\0';
                }

                // add ASM__WORD__ 2nd pass directives and we're done
                for(j = 0; j < strLen; j++) {
                    if(j % 4 == 0)
                        appendPreprocessedAsm("ASM__WORD__ 0x", i, false);

                    appendPreprocessedAsm(String.format("%02x", (int) tString[1].charAt(j)), i, false);

                    // advance address on every 4th byte (on next iteration)
                    if((j + 1) % 4 == 0 && j > 0) {
                        regionMap.add(curRegion);
                        curAddr += 4;
                        appendPreprocessedAsm("", i, true);
                    }
                }

                Msg.D("pr: " + tString[1], 5, this);
                }
            }

            // Pseudo-ops
            else if(asmTokens[0].equals("nop")) {
                appendPreprocessedAsm("sll $0,$0,0", i, true);
                regionMap.add(curRegion);
                curAddr += 4;
            }

            // copy register
            else if(asmTokens[0].equals("move")) {
                appendPreprocessedAsm("or " + asmTokens[1] + ",$0," + asmTokens[2], i, true);
                regionMap.add(curRegion);
                curAddr += 4;
            }

            // branch always
            else if(asmTokens[0].equals("b")) {
                appendPreprocessedAsm("beq $0,$0," + asmTokens[1], i, true);
                regionMap.add(curRegion);
                curAddr += 4;
            }

            // load-immediate
            else if(asmTokens[0].equals("li")) {
                appendPreprocessedAsm("lui " + asmTokens[1] + ",$_hi:" + asmTokens[2], i, true);
                appendPreprocessedAsm("ori " + asmTokens[1] + "," + asmTokens[1] + ",$_lo:" + asmTokens[2], i, true);
                regionMap.add(curRegion);
                regionMap.add(curRegion);
                curAddr += 8;
            }

            // push register onto stack
            else if(asmTokens[0].equals("push")) {
                appendPreprocessedAsm("sw " + asmTokens[1] + ", 0($sp)", i, true);
                appendPreprocessedAsm("addiu $sp, $sp, -4", i, true);
                regionMap.add(curRegion);
                regionMap.add(curRegion);
                curAddr += 8;
            }

            // pop register from stack
            else if(asmTokens[0].equals("pop")) {
                appendPreprocessedAsm("addiu $sp, $sp, 4", i, true);
                appendPreprocessedAsm("lw " + asmTokens[1] + ", 0($sp)", i, true);
                regionMap.add(curRegion);
                regionMap.add(curRegion);
                curAddr += 8;
            }

            // Instructions
            else {
                if(instrMap.containsKey(asmTokens[0]) == false) {
                    error ++; Msg.E("preprocess(" + curActiveFile + ":" + i +"): "
                                       + "Unable to process token " + asmTokens[0],
                                       Constants.PLP_ASM_INVALID_TOKEN, this);
                }
                Msg.D("exit i: " + i, 5, this);
                regionMap.add(curRegion);
                curAddr += 4;
                appendPreprocessedAsm(asmLines[i - 1], i, true);
            }

            Msg.D("pr:\n" + preprocessedAsm + ">>>", 30, this);

        }

        } catch(Exception e) {
            error++; Msg.E("preprocess(" + curActiveFile + ":" + i + "): " + e,
                              Constants.PLP_GENERIC_ERROR, this);
        }

        if(error > 0)
            Msg.E("preprocess: " +
                     error + " error(s).", Constants.PLP_ASM_PREPROCESS_FAILED, this);
        
        Msg.D("preprocess(" + curActiveFile + "): First pass completed.", 1, this);

        return (error == 0) ? 0 : Constants.PLP_ASM_PREPROCESS_FAILED;
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
        int error = 0;		// # errors
        long asmPC = 0;		// assembler PC address
        int s = 0;              // assembler directive line offsets (skips)
        curRegion = 0;          // reset to default region
        
        String delimiters = "[ ,\t]+|[()]";
        String lineDelim  = "\\r?\\n";

        Msg.D("assemble(): 1/2...", 1, this);

        preprocessedAsm = pass1Str.toString();
        String[] asmLines  = this.preprocessedAsm.split(lineDelim);
        String[] asmTokens;
        String[] stripComments;

        long branchTarget;
        boolean skip;

        objectCode = new long[asmLines.length - directiveOffset];
        addrTable = new long[asmLines.length - directiveOffset];
        entryType = new int[asmLines.length - directiveOffset];
        objCodeFileMapper = new int[asmLines.length - directiveOffset];
        objCodeLineNumMapper = new int[asmLines.length - directiveOffset];
        curActiveFile = this.topLevelFile;

        // clear error
        Msg.lastError = 0;

        try {

        Msg.D("assemble(): 2/2...", 1, this);

        while(i < asmLines.length) {
            stripComments = asmLines[i].split("#");
            stripComments[0] = stripComments[0].trim();
            asmTokens = stripComments[0].split(delimiters);
            skip = false;
            Msg.D("assemble(file " + sourceList.get(asmFileMap[i]).getAsmFilePath() +
                     " line " + lineNumMap[i]  + "): " + asmLines[i], 4, this);

            // resolve symbols ($_hi and $_lo directives from 1st pass)
            String tSymbol;
            int   tValue = 0;

            for(j = 0; j < asmTokens.length; j++) {

                Msg.D(asmTokens[j] + " l:" + asmTokens[j].length(), 15, this);
                
                if(asmTokens[j].length() > 4)  {

                    Msg.D(asmTokens[j].substring(0, 5), 15, this);

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
                    Msg.D("pr: " + Integer.toHexString(tValue), 15, this);
                }
            }

            int instrType = (Integer) instrMap.get(asmTokens[0]);

            if(instrType < 10) {
                objectCode[i - s] = 0;
                entryType[i - s] = 0;
            }

            switch(instrType) {

                // 3-op R-type (includes multiply, case 8)
                case 0:
		case 8:
                    if(!checkNumberOfOperands(asmTokens, 4, sourceList.get(asmFileMap[i]).getAsmFilePath(), lineNumMap[i])) {
                        error++;
                    }

                    else if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[2]) ||
                       !regs.containsKey(asmTokens[3])) {
                        error++; Msg.E("assemble(" + sourceList.get(asmFileMap[i]).getAsmFilePath() + ":" +
                                          lineNumMap[i] + "): Invalid register(s)",
                                          Constants.PLP_ASM_INVALID_REGISTER, this);
                    } else {

                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[2])) << 21;
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[3])) << 16;
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[1])) << 11;
                    objectCode[i - s] |= (Byte) funct.get(asmTokens[0]);

                    }

                    break;

                // Shift R-type
                case 1:
                    if(!checkNumberOfOperands(asmTokens, 4, sourceList.get(asmFileMap[i]).getAsmFilePath(), lineNumMap[i])) {
                        error++;
                    }

                    else if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[2])) {
                        error++; Msg.E("assemble(" + sourceList.get(asmFileMap[i]).getAsmFilePath() + ":" +
                                          lineNumMap[i] + "): Invalid register(s)",
                                          Constants.PLP_ASM_INVALID_REGISTER, this);
                    } else {

                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[2])) << 16;
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[1])) << 11;
                    objectCode[i - s] |= ((byte) (sanitize16bits(asmTokens[3]) & 0x1F)) << 6;
                    objectCode[i - s] |= (Byte) funct.get(asmTokens[0]);

                    }

                    break;

                // Jump R-type
                case 2:
                    if(!checkNumberOfOperands(asmTokens, 2, sourceList.get(asmFileMap[i]).getAsmFilePath(), lineNumMap[i])) {
                        error++;
                    }

                    else if(!regs.containsKey(asmTokens[1]))  {
                        error++; Msg.E("assemble(" + sourceList.get(asmFileMap[i]).getAsmFilePath() + ":" +
                                          lineNumMap[i] + "): Invalid register(s)",
                                          Constants.PLP_ASM_INVALID_REGISTER, this);
                    } else {

                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[1])) << 21;
                    objectCode[i - s] |= (Byte) funct.get(asmTokens[0]);

                    }

                    break;

                // Branch I-type
                case 3:
                    if(!checkNumberOfOperands(asmTokens, 4, sourceList.get(asmFileMap[i]).getAsmFilePath(), lineNumMap[i])) {
                        error++;
                    }

                    else if(!regs.containsKey(asmTokens[1]))  {
                        error++; Msg.E("assemble(" + sourceList.get(asmFileMap[i]).getAsmFilePath() + ":" +
                                          lineNumMap[i] + "): Invalid register(s)",
                                          Constants.PLP_ASM_INVALID_REGISTER, this);
                    }
                    else if(!symTable.containsKey(asmTokens[3]))  {
                        error++; Msg.E("assemble(" + sourceList.get(asmFileMap[i]).getAsmFilePath() + ":" +
                                          lineNumMap[i] + "): Invalid branch target \"" + asmTokens[3] + "\"",
                                          Constants.PLP_ASM_INVALID_BRANCH_TARGET, this);
                    } else {

                    branchTarget = symTable.get(asmTokens[3]) - (asmPC + 4);
                    branchTarget /= 4;
                    objectCode[i - s] |= branchTarget & 0xFFFF;
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[1])) << 21;
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[2])) << 16;
                    objectCode[i - s] |= (long) opcode.get(asmTokens[0]) << 26;

                    }

                    break;

                // Arithmetic and Logic I-type
                case 4:
                    if(!checkNumberOfOperands(asmTokens, 4, sourceList.get(asmFileMap[i]).getAsmFilePath(), lineNumMap[i])) {
                        error++;
                    }

                    else if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[2])) {
                        error++; Msg.E("assemble(" + sourceList.get(asmFileMap[i]).getAsmFilePath() + ":" +
                                          lineNumMap[i] + "): Invalid register(s)",
                                          Constants.PLP_ASM_INVALID_REGISTER, this);
                    } else {

                    objectCode[i - s] |= sanitize16bits(asmTokens[3]);
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[1])) << 16;
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[2])) << 21;
                    objectCode[i - s] |= (long) opcode.get(asmTokens[0]) << 26;

                    }

                    break;

                // Load upper immediate I-type
                case 5:
                    if(!checkNumberOfOperands(asmTokens, 3, sourceList.get(asmFileMap[i]).getAsmFilePath(), lineNumMap[i])) {
                        error++;
                    }

                    else if(!regs.containsKey(asmTokens[1]))  {
                        error++; Msg.E("assemble(" + sourceList.get(asmFileMap[i]).getAsmFilePath() + ":" +
                                          lineNumMap[i] + "): Invalid register(s)",
                                          Constants.PLP_ASM_INVALID_REGISTER, this);
                    } else {

                    objectCode[i - s] |= sanitize16bits(asmTokens[2]);
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[1])) << 16;
                    objectCode[i - s] |= (long) opcode.get(asmTokens[0]) << 26;

                    }

                    break;

                // Load/Store Word I-type
                case 6:
                    if(!checkNumberOfOperands(asmTokens, 4, sourceList.get(asmFileMap[i]).getAsmFilePath(), lineNumMap[i])) {
                        error++;
                    }

                    else if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[3])) {
                        error++; Msg.E("assemble(" + sourceList.get(asmFileMap[i]).getAsmFilePath() + ":" +
                                          lineNumMap[i] + "): Invalid register(s)",
                                          Constants.PLP_ASM_INVALID_REGISTER, this);
                    } else {

                    objectCode[i - s] |= sanitize16bits(asmTokens[2]);
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[1])) << 16;
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[3])) << 21;
                    objectCode[i - s] |= (long) opcode.get(asmTokens[0]) << 26;

                    }

                    break;

                // J-type
                case 7:
                    if(!checkNumberOfOperands(asmTokens, 2, sourceList.get(asmFileMap[i]).getAsmFilePath(), lineNumMap[i])) {
                        error++;
                    }

                    else if(!symTable.containsKey(asmTokens[1]))  {
                        error++; Msg.E("assemble(" + sourceList.get(asmFileMap[i]).getAsmFilePath() + ":" +
                                          lineNumMap[i] + "): Invalid jump target \"" + asmTokens[1] + "\"",
                                          Constants.PLP_ASM_INVALID_BRANCH_TARGET, this);
                    } else {

                    objectCode[i - s] |= (long) (symTable.get(asmTokens[1]) >> 2) & 0x3FFFFFF;
                    objectCode[i - s] |= (long) opcode.get(asmTokens[0]) << 26;

                    }

                    break;

                // jalr Instruction
                case 9:
                    if(!checkNumberOfOperands(asmTokens, 3, sourceList.get(asmFileMap[i]).getAsmFilePath(), lineNumMap[i])) {
                        error++;
                    }

                    else if(!regs.containsKey(asmTokens[1]) ||
                       !regs.containsKey(asmTokens[2])) {
                        error++; Msg.E("assemble(" + sourceList.get(asmFileMap[i]).getAsmFilePath() + ":" +
                                          lineNumMap[i] + "): Invalid register(s)",
                                          Constants.PLP_ASM_INVALID_REGISTER, this);
                    } else {

                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[2])) << 21;
                    objectCode[i - s] |= ((Byte) regs.get(asmTokens[1])) << 11;
                    objectCode[i - s] |= (Byte) funct.get(asmTokens[0]);

                    }

                    break;

                // 2nd pass directives
                case 10:
                    if(asmTokens[0].equals("ASM__WORD__")) {
                        if(!checkNumberOfOperands(asmTokens, 2, sourceList.get(asmFileMap[i]).getAsmFilePath(), lineNumMap[i])) {
                            error++;
                        } else {
                        entryType[i - s] = 1;
                        objectCode[i - s] = sanitize32bits(asmTokens[1]);
                        }
                    }
                    else if(asmTokens[0].equals("ASM__ORG__")) {
                        if(!checkNumberOfOperands(asmTokens, 2, sourceList.get(asmFileMap[i]).getAsmFilePath(), lineNumMap[i])) {
                            error++;
                        } else {

                        asmPC = sanitize32bits(asmTokens[1]);
                        s++;
                        skip = true;
                        }
                    }
                    else {
                        s++;
                        skip = true;
	            }

                    break;

                // Pass-2 pseudo ops
                case 11:

                    break;
                    
                default:
                    error++; Msg.E("assemble(" + sourceList.get(asmFileMap[i]).getAsmFilePath() + ":" +
                                      lineNumMap[i] + "): Unhandled error.",
                                      Constants.PLP_GENERIC_ERROR, this);
            }

            // Update address table and assembler PC if this line is a valid
            // instruction / .word directive
            if(!skip) {
                addrTable[i - s] = asmPC;
                asmPC += 4;

                // update mappers
                objCodeFileMapper[i - s] = asmFileMap[i];
                objCodeLineNumMapper[i - s] = lineNumMap[i];
            }
            i++;
        }

        Msg.D("assemble(): Assembly completed.", 1, this);
        
        } catch(Exception e) {
            error++; Msg.E("assemble(" + sourceList.get(asmFileMap[i]).getAsmFilePath() + ":" +
                              lineNumMap[i] + "): Unhandled exception: " + e,
                              Constants.PLP_GENERIC_ERROR, this);
        }

        if(error > 0)
            Msg.E("assemble: " +
                     error + " error(s).", Constants.PLP_ASM_ASSEMBLE_FAILED, this);
        else
            assembled = true;

        return (error == 0) ? Constants.PLP_OK : Constants.PLP_ASM_ASSEMBLE_FAILED;
    }
    
    /**
     * Returns whether the object code entry in the given index is an instruction
     * (zero) or not (non-zero)
     *
     * @return Returns the entry type ID
     * @param index Index of the entry in the object code array
     */
    public int isInstruction(int index) {
        return entryType[index];
    }

    /**
     * Returns the string representation of an opcode
     *
     * @return Returns instruction opcode in String
     * @param instrOpCode the opcode of the instruction in (byte)
     */
    public static String lookupInstrOpcode(byte instrOpCode) {
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

        /**
     * Returns the string representation of a function
     *
     * @return Returns instruction opcode in String
     * @param instrFunct the opcode of the instruction in (byte)
     */
    public static String lookupInstrFunct(byte instrFunct) {
        String key;

        if(funct.containsValue(instrFunct)) {
            Iterator iterator = funct.keySet().iterator();
            while(iterator.hasNext()) {
                key = (String) iterator.next();
                if(funct.get(key).equals(instrFunct)) {
                    return key;
                }
            }
        }
        return null;
    }

    /**
     * Lookup instruction type of the provided opcode as defined by
     * the PLPAsm class.
     *
     * @return Returns instruction type
     * @param instrOpCode Instruction opcode in String
     * @see lookupInstr(byte)
     * @see lookupInstrType(byte)
     */
    public static Integer lookupInstrType(String instrOpCode) {
        if(instrMap.containsKey(instrOpCode)) {
            return (Integer) instrMap.get(instrOpCode);
        }
        return Constants.PLP_ERROR_RETURN;
    }

    /**
     * Takes in a string and attempts to parse it as a 16 bit number. This
     * 16-bit number is stored in int primitive (32-bit data type) to
     * preserve all 16-bits of data since Java doesn't have unsigned data
     * types. Higher 2-bytes of the number are masked.
     *
     * @return Returns 16-bit number in long with higher 2-bytes masked
     * , returns PLP_NUMBER_ERROR if parseInt failed.
     * @param Number to be sanitized in String
     * @see sanitize32bits(String)
     */
    public static long sanitize16bits(String number) throws Exception {
        if(number.startsWith("0x") || number.startsWith("0h")) {
            number = number.substring(2);
            return Long.parseLong(number, 16) & 0xFFFF;
        }
        else if(number.startsWith("0b")) {
            number = number.substring(2);
            return Long.parseLong(number, 2) & 0xFFFF;
        }
        else if (number.startsWith("'") && number.endsWith("'") && number.length()==3) {
            return number.charAt(1);
        }
        else
            return Long.parseLong(number) & 0xFFFF;
    }

    /**
     * Takes in a string and attempts to parse it as a 32 bit number. This
     * 32-bit number is stored in long primitive (64-bit data type) to
     * preserve all 32-bits of data since Java doesn't have unsigned data
     * types. Higher 4-bytes of the number are masked.
     *
     * @return Returns 32-bit number in long with higher 4-bytes masked
     * , returns PLP_NUMBER_ERROR if parseLong failed.
     * @param Number to be sanitized in String
     * @see sanitize16bits(String)
     */
    public static long sanitize32bits(String number) throws Exception {
        if(number.startsWith("0x") || number.startsWith("0h")) {
            number = number.substring(2);
            return Long.parseLong(number, 16) & 0xFFFFFFFF;
        }
        else if(number.startsWith("0b")) {
            number = number.substring(2);
            return Long.parseLong(number, 2) & 0xFFFFFFFF;
        }
        else if (number.startsWith("'") && number.endsWith("'") && number.length()==3) {
            return number.charAt(1);
        }
        else
            return Long.parseLong(number) & 0xFFFFFFFF;
    }

    /**
     * Utility function to check for number of operands agreement.
     *
     * @return Returns true if number of operands is correct, false otherwise
     * @param iObj[] array of operands
     * @param length assertion
     * @param lineNum line number in asm source where this function is called
     */
    private boolean checkNumberOfOperands(Object iObj[], int length, String file, int lineNum) {
        if(iObj.length != length) {
            Msg.E("checkNumberOfOperands(" + file + ":" + lineNum + "): Invalid number of operands",
                            Constants.PLP_ASM_INVALID_NUMBER_OF_OPERANDS, this);
            return false;
        }
        return true;
    }

    /**
     * Casting PLPAsm as String will return PLPAsm(assembly file)
     *
     * @return Returns informative string
     */
    @Override public String toString() {
        return "Asm";
    }
}
