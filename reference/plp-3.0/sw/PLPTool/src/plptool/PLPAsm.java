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
public abstract class PLPAsm {

    /**
     * Assembly source files attached to this assembler
     */
    protected ArrayList<PLPAsmSource>  SourceList;

    protected int         mapperIndex;
    protected int[]       lineNumMap;
    protected int[]       asmFileMap;
    protected int[]       objCodeFileMapper;
    protected int[]       objCodeLineNumMapper;

    protected StringBuilder pass1Str;
    protected String        preprocessedAsm;

    public PLPAsm(String strAsm, String strFilePath) {
        PLPAsmSource plpAsmObj = new PLPAsmSource(strAsm, strFilePath, 0);
        SourceList = new ArrayList<PLPAsmSource>();
        SourceList.add(plpAsmObj);
        preprocessedAsm = new String();

        asmIndex = 0;
        mapperIndex = 0;
        assembled = false;
    }

    public PLPAsm(ArrayList<PLPAsmSource> asms) {
        SourceList = asms;
        preprocessedAsm = new String();

        asmIndex = 0;
        mapperIndex = 0;
        assembled = false;
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
     * Current index of assembly file
     */
    protected int         asmIndex;

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
     * Appends the preprocessed assembly string. This method also updates
     * the assembly file and line number mapping for error reporting purposes.
     *
     * @param index The 'preprocessed assembly' line number.
     * @param str Text to add to the string.
     * @param lineNum The line number of the assembly file currently being
     * preprocessed.
     */
    protected void appendPreprocessedAsm(String str, int lineNum, boolean newline) {

        // running out of mapper space? This is a complete hack!

        if(mapperIndex >= lineNumMap.length) {
            int[] newLineNumMap = new int[2 * lineNumMap.length];
            System.arraycopy(lineNumMap, 0, newLineNumMap, 0, lineNumMap.length);
            lineNumMap = newLineNumMap;
            int[] newAsmFileMap = new int[2 * lineNumMap.length];
            System.arraycopy(asmFileMap, 0, newAsmFileMap, 0, asmFileMap.length);
            asmFileMap = newAsmFileMap;

            Msg.D("new lineNumMap length: " + lineNumMap.length, 5, this);
        }

        lineNumMap[mapperIndex] = lineNum;
        asmFileMap[mapperIndex] = asmIndex;
        if(newline)
            mapperIndex++;
        pass1Str.append(str);
        pass1Str.append((newline ? "\n" : ""));
    }

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

    public int[] getFileMapper() {
        return objCodeFileMapper;
    }

    public int[] getLineNumMapper() {
        return objCodeLineNumMapper;
    }

    /**
     * Attaches a new symbol table to this assembler object
     *
     * @param symTable New symbol table to attach
     */
    public void setSymTable(HashMap<String, Long> symTable) {
        this.symTable = symTable;
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
     * Return the index associated with the given address. Can be used to
     * trace back an address to the source file
     *
     * @param addr Address to look up
     * @return Index where that address resides in the address table
     */
    public int lookupAddrIndex(long addr) {
        for(int i = 0; i < addrTable.length; i++) {
            if(addrTable[i] == addr)
                return i;
        }

        return -1;
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

    /**
     * Returns the SourceList attached to this assembly.
     *
     * @return Returns SourceList as ArrayList
     */
    public ArrayList<PLPAsmSource> getAsmList() {
        return SourceList;
    }

    public void setAssembled(boolean b) {
        assembled = b;
    }
}
