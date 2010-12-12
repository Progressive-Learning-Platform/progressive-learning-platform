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

import java.util.HashMap;
import java.util.ArrayList;
import java.util.Iterator;

/**
 * PLPTool assembler abstract.
 *
 * @author wira
 */
public abstract class PLPAsmX {

    public PLPAsmX(long[] addrTable, long[] objectCode) {
        if(addrTable != null)
            this.addrTable = addrTable;
        if(objectCode != null)
            this.objectCode = objectCode;
    }
    /**
     * The address table attached to this assembler.
     *
     * @see objectCode[]
     */
    protected long[]  addrTable;

    /**
     * The object code attached to this assembler.
     *
     * @see preprocess(int)
     * @see assemble()
     * @see getObjectCode()
     */
    protected long[]  objectCode;

    /**
     * The region of which each object code entry belongs to. This is
     * implementation specific and used by the linker to generate final
     * program.
     */
    protected ArrayList<Integer>   regionMap;

    /**
     * Assembler symbol table.
     *
     * @see objectCode[]
     */
    protected HashMap<String, Long> symTable;

    abstract public int preprocess(int index);
    
    abstract public int assemble();

    /**
     * Returns the object code array attached to this assembler object.
     *
     * @return Returns the object code as array of longs.
     */
    public long[] getObjectCode() {
        return objectCode;
    }

    /**
     * Returns the instruction addresses array attached to this assembler object.
     *
     * @return Returns the address table as array of longs.
     */
    public long[] getAddrTable() {
        return addrTable;
    }

    /**
     * Returns the symbol table attached to this assembler object.
     *
     * @return Returns the symbol table as a HashMap.
     */
    public HashMap getSymTable() {
        return symTable;
    }

    /**
     * Returns the region map
     *
     * @return The region map
     */
    public ArrayList<Integer> getRegionMap() {
       return regionMap;
    }

    /**
     * This variable describes the state of the asm sources attached to this
     * assembler. This variable is set true when all assembly sources were
     * successfully assembled and objectCode[], addrtable[] and symTable are
     * populated.
     */
    protected boolean     assembled;

    /**
     * Lookup if the provided memory address has a label attached to it in the
     * symbol table.
     *
     * @return Returns the label in String if found, null otherwise
     * @param address memory address to look up
     */
    public String lookupLabel(long address) {
        String key;

        if(symTable.containsValue(address)) {
            Iterator iterator = symTable.keySet().iterator();
            while(iterator.hasNext()) {
                key = (String) iterator.next();
                if(symTable.get(key).equals(address)) {
                    return key;
                }
            }
        }
        return null;
    }

    /**
     * Return the address associated with the given label, if it exists
     *
     * @param label Label to look up
     * @return The address associated with the label, or PLP_ERROR_RETURN if
     * it's not in the symtable
     */
    public long resolveAddress (String label) {
        if(symTable.containsKey(label)) {
            return symTable.get(label);
        }
        return Constants.PLP_ERROR_RETURN;
    }

    /**
     * Returns whether the source files attached to this assembler have
     * been successfully assembled.
     *
     * @return Returns boolean true if all sources are assembled, false
     * otherwise
     */
    public boolean isAssembled() {
        return assembled;
    }
}
