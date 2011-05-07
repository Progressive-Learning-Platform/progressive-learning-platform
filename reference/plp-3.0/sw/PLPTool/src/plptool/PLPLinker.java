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

import java.util.ArrayList;

/** 2.3.
 *
 * PLPTool Linker. Slated for PLP 2.3.
 *
 * @author wira
 */
public class PLPLinker {
    public int link(PLPAsm[] asmObjs, long[] regionMap) {
        int i, j, ret;
        int[] regionMapCounters = new int[regionMap.length];

        int[][] asmRegionLengths = new int[asmObjs.length][regionMap.length];

        for(i = 0; i < asmObjs.length; i++) {
            if((ret = asmObjs[i].preprocess(0)) != Constants.PLP_OK)
                return ret;
            for(j = 0; j < asmObjs[i].getRegionMap().size(); j++) {
                if(asmObjs[i].getRegionMap().get(j) > regionMap.length)
                    return Constants.PLP_ERROR_RETURN;

                asmRegionLengths[i][asmObjs[i].getRegionMap().get(j)]++;
            }
        }

        return Constants.PLP_OK;
    }
}
