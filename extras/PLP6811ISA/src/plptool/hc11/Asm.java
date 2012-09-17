/*
    Copyright 2012 David Fritz, Brian Gordon, Wira Mulia

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

package plptool.hc11;

import plptool.*;
import plptool.gui.ProjectDriver;
import java.util.ArrayList;

/**
 *
 * @author wira
 */
public class Asm extends PLPAsm {
    private String[] lines;

    int PC;

    public Asm(ArrayList<PLPAsmSource> asms) {
        super(asms);
    }

    public int assemble() {
        
        setAssembled(true);
        return Constants.PLP_OK;
    }

    public int preprocess(int index) {
        PC = 0x0000;

        /* In the first pass, we populate our symbol table and map addresses to
         * the corresponding source file and line number. For this, we also need
         * to figure out instruction lengths (damn variable length instructions!)
         */
        for(int i = 0; i < sourceList.size(); i++) {
            lines = sourceList.get(i).getAsmString().split("\\r?\\n");

            for(int l = 0; l < lines.length; l++) {
                
            }
        }

        return Constants.PLP_OK;
    }

    public String[] getLines() {
        return lines;
    }
}
