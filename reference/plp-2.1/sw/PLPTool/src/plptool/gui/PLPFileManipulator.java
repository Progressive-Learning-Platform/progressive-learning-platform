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
import plptool.PLPMsg;

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
            if(!(args.length == 4)) {
                PLPMsg.E("No file specified.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            backend.importAsm(args[3]);
            backend.savePLPFile();
        }
        if(args[2].equals("-importdir") || args[2].equals("-d")) {
            if(!(args.length == 4)) {
                PLPMsg.E("No file specified.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            java.io.File dir = new java.io.File(args[3]);

            String files[] = dir.list();

            for(int i = 0; i < files.length; i++)
                backend.importAsm(dir.getAbsolutePath() + "/" + files[i]);
            backend.savePLPFile();
        }
        else if((args[2].equals("-setmain") || args[2].equals("-sm"))) {
            if(!(args.length == 4)) {
                PLPMsg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            int main_index = Integer.parseInt(args[3]);
            if(main_index < 0 || main_index >= backend.asms.size())
                return;
            backend.main_asm = main_index;
            backend.savePLPFile();
        }
        else if((args[2].equals("-getmain") || args[2].equals("-gm"))) {
            PLPMsg.I("MAINSRC=" + backend.main_asm, null);
	}
        else if((args[2].equals("-r"))) {
            if(!(args.length == 4)) {
                PLPMsg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            int index = Integer.parseInt(args[3]);
            backend.removeAsm(index);
            backend.savePLPFile();
        }
        else if((args[2].equals("-e"))) {
            if(!(args.length == 5)) {
                PLPMsg.E("Missing argument(s).", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            int index = Integer.parseInt(args[3]);
            backend.exportAsm(index, args[4]);
        }
        else {
            PLPMsg.I("Invalid option: " + args[2], null);
            return;
        }         
    }
}
