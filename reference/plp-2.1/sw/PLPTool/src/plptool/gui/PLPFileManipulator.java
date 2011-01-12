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

package plptool.gui;

import plptool.Constants;

/**
 *
 * @author wira
 */
public class PLPFileManipulator {
    public static void CLI(String[] args) {
        PLPBackend backend = new PLPBackend(false, "plpmips");

	java.io.File plpHandler = new java.io.File(args[1]);
	
	if(plpHandler.exists())
            backend.openPLPFile(args[1]);
        else {
            backend.newPLPFile();
            backend.plpfile = args[1];
            if(backend.savePLPFile() != Constants.PLP_OK)
                return;
        }
        
        if(backend.plpfile == null || args.length <= 2)
            return;

        if(args[2].equals("-importasm") || args[2].equals("-i")) {
            backend.importAsm(args[3]);
        }
        else if(args[2].equals("-setmain") || args[2].equals("-sm")) {
            int main_index = Integer.parseInt(args[3]);
            if(main_index < 0 || main_index >= backend.asms.size())
                return;
            backend.main_asm = main_index;
        }
        else if(args[2].equals("-getmain") || args[2].equals("-gm")) {
            plptool.PLPMsg.I("MAINSRC=" + backend.main_asm, null);
	}
        
        backend.savePLPFile();
    }
}
