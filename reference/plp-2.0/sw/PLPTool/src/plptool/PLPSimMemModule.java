/*
    Copyright 2010 David Fritz, Brian Gordon, Wira Mulia

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

package plptool;

import java.util.Iterator;

/**
 * PLP memory module.
 *
 * @author wira
 */
public class PLPSimMemModule extends PLPSimBusModule {
    public PLPSimMemModule(long size, boolean wordAligned) {
        super(0, size, wordAligned);
    }

    // memory module doesn't need eval, just return OK.
    public int eval () { return PLPMsg.PLP_OK; }
    public int gui_eval(Object x) { return PLPMsg.PLP_OK; }


    public String introduce() {
        return "PLPSimMemModule " + PLPMsg.versionString;
    }

    public void printAll(long highlight) {
        long key;
        PLPMsg.M("->\taddress\t\tcontents\tASCII");
        PLPMsg.M("--\t-------\t\t--------\t-----");
        Iterator keyIterator = super.values.keySet().iterator();
        while(keyIterator.hasNext()) {
            key = (Long) keyIterator.next();
            if(wordAligned) {
                if(key * 4 == highlight)
                    System.out.print(">>>");
                PLPMsg.M(String.format("\t%08x\t%08x\t",
                                       key * 4, super.values.get(key)) +
                                       PLPAsmFormatter.asciiWord(super.values.get(key)));
            }
            else {
                if(key == highlight)
                    System.out.print(">>>");
                PLPMsg.M(String.format("\t%08x\t%08x\t",
                                        key, super.values.get(key)) +
                                        PLPAsmFormatter.asciiWord(super.values.get(key)));
            }
        }
    }

    public void printProgram(long highlight) {
        if(wordAligned) {
            long key;
            PLPMsg.M("pc\taddress\t\thex\t\tDisassembly");
            PLPMsg.M("--\t-------\t\t---\t\t-----------");
            Iterator keyIterator = super.values.keySet().iterator();
            while(keyIterator.hasNext()) {
                key = (Long) keyIterator.next();
                if(super.isInstr.get(key) == true) {
                    if(key * 4 == highlight)
                        System.out.print(">>>");
                    PLPMsg.M(String.format("\t%08x\t%08x\t",
                                           key * 4, super.values.get(key)) +
                                           MIPSInstr.format(super.values.get(key)));
                }
            }
        }
    }

    public void print(long addr) {
        if(wordAligned) {
            PLPMsg.M("\naddress\t\tcontents\tASCII");
            PLPMsg.M("-------\t\t--------\t-----");
            PLPMsg.M(String.format("%08x\t%08x\t" +
                                   PLPAsmFormatter.asciiWord(super.read(addr)),
                                   addr, super.read(addr)));
        }
        else {
            PLPMsg.M("\naddress\t\tcontents\tASCII");
            PLPMsg.M("-------\t\t--------\t-----");
            PLPMsg.M(String.format("%08x\t%08x\t" +
                                   PLPAsmFormatter.asciiWord(super.read(addr)),
                                   addr, super.read(addr)));
        }
    }

    @Override public String toString() {
        return "PLPMIPSSim.mod_memory";
    }
}
