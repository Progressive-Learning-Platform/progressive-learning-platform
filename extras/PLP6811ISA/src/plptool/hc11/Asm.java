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
    // Addressing Modes
    public static final int INH        = 0x0001;
    public static final int IMM        = 0x0002;
    public static final int DIR        = 0x0004;
    public static final int EXT        = 0x0008;
    public static final int INDX       = 0x0010;
    public static final int INDY       = 0x0020;
    public static final int REL        = 0x0040;

    public static final int REGA       = 0x0080;
    public static final int REGB       = 0x0100;
    public static final int MASK       = 0x0200;
    public static final int INHY       = 0x0400;
    public static final int LONG       = 0x0800;

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
         * to figure out instruction lengths (damn variable length instructions!)
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

        instructions.add(new HC11Instr("ABA",   INH                                 ,   0x1B));
        instructions.add(new HC11Instr("ABX",   INH                                 ,   0x3A));
        instructions.add(new HC11Instr("ABY",   INH                           | LONG,   0x183A));

        instructions.add(new HC11Instr("ADCA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x89, 0x99, 0xB9, 0xA9, 0x18A9));
        instructions.add(new HC11Instr("ADCB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC9, 0xD9, 0xF9, 0xE9, 0x18E9));

        instructions.add(new HC11Instr("ADDA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x8B, 0x9B, 0xBB, 0xAB, 0x18AB));
        instructions.add(new HC11Instr("ADDB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xCB, 0xDB, 0xFB, 0xEB, 0x18EB));
        instructions.add(new HC11Instr("ADDD",  IMM | DIR | EXT | INDX | INDY       ,   0xC3, 0xD3, 0xF3, 0xE3, 0x18E3));

        instructions.add(new HC11Instr("ANDA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x84, 0x94, 0xB4, 0xA4, 0x18A4));
        instructions.add(new HC11Instr("ANDB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC4, 0xD4, 0xF4, 0xE4, 0x18E4));

        instructions.add(new HC11Instr("ASL",               EXT | INDX | INDY       ,   -1,   -1,   0x78, 0x68, 0x1868));
        instructions.add(new HC11Instr("ASLA",  INH                           | REGA,   0x48));
        instructions.add(new HC11Instr("ASLB",  INH                           | REGB,   0x58));
        instructions.add(new HC11Instr("ASLD",  INH                                 ,   0x05));

        instructions.add(new HC11Instr("ASR",               EXT | INDX | INDY       ,   -1,   -1,   0x77, 0x67, 0x1867));
        instructions.add(new HC11Instr("ASRA",  INH                           | REGA,   0x47));
        instructions.add(new HC11Instr("ASRB",  INH                           | REGB,   0x57));

        instructions.add(new HC11Instr("BCC",   REL                                 ,   0x24));
        instructions.add(new HC11Instr("BCS",   REL                                 ,   0x25));
        instructions.add(new HC11Instr("BEQ",   REL                                 ,   0x27));
        instructions.add(new HC11Instr("BGE",   REL                                 ,   0x2C));
        instructions.add(new HC11Instr("BGT",   REL                                 ,   0x2E));
        instructions.add(new HC11Instr("BHI",   REL                                 ,   0x22));
        instructions.add(new HC11Instr("BHS",   REL                                 ,   0x24));
        instructions.add(new HC11Instr("BLE",   REL                                 ,   0x2F));
        instructions.add(new HC11Instr("BLO",   REL                                 ,   0x25));
        instructions.add(new HC11Instr("BLS",   REL                                 ,   0x23));
        instructions.add(new HC11Instr("BLT",   REL                                 ,   0x2D));
        instructions.add(new HC11Instr("BMI",   REL                                 ,   0x2B));
        instructions.add(new HC11Instr("BNE",   REL                                 ,   0x26));
        instructions.add(new HC11Instr("BPL",   REL                                 ,   0x2A));
        instructions.add(new HC11Instr("BRA",   REL                                 ,   0x20));
        instructions.add(new HC11Instr("BRN",   REL                                 ,   0x21));
        instructions.add(new HC11Instr("BSR",   REL                                 ,   0x8D));
        instructions.add(new HC11Instr("BVC",   REL                                 ,   0x28));
        instructions.add(new HC11Instr("BVS",   REL                                 ,   0x29));

        instructions.add(new HC11Instr("BRCLR", REL | DIR |       INDX | INDY | MASK,   -1,   0x13, -1,   0x1F, 0x181F));
        instructions.add(new HC11Instr("BRSET", REL | DIR |       INDX | INDY | MASK,   -1,   0x12, -1,   0x1E, 0x181E));

        instructions.add(new HC11Instr("BCLR",        DIR |       INDX | INDY | MASK,   -1,   0x15, -1,   0x1D, 0x181D));
        instructions.add(new HC11Instr("BSET",        DIR |       INDX | INDY | MASK,   -1,   0x14, -1,   0x1C, 0x181C));
        instructions.add(new HC11Instr("BITA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x85, 0x95, 0xB5, 0xA5, 0x18A5));
        instructions.add(new HC11Instr("BITB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC5, 0xD5, 0xF5, 0xE5, 0x18E5));

        instructions.add(new HC11Instr("CBA",   INH                                 ,   0x11));
        instructions.add(new HC11Instr("CLC",   INH                                 ,   0x0C));
        instructions.add(new HC11Instr("CLI",   INH                                 ,   0x0E));

        instructions.add(new HC11Instr("CLR",               EXT | INDX | INDY       ,   -1,   -1,   0x7F, 0x6F, 0x186F));
        instructions.add(new HC11Instr("CLRA",  INH                           | REGA,   0x4F));
        instructions.add(new HC11Instr("CLRB",  INH                           | REGB,   0x5F));

        instructions.add(new HC11Instr("CLV",   INH                                 ,   0x0A));

        instructions.add(new HC11Instr("CMPA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x81, 0x91, 0xB1, 0xA1, 0x18A1));
        instructions.add(new HC11Instr("CMPB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC1, 0xD1, 0xF1, 0xE1, 0x18E1));

        instructions.add(new HC11Instr("COM",               EXT | INDX | INDY       ,   -1,   -1,   0x73, 0x63, 0x1863));
        instructions.add(new HC11Instr("COMA",  INH                           | REGA,   0x43));
        instructions.add(new HC11Instr("COMB",  INH                           | REGB,   0x53));

        instructions.add(new HC11Instr("CPD",   IMM | DIR | EXT | INDX | INDY | LONG,   0x1A83, 0x1A93, 0x1AB3, 0x1AA3, 0xCDA3));
        instructions.add(new HC11Instr("CPX",   IMM | DIR | EXT | INDX | INDY       ,   0x8C, 0x9C, 0xBC, 0xAC, 0xCDAC));
        instructions.add(new HC11Instr("CPY",   IMM | DIR | EXT | INDX | INDY | LONG,   0x188C, 0x189C, 0x18BC, 0x1AAC, 0x18AC));

        instructions.add(new HC11Instr("DAA",   INH                                 ,   0x19));

        instructions.add(new HC11Instr("DEC",               EXT | INDX | INDY       ,   -1,   -1,   0x7A, 0x6A, 0x186A));
        instructions.add(new HC11Instr("DECA",  INH                           | REGA,   0x4A));
        instructions.add(new HC11Instr("DECB",  INH                           | REGB,   0x5A));
        instructions.add(new HC11Instr("DES",   INH                                 ,   0x34));
        instructions.add(new HC11Instr("DEX",   INH                                 ,   0x09));
        instructions.add(new HC11Instr("DEY",   INH                           | LONG,   0x1809));

        instructions.add(new HC11Instr("EORA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x88, 0x98, 0xB8, 0xA8, 0x18A8));
        instructions.add(new HC11Instr("EORB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC8, 0xD8, 0xF8, 0xE8, 0x18E8));

        instructions.add(new HC11Instr("FDIV",  INH                                 ,   0x03));
        instructions.add(new HC11Instr("IDIV",  INH                                 ,   0x02));

        instructions.add(new HC11Instr("INC",               EXT | INDX | INDY       ,   -1,   -1,   0x7C, 0x6C, 0x186C));
        instructions.add(new HC11Instr("INCA",  INH                           | REGA,   0x4C));
        instructions.add(new HC11Instr("INCB",  INH                           | REGB,   0x5C));
        instructions.add(new HC11Instr("INS",   INH                                 ,   0x31));
        instructions.add(new HC11Instr("INX",   INH                                 ,   0x08));
        instructions.add(new HC11Instr("INY",   INH                           | LONG,   0x1808));

        instructions.add(new HC11Instr("JMP",               EXT | INDX | INDY       ,   -1,   -1,   0x7E, 0x6E, 0x186E));
        instructions.add(new HC11Instr("JSR",         DIR | EXT | INDX | INDY       ,   -1,   0x9D, 0xBD, 0xAD, 0x18AD));

        instructions.add(new HC11Instr("LDAA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x86, 0x96, 0xB6, 0xA6, 0x18A6));
        instructions.add(new HC11Instr("LDAB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC6, 0xD6, 0xF6, 0xE6, 0x18E6));
        instructions.add(new HC11Instr("LDD",   IMM | DIR | EXT | INDX | INDY       ,   0xCC, 0xDC, 0xFC, 0xEC, 0x18EC));
        instructions.add(new HC11Instr("LDS",   IMM | DIR | EXT | INDX | INDY       ,   0x8E, 0x9E, 0xBE, 0xAE, 0x18AE));
        instructions.add(new HC11Instr("LDX",   IMM | DIR | EXT | INDX | INDY       ,   0xCE, 0xDE, 0xFE, 0xEE, 0xCDEE));
        instructions.add(new HC11Instr("LDY",   IMM | DIR | EXT | INDX | INDY | LONG,   0x18CE, 0x18DE, 0x18FE, 0x1AEE, 0x18EE));

        instructions.add(new HC11Instr("LSL",               EXT | INDX | INDY       ,   -1,   -1,   0x78, 0x68, 0x1868));
        instructions.add(new HC11Instr("LSLA",  INH                           | REGA,   0x48));
        instructions.add(new HC11Instr("LSLB",  INH                           | REGB,   0x58));
        instructions.add(new HC11Instr("LSLD",  INH                                 ,   0x05));

        instructions.add(new HC11Instr("LSR",               EXT | INDX | INDY       ,   -1,   -1,   0x74, 0x64, 0x1864));
        instructions.add(new HC11Instr("LSRA",  INH                           | REGA,   0x44));
        instructions.add(new HC11Instr("LSRB",  INH                           | REGB,   0x54));
        instructions.add(new HC11Instr("LSRD",  INH                                 ,   0x04));

        instructions.add(new HC11Instr("MUL",   INH                                 ,   0x3D));

        instructions.add(new HC11Instr("NEG",               EXT | INDX | INDY       ,   -1,   -1,   0x70, 0x60, 0x1860));
        instructions.add(new HC11Instr("NEGA",  INH                           | REGA,   0x40));
        instructions.add(new HC11Instr("NEGB",  INH                           | REGB,   0x50));

        instructions.add(new HC11Instr("NOP",   INH                                 ,   0x01));

        instructions.add(new HC11Instr("ORAA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x8A, 0x9A, 0xBA, 0xAA, 0x18AA));
        instructions.add(new HC11Instr("ORAB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xCA, 0xDA, 0xFA, 0xEA, 0x18EA));

        instructions.add(new HC11Instr("PSHA",  INH                           | REGA,   0x36));
        instructions.add(new HC11Instr("PSHB",  INH                           | REGB,   0x37));
        instructions.add(new HC11Instr("PSHX",  INH                                 ,   0x3C));
        instructions.add(new HC11Instr("PSHY",  INH                           | LONG,   0x183C));

        instructions.add(new HC11Instr("PULA",  INH                           | REGA,   0x32));
        instructions.add(new HC11Instr("PULB",  INH                           | REGB,   0x33));
        instructions.add(new HC11Instr("PULX",  INH                                 ,   0x38));
        instructions.add(new HC11Instr("PULY",  INH                           | LONG,   0x1838));

        instructions.add(new HC11Instr("ROL",               EXT | INDX | INDY       ,   -1,   -1,   0x79, 0x69, 0x1869));
        instructions.add(new HC11Instr("ROLA",  INH                           | REGA,   0x49));
        instructions.add(new HC11Instr("ROLB",  INH                           | REGB,   0x59));

        instructions.add(new HC11Instr("ROR",               EXT | INDX | INDY       ,   -1,   -1,   0x76, 0x66, 0x1866));
        instructions.add(new HC11Instr("RORA",  INH                           | REGA,   0x46));
        instructions.add(new HC11Instr("RORB",  INH                           | REGB,   0x56));

        instructions.add(new HC11Instr("RTI",   INH                                 ,   0x3B));
        instructions.add(new HC11Instr("RTS",   INH                                 ,   0x39));

        instructions.add(new HC11Instr("SBA",   INH                                 ,   0x10));
        instructions.add(new HC11Instr("SBCA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x82, 0x92, 0xB2, 0xA2, 0x18A2));
        instructions.add(new HC11Instr("SBCB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC2, 0xD2, 0xF2, 0xE2, 0x18E2));

        instructions.add(new HC11Instr("SEC",   INH                                 ,   0x0D));
        instructions.add(new HC11Instr("SEI",   INH                                 ,   0x0F));
        instructions.add(new HC11Instr("SEV",   INH                                 ,   0x0B));
        instructions.add(new HC11Instr("STOP",  INH                                 ,   0xCF));
        instructions.add(new HC11Instr("SWI",   INH                                 ,   0x3F));

        instructions.add(new HC11Instr("STAA",        DIR | EXT | INDX | INDY | REGA,   -1,   0x97, 0xB7, 0xA7, 0x18A7));
        instructions.add(new HC11Instr("STAB",        DIR | EXT | INDX | INDY | REGB,   -1,   0xD7, 0xF7, 0xE7, 0x18E7));
        instructions.add(new HC11Instr("STD",         DIR | EXT | INDX | INDY       ,   -1,   0xDD, 0xFD, 0xED, 0x18ED));
        instructions.add(new HC11Instr("STS",         DIR | EXT | INDX | INDY       ,   -1,   0x9F, 0xBF, 0xAF, 0x18AF));
        instructions.add(new HC11Instr("STX",         DIR | EXT | INDX | INDY       ,   -1,   0xDF, 0xFF, 0xEF, 0xCDEF));
        instructions.add(new HC11Instr("STY",         DIR | EXT | INDX | INDY | LONG,   -1,   0x18DF, 0x18FF, 0x1AEF, 0x18EF));

        instructions.add(new HC11Instr("SUBA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x80, 0x90, 0xB0, 0xA0, 0x18A0));
        instructions.add(new HC11Instr("SUBB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC0, 0xD0, 0xF0, 0xE0, 0x18E0));
        instructions.add(new HC11Instr("SUBD",  IMM | DIR | EXT | INDX | INDY       ,   0x83, 0x93, 0xB3, 0xA3, 0x18A3));

        instructions.add(new HC11Instr("TAB",   INH                                 ,   0x16));
        instructions.add(new HC11Instr("TAP",   INH                                 ,   0x06));
        instructions.add(new HC11Instr("TBA",   INH                                 ,   0x17));
        instructions.add(new HC11Instr("TEST",  INH                                 ,   0x00));
        instructions.add(new HC11Instr("TPA",   INH                                 ,   0x07));

        instructions.add(new HC11Instr("TST",               EXT | INDX | INDY       ,   -1,   -1,   0x7D, 0x6D, 0x186D));
        instructions.add(new HC11Instr("TSTA",  INH                           | REGA,   0x4D));
        instructions.add(new HC11Instr("TSTB",  INH                           | REGB,   0x5D));

        instructions.add(new HC11Instr("TSX",   INH                                 ,   0x30));
        instructions.add(new HC11Instr("ISY",   INH                           | LONG,   0x1830));
        instructions.add(new HC11Instr("TXS",   INH                                 ,   0x35));
        instructions.add(new HC11Instr("TXY",   INH                           | LONG,   0x1835));

        instructions.add(new HC11Instr("WAI",   INH                                 ,   0x3E));

        instructions.add(new HC11Instr("XGDX",  INH                                 ,   0x8F));
        instructions.add(new HC11Instr("XGDY",  INH                           | LONG,   0x188F));

        for(int i = 0; i < instructions.size(); i++)
            instrMap.put(instructions.get(i).mnemonic, instructions.get(i));
    }

    public int getMode(String token) {

        return -1;
    }

    public boolean INH(HC11Instr instr) {
        return (instr.addr_modes & INH) == INH;
    }

    public boolean REL(HC11Instr instr) {
        return (instr.addr_modes & REL) == REL;
    }

    public boolean IMM(HC11Instr instr) {
        return (instr.addr_modes & IMM) == IMM;
    }

    public class HC11Instr {
        public String mnemonic;
        public int addr_modes;
        public int[] opcodes;

        public static final int REL  = 0;
        public static final int INH =  0;
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
