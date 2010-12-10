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

/**
 * PLPTool assembler abstract.
 *
 * @author wira
 */
public abstract class PLPAsmX {
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
}
