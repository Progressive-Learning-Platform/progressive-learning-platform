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

    public RuntimeProfile(Asm p) {
        this.p = p;
        BasicBlock head = new BasicBlock(p.getEntryPoint());
        blocks = new ArrayList<BasicBlock>();
        blocks.add(head);
        current = 0;
    }

    public void eval(long pc) {
        BasicBlock cur = blocks.get(current);



        cur.stats.CYCLES++;
    }

    public BasicBlock checkStartingAddress(long addr) {
        for(int i = 0; i < blocks.size(); i++)
            if(blocks.get(i).getStartingAddress() == addr) {
                current = i;
                return blocks.get(i);
            }
        return null;
    }
}
