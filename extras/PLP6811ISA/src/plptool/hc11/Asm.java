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
    public static final int INH        = 0x01;
    public static final int IMM        = 0x02;
    public static final int DIR        = 0x04;
    public static final int EXT        = 0x08;
    public static final int INDX       = 0x10;
    public static final int INDY       = 0x20;
    public static final int REL        = 0x40;

    public static final int REGA       = 0x80;
    public static final int REGB       = 0x100;
    public static final int MASK       = 0x200;
    public static final int INHY       = 0x400;

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
        instructions.add(new HC11Instr("ABY",   INHY                                ,   0x183A));

        instructions.add(new HC11Instr("ADCA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x89, 0x99, 0xB9, 0xA9, 0x18A9));
        instructions.add(new HC11Instr("ADCB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC9, 0xD9, 0xF9, 0xE9, 0x18E9));

        instructions.add(new HC11Instr("ADDA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x8B, 0x9B, 0xBB, 0xAB, 0x18AB));
        instructions.add(new HC11Instr("ADDB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xCB, 0xDB, 0xFB, 0xEB, 0x18EB));
        instructions.add(new HC11Instr("ADDD",  IMM | DIR | EXT | INDX | INDY       ,   0xC3, 0xD3, 0xF3, 0xE3, 0x18E3));

        instructions.add(new HC11Instr("ANDA",  IMM | DIR | EXT | INDX | INDY | REGA,   0x84, 0x94, 0xB4, 0xA4, 0x18A4));
        instructions.add(new HC11Instr("ANDB",  IMM | DIR | EXT | INDX | INDY | REGB,   0xC4, 0xD4, 0xF4, 0xE4, 0x18E4));

        instructions.add(new HC11Instr("ASL",               EXT | INDX | INDY       ,      -1,   -1,   0x78, 0x68, 0x1868));
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

        for(int i = 0; i < instructions.size(); i++)
            instrMap.put(instructions.get(i).mnemonic, instructions.get(i));
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
