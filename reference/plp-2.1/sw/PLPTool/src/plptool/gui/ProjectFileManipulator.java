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

import java.io.*;

/**
 *
 * @author wira
 */
public class ProjectFileManipulator {
    public static void CLI(String[] args) {
        ProjectDriver plp = new ProjectDriver(false, "plpmips");
	
        if(args == null || args.length < 2) 
            return;

        File plpHandler = new File(args[1]);
	
	if(plpHandler.exists())
            plp.open(args[1]);
        else if(args.length == 2) {
            plp.create();
            plp.plpfile = args[1];
            if(plp.save() != Constants.PLP_OK)
                return;
        } else
            plp.plpfile = args[1];
        
        if(plp.plpfile == null || args.length <= 2)
            return;

        if(args[2].equals("-importasm") || args[2].equals("-i")) {
            if(!(args.length == 4)) {
                PLPMsg.E("No file specified.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            String temp = plp.plpfile;

            if(!plpHandler.exists())
                plp.create(args[3]);
            else
                plp.importAsm(args[3]);

            plp.plpfile = temp;
            plp.save();
        }
        else if(args[2].equals("-c")) {
            if(!(args.length == 4)) {
                PLPMsg.E("No file specified.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            plp.create(args[3]);
            plp.plpfile = args[1];
            plp.save();
        }
        else if(args[2].equals("-importdir") || args[2].equals("-d")) {
            if(!(args.length == 4)) {
                PLPMsg.E("No file specified.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            File dir = new File(args[3]);

            String files[] = dir.list();

            for(int i = 0; i < files.length; i++)
                plp.importAsm(dir.getAbsolutePath() + "/" + files[i]);
            plp.save();
        }
        else if((args[2].equals("-setmain") || args[2].equals("-s"))) {
            if(!(args.length == 4)) {
                PLPMsg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            int main_index = Integer.parseInt(args[3]);
            if(main_index < 0 || main_index >= plp.asms.size())
                return;
            plp.main_asm = main_index;
            plp.save();
        }
        else if(args[2].equals("-v")) {
            if(!(args.length == 4)) {
                PLPMsg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            int index = Integer.parseInt(args[3]);
            if(index < 0 || index >= plp.asms.size())
                return;
            PLPMsg.M(plp.getAsm(index).getAsmString());
        }

        else if(args[2].equals("-m")) {
            PLPMsg.I("Metafile contents:", null);
            PLPMsg.M(plp.meta);
	}
        else if((args[2].equals("-r"))) {
            if(!(args.length == 4)) {
                PLPMsg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            int index = Integer.parseInt(args[3]);
            plp.removeAsm(index);
            plp.save();
        }
        else if((args[2].equals("-e"))) {
            if(!(args.length == 5)) {
                PLPMsg.E("Missing argument(s).", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            int index = Integer.parseInt(args[3]);
            plp.exportAsm(index, args[4]);
        }
        else if((args[2].equals("-edit"))) {
            if(!(args.length == 4)) {
                PLPMsg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }
 
            try {

            int index = Integer.parseInt(args[3]);
            File temp = new File("PLPTool.temp.asm");
            if(temp.exists()) 
                temp.delete();
            plp.exportAsm(index, "PLPTool.temp.asm");
            String file = plp.getAsm(index).getAsmFilePath();
            Process p = Runtime.getRuntime().exec("gedit PLPTool.temp.asm");
            p.waitFor();
            File newFile = new File(file);
            temp.renameTo(newFile);
            plp.removeAsm(index);
            plp.importAsm(newFile.getAbsolutePath());
            newFile.delete();
            plp.save();
            } catch(Exception e) { e.printStackTrace(); }
        }

        else if(args[2].equals("-a")) {
            plp.save();
            if(plp.asm.isAssembled() && plp.getArch().equals("plpmips")) {
                plptool.mips.Formatter.symTablePrettyPrint(plp.asm.getSymTable());
                PLPMsg.M("");
                plptool.mips.Formatter.prettyPrint((plptool.mips.Asm) plp.asm);
            }
        }
        else if(args[2].equals("-p")) {
            if(!(args.length == 4)) {
                PLPMsg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            plp.assemble();
            if(plp.asm != null && plp.asm.isAssembled())
                plp.program(args[3]);
        }

        else {
            PLPMsg.I("Invalid option: " + args[2], null);
            return;
        }         
    }
}
