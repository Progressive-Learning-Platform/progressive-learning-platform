/*
    Copyright 2012 PLP Contributors

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

package plptool.extras.execviz;

import plptool.mips.*;
import plptool.extras.execviz.program.*;

import java.util.ArrayList;

/**
 *
 * @author Wira
 */
public class RuntimeProfile {
    Asm p;
    ArrayList<BasicBlock> blocks;
    int current;

    private final byte BEQ  = 0x04;
    private final byte BNE  = 0x05;
    private final byte J    = 0x02;
    private final byte JAL  = 0x03;
    private final byte JR   = 0x08;
    private final byte JALR = 0x09;

    public RuntimeProfile(Asm p) {
        this.p = p;
        BasicBlock head = new BasicBlock(p.getEntryPoint());
        blocks = new ArrayList<BasicBlock>();
        blocks.add(head);
        current = 0;
    }

    public void eval(long pc) {
        BasicBlock t;
        BasicBlock cur = blocks.get(current);
        int index = p.lookupAddrIndex(pc);
   
        if(index < 0)
            return;
        else if((t = checkStartingAddress(pc)) != null) {
            t.stats.VISITS++;
            cur = t;
        } else if((t = checkAddress(pc)) != null)
            cur = t;
        else if(p.lookupLabel(pc) != null) {
            cur = new BasicBlock(pc);
            blocks.add(cur);
            current = blocks.size()-1;
            cur.stats.VISITS++;
        }

        cur.stats.CYCLES++;
        long instr  = p.getObjectCode()[index];
        byte opcode = MIPSInstr.opcode(instr);
        byte funct  = MIPSInstr.funct(instr);
        long dest;
        int old_current = current;
        cur.addAddress(pc);

        if(!cur.hasBranch() && (opcode == BEQ || opcode == BNE)) {
            dest = MIPSInstr.resolve_baddr(pc, instr);
            if((t = checkStartingAddress(dest)) != null) {
                cur.setBranchingNode(new DecisionNode(t));
                current = old_current;
            } else if((t = checkAddress(dest)) != null) {
                t = new BasicBlock(dest);
                blocks.add(t);
            }
            cur.setBranchingNode(new DecisionNode(t));
            if(checkAddress(pc+8) == null)
                blocks.add(new BasicBlock(pc+8));
        } else if(opcode == J) {
            dest = MIPSInstr.resolve_jaddr(pc, instr);
            if(checkStartingAddress(dest) != null)
                return;
            BasicBlock jdest = new BasicBlock(dest);
            cur.setNext(jdest);
            blocks.add(jdest);
            current = blocks.size() - 1;
        } else if(opcode == JAL) {

        } else if(opcode == 0 && funct == JR) {

        } else if(opcode == 0 && funct == JALR) {

        } else {

        }
    }

    public void traverse(int counter, long pc) {
        int index;
        long instr;
        long dest;
        byte opcode, funct;
        BasicBlock cur;

        while(true) {
            
        }
    }

    public BasicBlock checkStartingAddress(long addr) {
        for(int i = 0; i < blocks.size(); i++)
            if(blocks.get(i).getStartingAddress() == addr) {
                current = i;
                return blocks.get(i);
            }
        return null;
    }

    public BasicBlock checkAddress(long addr) {
        for(int i = 0; i < blocks.size(); i++)
            if(blocks.get(i).hasAddress(addr)) {
                current = i;
                return blocks.get(i);
            }        
        return null;
    }

    public BasicBlock getBasicBlock(int index) {
        return blocks.get(index);
    }

    public int getNumberofBasicBlocks() {
        return blocks.size();
    }

    public String getBasicBlockCode(int index) {
        String ret = "";
        BasicBlock b = blocks.get(index);
        ArrayList<Long> addresses = b.getAddresses();
        long addr;
        int[] fIndex = new int[2];
        int[] prev = {-1, -1};
        for(int i = 0; i < addresses.size(); i++) {
            addr = addresses.get(i);
            fIndex[0] = p.getFileIndex(addr);
            fIndex[1] = p.getLineNum(addr);
            if(prev[0] != fIndex[0] && prev[1] != fIndex[1]) {
                prev[0] = fIndex[0];
                prev[1] = fIndex[1];
                ret += p.getAsmList().get(fIndex[0]).getAsmLine(fIndex[1]) + "\n";
            }
        }

        return ret;
    }

    public String lookupLabel(int index) {
        return p.lookupLabel(blocks.get(index).getStartingAddress());
    }

    public int getCurrent() {
        return current;
    }
}
