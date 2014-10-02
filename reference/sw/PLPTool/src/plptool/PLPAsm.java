/*
    Copyright 2010-2014 David Fritz, Brian Gordon, Wira Mulia

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
    protected ArrayList<PLPAsmSource>  sourceList;

    protected int         mapperIndex;
    protected int[]       lineNumMap;
    protected int[]       asmFileMap;
    protected int[]       objCodeFileMapper;
    protected int[]       objCodeLineNumMapper;

    /**
     * Entry point to the assembled program to be optionally used by the rest
     * of the framework
     */
    protected long          entryPoint;

    /**
     * First pass string builder. Use this object for faster string
     * manipulation such as appending preprocessed text
     */
    protected StringBuilder pass1Str;

    /**
     * Optional preprocessed assembly string to be used by assemble()
     */
    protected String        preprocessedAsm;

    /**
     * An array list of build error objects that can be used to relate back
     * to the source file index and the line number where the source of the
     * error is
     */
    protected ArrayList<PLPBuildError> errorList;

    /**
     * PLPAsm defaults to returning the current version of PLPTool.
     * User assemblers can override this. May be useful for dynamic modules
     *
     * @return String of PLPTool version
     */
    public String getVersion() { return Text.versionString; }

    /**
     * Constructor for the assembler object that takes a string of assembly
     * and informational assembly file path. This constructor will create and
     * populate the source list with one assembly source
     *
     * @param strAsm Source string
     * @param strFilePath Source path information
     */
    public PLPAsm(String strAsm, String strFilePath) {
        PLPAsmSource plpAsmObj = new PLPAsmSource(strAsm, strFilePath, 0);
        sourceList = new ArrayList<PLPAsmSource>();
        sourceList.add(plpAsmObj);
        pass1Str = new StringBuilder();
        preprocessedAsm = new String();
        symTable = new HashMap<String, Long>();

        asmIndex = 0;
        mapperIndex = 0;
        assembled = false;
    }

    /**
     * Constructor for the assembler object that takes an arraylist of
     * PLPAsmSource objects
     *
     * @param asms List ouf assembly sources
     */
    public PLPAsm(ArrayList<PLPAsmSource> asms) {
        sourceList = asms;
        pass1Str = new StringBuilder();
        preprocessedAsm = new String();
        symTable = new HashMap<String, Long>();

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

    /**
     * Call the preprocess routine of the assembler
     *
     * @param index Source file index to start preprocess with
     * @return PLP_OK or error code
     */
    abstract public int preprocess(int index);

    /**
     * Call the assemble routine of the assembler
     *
     * @return PLP_OK or error code
     */
    abstract public int assemble();

    /**
     * Append the preprocessed assembly string. This method also updates
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
     * Return the object code array attached to this assembler object.
     *
     * @return Returns the object code as array of longs.
     */
    public long[] getObjectCode() {
        return objectCode;
    }

    /**
     * Return the instruction addresses array attached to this assembler object.
     *
     * @return Returns the address table as array of longs.
     */
    public long[] getAddrTable() {
        return addrTable;
    }

    /**
     * Return the symbol table attached to this assembler object.
     *
     * @return Returns the symbol table as a HashMap.
     */
    public HashMap<String, Long> getSymTable() {
        return symTable;
    }

    /**
     * Get the file mapper array. The index of the array is where the object
     * code is stored in the object code array. The value is the source file
     * ID when assembly was done.
     *
     * @return An array mapping the object code index to the source file ID
     */
    public int[] getFileMapper() {
        return objCodeFileMapper;
    }

    /**
     * Get the line number array. The index of the array is where the object
     * code is stored in the object code array. The value is the line number
     * where this instruction came from.
     *
     * @return An array mapping the object code index to the source file
     * line number
     */
    public int[] getLineNumMapper() {
        return objCodeLineNumMapper;
    }

    /**
     * Get the index of the source file where the instruction specified
     * by the address came from.
     *
     * @param addr Address of the instruction
     * @return The index of the source file where the instruction came from
     */
    public int getFileIndex(long addr) {
        if(addrTable == null) return -1;

        for(int i = 0; i < addrTable.length; i++) {
            if(addrTable[i] == addr)
                return objCodeFileMapper[i];
        }

        return -1;

    }

    /**
     * Get the line number of the code where the instruction specified
     * by the address came from
     *
     * @param addr Address of the instruction
     * @return The index of the source file where the instruction came from
     */
    public int getLineNum(long addr) {
        if(addrTable == null) return -1;

        for(int i = 0; i < addrTable.length; i++) {
            if(addrTable[i] == addr)
                return objCodeLineNumMapper[i];
        }

        return -1;
    }

    /**
     * Return the address of a given line of code in a source file
     *
     * @param fileIndex The index of the source file
     * @param lineNum The line number of the code
     * @return The address of the resulting instruction, or -1 if the search
     * fails
     */
    public long getAddrFromFileMetadata(int fileIndex, int lineNum) {
        if(!isAssembled())    return -1;
        if(addrTable == null) return -1;

        int i = 0;
        while(i < objCodeFileMapper.length) {
            if(objCodeFileMapper[i] == fileIndex && objCodeLineNumMapper[i] == lineNum)
                return addrTable[i];
            i++;
        }

        return -1;
    }

    /**
     * Attach a new symbol table to this assembler object
     *
     * @param symTable New symbol table to attach
     */
    public void setSymTable(HashMap<String, Long> symTable) {
        this.symTable = symTable;
    }

    /**
     * Return the region map
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
        if(symTable == null) return null;

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
        if(symTable == null) return Constants.PLP_ERROR_RETURN;

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
        if(addrTable == null) return -1;

        for(int i = 0; i < addrTable.length; i++) {
            if(addrTable[i] == addr)
                return i;
        }

        return -1;
    }

    /**
     * Return whether the source files attached to this assembler have
     * been successfully assembled.
     *
     * @return Returns boolean true if all sources are assembled, false
     * otherwise
     */
    public boolean isAssembled() {
        return assembled;
    }

    /**
     * Return the sourceList attached to this assembly.
     *
     * @return Returns sourceList as ArrayList
     */
    public ArrayList<PLPAsmSource> getAsmList() {
        return sourceList;
    }

    /**
     * Get entry point of the program (first .text <addr> directive)
     *
     * @return Entry point address
     */
    public long getEntryPoint() {
        return entryPoint;
    }

    /**
     * Force this assembler object to be marked assembled or not
     *
     * @param b
     */
    public void setAssembled(boolean b) {
        assembled = b;
    }

    /**
     * Other ISA-specific checks that the develop may wish to do. Any
     * return value other than PLP_OK will prevent the start of the simulation
     *
     * @return ProjectDriver expects PLP_OK to continue with the simulation
     */
    public int preSimulationCheck() { return Constants.PLP_OK; }

    /**
     * Get the build error list from this assembler object
     *
     * @return An array list of the build error objects
     */
    public final ArrayList<PLPBuildError> getErrorList() {
        return errorList;
    }

    /**
     * Overridable developer-specified generic hook.
     *
     * @param param An object to pass to the hook
     * @return A reference to an object returned from the hook function
     */
    public Object hook(Object param) {return null;};
}
