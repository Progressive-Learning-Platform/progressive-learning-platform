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
import plptool.Msg;

import java.io.*;

/**
 *
 * @author wira
 */
public class ProjectFileManipulator {
    public static void CLI(String[] args) {
        ProjectDriver plp = new ProjectDriver(false, "plpmips");
	
        if(args == null || args.length < 2) {
            helpMessage();
            return;
        }

        File plpHandler = new File(args[1]);
	
	if(plpHandler.exists())
            plp.open(args[1]);
        else if(args.length == 2) {
            plp.create();
            plp.plpfile = new File(args[1]);
            if(plp.save() != Constants.PLP_OK)
                return;
        } else
            plp.plpfile = new File(args[1]);
        
        if(plp.plpfile == null || args.length <= 2)
            return;

        if(args[2].equals("-importasm") || args[2].equals("-i")) {
            if((args.length < 4)) {
                Msg.E("No file specified.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            String temp = plp.plpfile.getAbsolutePath();

            for(int i = 3; i < args.length; i++)
                if(!plpHandler.exists()) {
                    if(args[i].endsWith(".plp")) {
                        ProjectDriver tempPlp = new ProjectDriver(false, "plpmips");
                        tempPlp.open(args[i]);
                    }
                    else if(i == 3)
                        plp.create(args[i]);
                    else
                        plp.importAsm(args[i]);
                }
                else {
                    if(args[i].endsWith(".plp")) {
                    }
                    else
                    plp.importAsm(args[i]);
                }

            plp.plpfile = new File(temp);
            plp.save();
        }
        else if(args[2].equals("-c")) {
            if(!(args.length == 4)) {
                Msg.E("No file specified.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            plp.create(args[3]);
            plp.plpfile = new File(args[1]);
            plp.save();
        }
        else if(args[2].equals("-importdir") || args[2].equals("-d")) {
            if(!(args.length == 4)) {
                Msg.E("No file specified.", Constants.PLP_GENERIC_ERROR, null);
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
                Msg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            int main_index = Integer.parseInt(args[3]);
            if(main_index <= 0 || main_index >= plp.asms.size())
                return;
            plp.setMainAsm(main_index);
            plp.save();
        }
        else if(args[2].equals("-v")) {
            if(!(args.length == 4 || args.length == 5)) {
                Msg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            int index = Integer.parseInt(args[3]);
            if(index < 0 || index >= plp.asms.size())
                return;
            String asmStr = plp.getAsm(index).getAsmString();
            if(args.length == 4)
                Msg.M(asmStr);
            else  {
                String[] splitStr = asmStr.split("\\r?\\n");
                int lineNum = Integer.parseInt(args[4]);
                Msg.M(lineNum + "\t: " + splitStr[lineNum - 1]);
            }
        }

        else if(args[2].equals("-m")) {
            Msg.I("Metafile contents:", null);
            Msg.M(plp.meta);
	}
        else if((args[2].equals("-r"))) {
            if(!(args.length == 4)) {
                Msg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            int index = Integer.parseInt(args[3]);
            plp.removeAsm(index);
            plp.save();
        }
        else if((args[2].equals("-e"))) {
            if(!(args.length == 5)) {
                Msg.E("Missing argument(s).", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            int index = Integer.parseInt(args[3]);
            plp.exportAsm(index, args[4]);
        }
        else if((args[2].equals("-edit"))) {
            if(!(args.length == 4)) {
                Msg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
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
            String timestamp = (new java.util.Date()).toString();
            if(plp.asm != null && plp.asm.isAssembled() && plp.getArch().equals("plpmips")) {
                plptool.mips.Formatter.symTablePrettyPrint(plp.asm.getSymTable());
                Msg.M("");
                plptool.mips.Formatter.prettyPrint((plptool.mips.Asm) plp.asm);
                Msg.M("");
                Msg.M("Build timestamp: " + timestamp);
                Msg.M("Binary size: " + plp.asm.getObjectCode().length + " words");
                Msg.M("Starting address: " + String.format("0x%08x", plp.asm.getAddrTable()[0]));
            } else
                Msg.E("BUILD FAILED", Constants.PLP_GENERIC_ERROR, plp);
        }
        else if(args[2].equals("-p")) {
            if(!(args.length == 4)) {
                Msg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
                return;
            }

            if(plp.asm != null && plp.asm.isAssembled())
                plp.program(args[3]);
        }

        else {
            Msg.I("Invalid option: " + args[2], null);
            return;
        }         
    }

    public static void helpMessage() {
        System.out.println("  PLP project file manipulator commands, to be run with -plp <plpfile> [options]");
        System.out.println();
        System.out.println("  -c <asm>");
        System.out.println("       Create a PLP project <plpfile> and import <asm> into the project.");
        System.out.println();
        System.out.println("  -p <port>");
        System.out.println("       Program PLP target board with <plpfile> using serial port <port>.");
        System.out.println();
        System.out.println("  -a");
        System.out.println("       Assemble <plpfile>.");
        System.out.println();
        System.out.println("  -i <asm 1> <asm 2> ...");
        System.out.println("       Import <asm-x> into <plpfile>.");
        System.out.println();
        System.out.println("  -d <directory>");
        System.out.println("       Import all files within <directory> into <plpfile>.");
        System.out.println();
        System.out.println("  -e <index> <file>");
        System.out.println("       Export source file with <index> as <file>.");
        System.out.println();
        System.out.println("  -r <index>");
        System.out.println("       Remove source file with <index> from <plpfile>.");
        System.out.println();
        System.out.println("  -s <index>");
        System.out.println("       Set source file with <index> as the main program.");
        System.out.println();
    }
}
