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

import java.util.Scanner;
import java.io.File;

/**
 * This class implements an assembly source and its data structures used by
 * PLPAssembler.
 *
 * @author wira
 */

public class PLPAsmSource {
    private PLPAsmSource    refSource;
    private String          asmString;
    private String[]        asmLines;
    private String          asmFilePath;
    private int             lastLineOpen;

    int             recursionLevel;

    public PLPAsmSource(String strAsm, String strFilePath, int intLevel) {
        asmFilePath = strFilePath;
        lastLineOpen = 0;

        try {
        if(strAsm == null) {
            java.io.File fHandler = new java.io.File(strFilePath);
            java.io.FileReader fReader = new java.io.FileReader(fHandler);
            char[] readBuffer = new char[(int) fHandler.length()];
            fReader.read(readBuffer, 0, readBuffer.length);
            asmString = String.copyValueOf(readBuffer);
            fReader.close();
        } else {
            asmString = strAsm;
        }
        
        asmLines = asmString.split("\\r?\\n");

        recursionLevel = intLevel;

        } catch(Exception e) {
            Msg.E("Error reading file",
                            Constants.PLP_GENERIC_ERROR, this);
        }
    }

    public int setRefSource(PLPAsmSource plpRefSource) {
        refSource = plpRefSource;

        if(refSource != null)
            return 0;
        else {
            return Constants.PLP_ASM_INVALID_REFSOURCE;
        }
    }

    public String getAsmFilePath() {
        return asmFilePath;
    }

    public String getAsmString() {
        return asmString;
    }

    public void setAsmString(String newStr) {
        asmString = newStr;
        asmLines = asmString.split("\\r?\\n");
    }

    public void setAsmFilePath(String newPath) {
        asmFilePath = newPath;
    }

    public String getAsmLine(int lineNum) {
        if(lineNum > asmLines.length || lineNum <= 0) {
            Msg.E("Invalid line number: " + lineNum,
                     Constants.PLP_ASM_INVALID_LINENUM, this);
            return null;
        }

        return asmLines[lineNum - 1];
    }

    public void setLastLine(int lineNumber) {
        lastLineOpen = lineNumber;
    }

    public int getLastLine() {
        return lastLineOpen;
    }

    @Override public String toString() {
        return "PLPAsmSource(" + this.asmFilePath + ")";
    }
}
