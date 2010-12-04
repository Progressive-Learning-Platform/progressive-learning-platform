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
 * PLP memory module. This module implements a wrapper read function that
 * typecasts stored objects to Long.
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
        long addr;
        PLPMsg.M("->\taddress\t\tcontents\tASCII");
        PLPMsg.M("--\t-------\t\t--------\t-----");
        Object[][] valueSet = super.getValueSet();
        for(int i = 0; i < valueSet.length; i++) {
            addr = (Long) valueSet[i][0];
            if(addr == highlight)
                System.out.print(">>>");
            PLPMsg.M(String.format("\t%08x\t%08x\t",
                                   addr, super.values.get(addr)) +
                                   PLPToolbox.asciiWord((Long) super.values.get(addr)));
        }
    }

    /**
     * Print the contents of the specified address
     *
     * @param addr
     */
    public void print(long addr) {
        if(super.isInitialized(addr)) {
            PLPMsg.M("\naddress\t\tcontents\tASCII");
            PLPMsg.M("-------\t\t--------\t-----");
            PLPMsg.M(String.format("%08x\t%08x\t",addr, super.read(addr)) +
                                   PLPToolbox.asciiWord((Long) super.read(addr)));
        }
    }

    @Override public String toString() {
        return "PLPSimMemModule";
    }
}
