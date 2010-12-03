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
 * PLP memory module. It's simply PLPSimBusModule with pretty printouts.
 *
 * @author wira
 */
public class PLPSimMemModule extends PLPSimBusModule {
    /**
     * The constructor for the memory module takes the size of the memory
     * in bytes if it's word aligned (such as main memory), or in words
     * (such as register file).
     *
     * @param size Size of memory module. In bytes if aligned, in words otherwise.
     * @param wordAligned Denotes whether this memory module is word aligned
     */
    public PLPSimMemModule(long size, boolean wordAligned) {
        super(0, wordAligned ? size - 4: size - 1, wordAligned);
    }

    /**
     * The memory module should not have an evaluation procedure, this is here
     * to satisfy implementation of the PLPSimBusModule superclass. Fancier
     * memory modules can use this.
     *
     * @return PLP_OK
     */
    public int eval () { return PLPMsg.PLP_OK; }

    /**
     * The memory module should not have an evaluation procedure, this is here
     * to satisfy implementation of the PLPSimBusModule superclass.
     *
     * @return PLP_OK
     */
    public int gui_eval(Object x) { return PLPMsg.PLP_OK; }


    public String introduce() {
        return "PLPSimMemModule " + PLPMsg.versionString;
    }

    /**
     * Print the contents of this memory module.
     *
     * @param highlight Memory location to highlight, probably the PC value
     */
    public void printAll(long highlight) {
        long key;
        PLPMsg.M("->\taddress\t\tcontents\tASCII");
        PLPMsg.M("--\t-------\t\t--------\t-----");
        Iterator keyIterator = super.values.keySet().iterator();
        while(keyIterator.hasNext()) {
            key = (Long) keyIterator.next();
            if(key == highlight)
                System.out.print(">>>");
            PLPMsg.M(String.format("\t%08x\t%08x\t",
                                   key, super.values.get(key)) +
                                   PLPAsmFormatter.asciiWord((Long) super.values.get(key)));
        }
    }

    /**
     * Print the contents of this memory module formatted as MIPS program.
     *
     * @param highlight Memory location to highlight, probably the PC value
     */
    public void printProgram(long highlight) {
        if(wordAligned) {
            long key;
            PLPMsg.M("pc\taddress\t\thex\t\tDisassembly");
            PLPMsg.M("--\t-------\t\t---\t\t-----------");
            Iterator keyIterator = super.values.keySet().iterator();
            while(keyIterator.hasNext()) {
                key = (Long) keyIterator.next();
                if(super.isInstr.get(key) == true) {
                    if(key == highlight)
                        System.out.print(">>>");
                    PLPMsg.M(String.format("\t%08x\t%08x\t",
                                           key, super.values.get(key)) +
                                           MIPSInstr.format((Long) super.values.get(key)));
                }
            }
        }
    }

    /**
     * Print the contents of the specified address
     *
     * @param addr
     */
    public void print(long addr) {
        long value = (Long) super.read(addr);

        if(value >= 0) {
            PLPMsg.M("\naddress\t\tcontents\tASCII");
            PLPMsg.M("-------\t\t--------\t-----");
            PLPMsg.M(String.format("%08x\t%08x\t",addr, value) +
                                   PLPAsmFormatter.asciiWord(value));
        }
    }

    @Override public String toString() {
        return "PLPSimMemModule";
    }
}
