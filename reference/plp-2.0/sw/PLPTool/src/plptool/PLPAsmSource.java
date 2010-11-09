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
    private String          asmFilePath;

    int             recursionLevel;

    public PLPAsmSource(String strAsm, String strFilePath, int intLevel) {
        asmString = new String();
        asmFilePath = strFilePath;

        try {
        if(strAsm == null) {
            Scanner fScan = new Scanner(new File(strFilePath));
            while(fScan.hasNextLine())
                asmString += fScan.nextLine() + "\n";
            fScan.close();
        } else
            asmString = strAsm;

        recursionLevel = intLevel;

        } catch(Exception e) {
            PLPMsg.E("Error reading file",
                            PLPMsg.PLP_GENERIC_ERROR, this);
        }
    }

    public int setRefSource(PLPAsmSource plpRefSource) {
        refSource = plpRefSource;

        if(refSource != null)
            return 0;
        else {
            return PLPMsg.PLP_ASM_INVALID_REFSOURCE;
        }
    }

    public String getAsmFilePath() {
        return asmFilePath;
    }

    public String getAsmString() {
        return asmString;
    }

    @Override public String toString() {
        return "PLPAsmSource(" + this.asmFilePath + ")";
    }
}
