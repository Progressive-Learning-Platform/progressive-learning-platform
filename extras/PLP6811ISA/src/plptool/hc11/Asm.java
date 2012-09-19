/*
    Copyright 2012 David Fritz, Brian Gordon, Wira Mulia

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

package plptool.hc11;

import plptool.*;
import plptool.gui.ProjectDriver;
import java.util.ArrayList;
import java.util.HashMap;

/**
 *
 * @author wira
 */
public class Asm extends PLPAsm {
    // Addressing Mode bitmask
    public static final int INH        = 0x001;
    public static final int IMM        = 0x002;
    public static final int DIR        = 0x004;
    public static final int EXT        = 0x008;
    public static final int INDX       = 0x010;
    public static final int INDY       = 0x020;
    public static final int REL        = 0x040;

    // Additional parameters bitmask
    public static final int REGA       = 0x100;
    public static final int REGB       = 0x200;
    public static final int MASK       = 0x400;
    public static final int LONG       = 0x800;

    public ArrayList<HC11Instr> instructions;
    public HashMap<String, HC11Instr> instrMap;
    private String[] lines;

    int PC;

    public Asm(ArrayList<PLPAsmSource> asms) {
        super(asms);
        define();
    }

    public int assemble() {
        
        setAssembled(true);
        return Constants.PLP_OK;
    }

    public int preprocess(int index) {
        PC = 0x0000;

        /* In the first pass, we populate our symbol table and map addresses to
         * the corresponding source file and line number. For this, we also need
         * to figure out instruction lengths
         */
        for(int i = 0; i < sourceList.size(); i++) {
            lines = sourceList.get(i).getAsmString().split("\\r?\\n");

            for(int l = 0; l < lines.length; l++) {
                
            }
        }

        return Constants.PLP_OK;
    }

    public String[] getLines() {
        return lines;
    }

    private void define() {
        instructions = new ArrayList<HC11Instr>();
        instrMap = new HashMap<String, HC11Instr>();

        add("ABA",   INH                                 ,   0x1B);
        add("ABX",   INH                                 ,   0x3A);
        add("ABY",   INH                           | LONG,   0x183A);

        add("ADCA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x89, 0x99, 0xB9, 0xA9, 0x18A9);
        add("ADCB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC9, 0xD9, 0xF9, 0xE9, 0x18E9);

        add("ADDA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x8B, 0x9B, 0xBB, 0xAB, 0x18AB);
        add("ADDB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xCB, 0xDB, 0xFB, 0xEB, 0x18EB);
        add("ADDD",  IMM | DIR | EXT | INDX | INDY       ,   0xC3, 0xD3, 0xF3, 0xE3, 0x18E3);

        add("ANDA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x84, 0x94, 0xB4, 0xA4, 0x18A4);
        add("ANDB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC4, 0xD4, 0xF4, 0xE4, 0x18E4);

        add("ASL",               EXT | INDX | INDY       ,   -1,   -1,   0x78, 0x68, 0x1868);
        add("ASLA",  INH                           | REGA,   0x48);
        add("ASLB",  INH                           | REGB,   0x58);
        add("ASLD",  INH                                 ,   0x05);

        add("ASR",               EXT | INDX | INDY       ,   -1,   -1,   0x77, 0x67, 0x1867);
        add("ASRA",  INH                           | REGA,   0x47);
        add("ASRB",  INH                           | REGB,   0x57);

        add("BCC",   REL                                 ,   0x24);
        add("BCS",   REL                                 ,   0x25);
        add("BEQ",   REL                                 ,   0x27);
        add("BGE",   REL                                 ,   0x2C);
        add("BGT",   REL                                 ,   0x2E);
        add("BHI",   REL                                 ,   0x22);
        add("BHS",   REL                                 ,   0x24);
        add("BLE",   REL                                 ,   0x2F);
        add("BLO",   REL                                 ,   0x25);
        add("BLS",   REL                                 ,   0x23);
        add("BLT",   REL                                 ,   0x2D);
        add("BMI",   REL                                 ,   0x2B);
        add("BNE",   REL                                 ,   0x26);
        add("BPL",   REL                                 ,   0x2A);
        add("BRA",   REL                                 ,   0x20);
        add("BRN",   REL                                 ,   0x21);
        add("BSR",   REL                                 ,   0x8D);
        add("BVC",   REL                                 ,   0x28);
        add("BVS",   REL                                 ,   0x29);

        add("BRCLR", REL | DIR |       INDX | INDY | MASK,   -1,   0x13, -1,   0x1F, 0x181F);
        add("BRSET", REL | DIR |       INDX | INDY | MASK,   -1,   0x12, -1,   0x1E, 0x181E);

        add("BCLR",        DIR |       INDX | INDY | MASK,   -1,   0x15, -1,   0x1D, 0x181D);
        add("BSET",        DIR |       INDX | INDY | MASK,   -1,   0x14, -1,   0x1C, 0x181C);
        add("BITA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x85, 0x95, 0xB5, 0xA5, 0x18A5);
        add("BITB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC5, 0xD5, 0xF5, 0xE5, 0x18E5);

        add("CBA",   INH                                 ,   0x11);
        add("CLC",   INH                                 ,   0x0C);
        add("CLI",   INH                                 ,   0x0E);

        add("CLR",               EXT | INDX | INDY       ,   -1,   -1,   0x7F, 0x6F, 0x186F);
        add("CLRA",  INH                           | REGA,   0x4F);
        add("CLRB",  INH                           | REGB,   0x5F);

        add("CLV",   INH                                 ,   0x0A);

        add("CMPA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x81, 0x91, 0xB1, 0xA1, 0x18A1);
        add("CMPB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC1, 0xD1, 0xF1, 0xE1, 0x18E1);

        add("COM",               EXT | INDX | INDY       ,   -1,   -1,   0x73, 0x63, 0x1863);
        add("COMA",  INH                           | REGA,   0x43);
        add("COMB",  INH                           | REGB,   0x53);

        add("CPD",   IMM | DIR | EXT | INDX | INDY | LONG,   0x1A83, 0x1A93, 0x1AB3, 0x1AA3, 0xCDA3);
        add("CPX",   IMM | DIR | EXT | INDX | INDY       ,   0x8C, 0x9C, 0xBC, 0xAC, 0xCDAC);
        add("CPY",   IMM | DIR | EXT | INDX | INDY | LONG,   0x188C, 0x189C, 0x18BC, 0x1AAC, 0x18AC);

        add("DAA",   INH                                 ,   0x19);

        add("DEC",               EXT | INDX | INDY       ,   -1,   -1,   0x7A, 0x6A, 0x186A);
        add("DECA",  INH                           | REGA,   0x4A);
        add("DECB",  INH                           | REGB,   0x5A);
        add("DES",   INH                                 ,   0x34);
        add("DEX",   INH                                 ,   0x09);
        add("DEY",   INH                           | LONG,   0x1809);

        add("EORA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x88, 0x98, 0xB8, 0xA8, 0x18A8);
        add("EORB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC8, 0xD8, 0xF8, 0xE8, 0x18E8);

        add("FDIV",  INH                                 ,   0x03);
        add("IDIV",  INH                                 ,   0x02);

        add("INC",               EXT | INDX | INDY       ,   -1,   -1,   0x7C, 0x6C, 0x186C);
        add("INCA",  INH                           | REGA,   0x4C);
        add("INCB",  INH                           | REGB,   0x5C);
        add("INS",   INH                                 ,   0x31);
        add("INX",   INH                                 ,   0x08);
        add("INY",   INH                           | LONG,   0x1808);

        add("JMP",               EXT | INDX | INDY       ,   -1,   -1,   0x7E, 0x6E, 0x186E);
        add("JSR",         DIR | EXT | INDX | INDY       ,   -1,   0x9D, 0xBD, 0xAD, 0x18AD);

        add("LDAA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x86, 0x96, 0xB6, 0xA6, 0x18A6);
        add("LDAB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC6, 0xD6, 0xF6, 0xE6, 0x18E6);
        add("LDD",   IMM | DIR | EXT | INDX | INDY       ,   0xCC, 0xDC, 0xFC, 0xEC, 0x18EC);
        add("LDS",   IMM | DIR | EXT | INDX | INDY       ,   0x8E, 0x9E, 0xBE, 0xAE, 0x18AE);
        add("LDX",   IMM | DIR | EXT | INDX | INDY       ,   0xCE, 0xDE, 0xFE, 0xEE, 0xCDEE);
        add("LDY",   IMM | DIR | EXT | INDX | INDY | LONG,   0x18CE, 0x18DE, 0x18FE, 0x1AEE, 0x18EE);

        add("LSL",               EXT | INDX | INDY       ,   -1,   -1,   0x78, 0x68, 0x1868);
        add("LSLA",  INH                           | REGA,   0x48);
        add("LSLB",  INH                           | REGB,   0x58);
        add("LSLD",  INH                                 ,   0x05);

        add("LSR",               EXT | INDX | INDY       ,   -1,   -1,   0x74, 0x64, 0x1864);
        add("LSRA",  INH                           | REGA,   0x44);
        add("LSRB",  INH                           | REGB,   0x54);
        add("LSRD",  INH                                 ,   0x04);

        add("MUL",   INH                                 ,   0x3D);

        add("NEG",               EXT | INDX | INDY       ,   -1,   -1,   0x70, 0x60, 0x1860);
        add("NEGA",  INH                           | REGA,   0x40);
        add("NEGB",  INH                           | REGB,   0x50);

        add("NOP",   INH                                 ,   0x01);

        add("ORAA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x8A, 0x9A, 0xBA, 0xAA, 0x18AA);
        add("ORAB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xCA, 0xDA, 0xFA, 0xEA, 0x18EA);

        add("PSHA",  INH                           | REGA,   0x36);
        add("PSHB",  INH                           | REGB,   0x37);
        add("PSHX",  INH                                 ,   0x3C);
        add("PSHY",  INH                           | LONG,   0x183C);

        add("PULA",  INH                           | REGA,   0x32);
        add("PULB",  INH                           | REGB,   0x33);
        add("PULX",  INH                                 ,   0x38);
        add("PULY",  INH                           | LONG,   0x1838);

        add("ROL",               EXT | INDX | INDY       ,   -1,   -1,   0x79, 0x69, 0x1869);
        add("ROLA",  INH                           | REGA,   0x49);
        add("ROLB",  INH                           | REGB,   0x59);

        add("ROR",               EXT | INDX | INDY       ,   -1,   -1,   0x76, 0x66, 0x1866);
        add("RORA",  INH                           | REGA,   0x46);
        add("RORB",  INH                           | REGB,   0x56);

        add("RTI",   INH                                 ,   0x3B);
        add("RTS",   INH                                 ,   0x39);

        add("SBA",   INH                                 ,   0x10);
        add("SBCA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x82, 0x92, 0xB2, 0xA2, 0x18A2);
        add("SBCB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC2, 0xD2, 0xF2, 0xE2, 0x18E2);

        add("SEC",   INH                                 ,   0x0D);
        add("SEI",   INH                                 ,   0x0F);
        add("SEV",   INH                                 ,   0x0B);
        add("STOP",  INH                                 ,   0xCF);
        add("SWI",   INH                                 ,   0x3F);

        add("STAA",        DIR | EXT | INDX | INDY | REGA,   -1,   0x97, 0xB7, 0xA7, 0x18A7);
        add("STAB",        DIR | EXT | INDX | INDY | REGB,   -1,   0xD7, 0xF7, 0xE7, 0x18E7);
        add("STD",         DIR | EXT | INDX | INDY       ,   -1,   0xDD, 0xFD, 0xED, 0x18ED);
        add("STS",         DIR | EXT | INDX | INDY       ,   -1,   0x9F, 0xBF, 0xAF, 0x18AF);
        add("STX",         DIR | EXT | INDX | INDY       ,   -1,   0xDF, 0xFF, 0xEF, 0xCDEF);
        add("STY",         DIR | EXT | INDX | INDY | LONG,   -1,   0x18DF, 0x18FF, 0x1AEF, 0x18EF);

        add("SUBA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x80, 0x90, 0xB0, 0xA0, 0x18A0);
        add("SUBB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC0, 0xD0, 0xF0, 0xE0, 0x18E0);
        add("SUBD",  IMM | DIR | EXT | INDX | INDY       ,   0x83, 0x93, 0xB3, 0xA3, 0x18A3);

        add("TAB",   INH                                 ,   0x16);
        add("TAP",   INH                                 ,   0x06);
        add("TBA",   INH                                 ,   0x17);
        add("TEST",  INH                                 ,   0x00);
        add("TPA",   INH                                 ,   0x07);

        add("TST",               EXT | INDX | INDY       ,   -1,   -1,   0x7D, 0x6D, 0x186D);
        add("TSTA",  INH                           | REGA,   0x4D);
        add("TSTB",  INH                           | REGB,   0x5D);

        add("TSX",   INH                                 ,   0x30);
        add("ISY",   INH                           | LONG,   0x1830);
        add("TXS",   INH                                 ,   0x35);
        add("TXY",   INH                           | LONG,   0x1835);

        add("WAI",   INH                                 ,   0x3E);

        add("XGDX",  INH                                 ,   0x8F);
        add("XGDY",  INH                           | LONG,   0x188F);

        for(int i = 0; i < instructions.size(); i++)
            instrMap.put(instructions.get(i).mnemonic, instructions.get(i));
    }

    private void add(String mnemonic, int modes, int...opcodes) {
        instructions.add(new HC11Instr(mnemonic, modes, opcodes));
    }

    /**
     * Get mode of the specified instruction expression and validate the literal
     * input
     *
     * @param instr Instruction expression to validate
     * @return Mode, throws exception if invalid literal is provided
     */
    public int getMode(String instr) throws Exception {
        int mode = 0;

        String tokens[] = instr.split("\\s+");

        HC11Instr i = instrMap.get(tokens[0]);

        if(i == null)
            throw new Exception("Invalid instruction mnemonic");

        if(tokens.length == 1)
            mode |= INH;
        else if(tokens.length == 2 && tokens[1].startsWith("#"))
            mode |= IMM;

        if(INH(mode) != INH(i.addr_modes))
            throw new Exception(i.mnemonic + " can not be used inherently");

        if(REL(mode) != REL(i.addr_modes))
            throw new Exception(i.mnemonic + " does not use relative addressing");

        if(IMM(mode) != IMM(i.addr_modes))
            throw new Exception(i.mnemonic + " can not be used with immediate addressing mode");

        if(DIR(mode) != DIR(i.addr_modes))
            throw new Exception(i.mnemonic + " can not be used with direct addressing mode");

        if(EXT(mode) != EXT(i.addr_modes))
            throw new Exception(i.mnemonic + " can not be used with extended addressing mode");

        if(INDX(mode) != INDX(i.addr_modes))
            throw new Exception(i.mnemonic + " can not be used with indexed addressing mode (X)");

        if(INDY(mode) != INDY(i.addr_modes))
            throw new Exception(i.mnemonic + " can not be used with indexed addressing mode (Y)");

        return mode;
    }

    public boolean INH(int modes) {
        return (modes & INH) == INH;
    }

    public boolean REL(int modes) {
        return (modes & REL) == REL;
    }

    public boolean IMM(int modes) {
        return (modes & IMM) == IMM;
    }

    public boolean DIR(int modes) {
        return (modes & DIR) == DIR;
    }

    public boolean EXT(int modes) {
        return (modes & EXT) == EXT;
    }

    public boolean INDX(int modes) {
        return (modes & INDX) == INDX;
    }

    public boolean INDY(int modes) {
        return (modes & INDY) == INDY;
    }

    public boolean LONG(int modes) {
        return (modes & LONG) == LONG;
    }

    public class HC11Instr {
        public String mnemonic;
        public int addr_modes;
        public int[] opcodes;

        // opcode array index
        public static final int REL  = 0;
        public static final int INH  = 0;
        public static final int IMM  = 0;
        public static final int DIR  = 1;
        public static final int EXT  = 2;
        public static final int INDX = 3;
        public static final int INDY = 4;


        public HC11Instr(String mnemonic, int addr_modes, int...opcodes) {
            this.mnemonic = mnemonic;
            this.addr_modes = addr_modes;
            this.opcodes = opcodes;
        }

    }
}
