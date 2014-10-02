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

package plptool.mods;

import plptool.Constants;
import plptool.Msg;
import plptool.PLPSimBusModule;
import plptool.PLPToolbox;
import plptool.Text;

/**
 * PLP memory module. This module implements a wrapper read function that
 * typecasts stored objects to Long.
 *
 * @author wira
 */
public class MemModule extends PLPSimBusModule {
    /**
     * The constructor for the memory module takes the size of the memory
     * in bytes if it's word aligned (such as main memory), or in words
     * (such as register file).
     *
     * @param size Size of memory module. In bytes if aligned, in words otherwise.
     * @param wordAligned Denotes whether this memory module is word aligned
     */
    public MemModule(long addr, long size, boolean wordAligned) {
        super(addr, addr + (wordAligned ? size - 4: size - 1), wordAligned);
    }

    /**
     * The memory module should not have an evaluation procedure, this is here
     * to satisfy implementation of the PLPSimBusModule superclass. Fancier
     * memory modules can use this.
     *
     * @return PLP_OK
     */
    public int eval () { return Constants.PLP_OK; }

    /**
     * The memory module should not have an evaluation procedure, this is here
     * to satisfy implementation of the PLPSimBusModule superclass.
     *
     * @return PLP_OK
     */
    public int gui_eval(Object x) { return Constants.PLP_OK; }

    public String introduce() {
        return "Memory Module " + Text.versionString;
    }

    /**
     * Print the contents of this memory module.
     *
     * @param highlight Memory location to highlight, probably the PC value
     */
    public void printAll(long highlight) {
        long addr;
        Msg.p("->\taddress\t\tcontents\tASCII");
        Msg.p("--\t-------\t\t--------\t-----");
        Object[][] valueSet = super.getValueSet();
        for(int i = 0; i < valueSet.length; i++) {
            addr = (Long) valueSet[i][0];
            if(addr == highlight)
                Msg.pn(">>>");
            Msg.p(String.format("\t%08x\t%08x\t",
                                   addr, super.values.get(addr)) +
                                   PLPToolbox.asciiWord((Long) super.values.get(addr)));
        }
        Msg.P();
    }

    /**
     * Print the contents of the specified address
     *
     * @param addr
     */
    public void print(long addr) {
        if(super.isInitialized(addr)) {
            Msg.M(String.format("%08x\t%08x\t",addr, super.read(addr)) +
                                   PLPToolbox.asciiWord((Long) super.read(addr)));
        }
    }

    /**
     * Override super.read() so now it returns a Long
     *
     * @param addr Address to be read from
     * @return Returns Long
     */
    @Override public Long read(long addr) {
        return (Long) readReg(addr);
    }

    @Override public void reset() {
        super.clear();
    }

    @Override public String toString() {
        return "MemModule";
    }
}
