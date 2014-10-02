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
import plptool.PLPToolbox;

/**
 *
 * @author wira
 */
public class ProjectFileManipulator {
    public static int CLI(String[] args, int archID, boolean autoloadModules) {
        if(args == null || args.length < 2) {
            helpMessage();
            return Constants.PLP_OK;
        }

        int ret = Constants.PLP_OK;
        ProjectDriver plp = new ProjectDriver(Constants.PLP_DEFAULT);
        // Autoload saved modules
        if(autoloadModules)
            PLPToolApp.loadDynamicModules(plp, PLPToolbox.getConfDir() + "/autoload",
                                               PLPToolbox.getConfDir() + "/usermods");
        File plpHandler = new File(args[1]);
	
	if(plpHandler.exists() && !(args.length > 2 && args[2].equals("-c"))) {
            plp.open(args[1], false);

        } else if (args.length == 2) {
            plp.create(archID);
            plp.plpfile = new File(args[1]);
            if((ret = plp.save()) != Constants.PLP_OK)
                return ret;

        } else
            plp.plpfile = new File(args[1]);
        
        if(plp.plpfile == null || args.length <= 2)
            return Constants.PLP_OK;

        if(args[2].equals("-importasm") || args[2].equals("-i")) {
            if((args.length < 4)) {
                return Msg.E("No file specified.", Constants.PLP_GENERIC_ERROR, null);
            }

            String temp = plp.plpfile.getAbsolutePath();

            for(int i = 3; i < args.length; i++)
                if(!plpHandler.exists()) {
                    if(args[i].endsWith(".plp")) {
                        ProjectDriver tempPlp = new ProjectDriver(Constants.PLP_DEFAULT);
                        tempPlp.open(args[i], false);
                        
                    } else if(i == 3)
                        plp.create(args[i], archID);
                    else
                        plp.importAsm(args[i]);

                } else {
                    if(args[i].endsWith(".plp")) {
                        // TODO: Merge .plp files
                    } else {
                        plp.importAsm(args[i]);
                    }
                }

            plp.plpfile = new File(temp);
            plp.save();

        } else if(args[2].equals("-c")) {
            if(args.length < 4) {
                return Msg.E("No file specified.", Constants.PLP_GENERIC_ERROR, null);
            }
            plp.create(args[3], archID);

            for(int i = 4; i < args.length; i++)
                plp.importAsm(args[i]);

            plp.plpfile = new File(args[1]);
            plp.save();

        } else if(args[2].equals("-importdir") || args[2].equals("-d")) {
            if(!(args.length == 4)) {
                return Msg.E("No file specified.", Constants.PLP_GENERIC_ERROR, null);
            }

            File dir = new File(args[3]);

            String files[] = dir.list();

            for(int i = 0; i < files.length; i++)
                plp.importAsm(dir.getAbsolutePath() + "/" + files[i]);
            plp.save();

        } else if((args[2].equals("-setmain") || args[2].equals("-s"))) {
            if(!(args.length == 4)) {
                return Msg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
            }

            int main_index = Integer.parseInt(args[3]);
            if(main_index <= 0 || main_index >= plp.getAsms().size())
                return Constants.PLP_GENERIC_ERROR;
            plp.setMainAsm(main_index);
            plp.save();

        } else if(args[2].equals("-v")) {
            if(!(args.length == 4 || args.length == 5)) {
                return Msg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
            }

            int index = Integer.parseInt(args[3]);
            if(index < 0 || index >= plp.getAsms().size())
                return Constants.PLP_GENERIC_ERROR;
            String asmStr = plp.getAsm(index).getAsmString();
            if(args.length == 4)
                Msg.M(asmStr);
            else  {
                String[] splitStr = asmStr.split("\\r?\\n");
                int lineNum = Integer.parseInt(args[4]);
                if(lineNum-1 < splitStr.length && lineNum > 0)
                    Msg.M(lineNum + "\t: " + splitStr[lineNum - 1]);
            }

        } else if(args[2].equals("-meta")) {
            Msg.I("Metafile contents:", null);
            Msg.M(plp.meta);

	} else if((args[2].equals("-r"))) {
            if(!(args.length == 4)) {
                return Msg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
            }

            int index = Integer.parseInt(args[3]);
            plp.removeAsm(index);
            plp.save();

        } else if((args[2].equals("-e"))) {
            if(!(args.length == 5)) {
                return Msg.E("Missing argument(s).", Constants.PLP_GENERIC_ERROR, null);
            }

            int index = Integer.parseInt(args[3]);
            plp.exportAsm(index, args[4]);

        } else if((args[2].equals("-m"))) {
            if(!(args.length == 5)) {
                return Msg.E("Missing argument(s).", Constants.PLP_GENERIC_ERROR, null);
            }

            int index = Integer.parseInt(args[3]);
            int newIndex = Integer.parseInt(args[4]);
            plp.setAsmPosition(index, newIndex);
            plp.save();

        } else if((args[2].equals("-edit"))) {
            if(!(args.length == 4)) {
                return Msg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
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

        } else if(args[2].equals("-a")) {
            String timestamp = (new java.util.Date()).toString();
            plp.assemble();
            if(plp.asm != null && plp.asm.isAssembled() && plp.getArch().getStringID().equals("plpmips")) {
                plptool.mips.Formatter.symTablePrettyPrint(plp.asm.getSymTable());
                Msg.M("");
                plptool.mips.Formatter.prettyPrint((plptool.mips.Asm) plp.asm);
                Msg.M("");
                Msg.M("Build timestamp: " + timestamp);
                Msg.M("Binary size: " + plp.asm.getObjectCode().length + " words");
                Msg.M("Starting address: " + String.format("0x%08x", plp.asm.getAddrTable()[0]));
                plp.save();
            } else if(plp.asm != null && plp.asm.isAssembled()) {
                plp.save();
            } else
                Msg.E("BUILD FAILED", Constants.PLP_GENERIC_ERROR, plp);

        } else if(args[2].equals("-p")) {
            if(!(args.length == 4)) {
                return Msg.E("Missing argument.", Constants.PLP_GENERIC_ERROR, null);
            }

            if(!plp.isDirty()) {
                ret = plp.assemble();
                ret = plp.program(args[3]);
            } else
                return Msg.E("Binary files are not up to date!", Constants.PLP_GENERIC_ERROR, plp);

        } else {
            Msg.I("Invalid option: " + args[2], null);
            return Constants.PLP_GENERIC_ERROR;
        }

        return ret;
    }

    public static void helpMessage() {
        System.out.println("Project file manipulator commands, to be run with -plp <plpfile> [command]:");
        System.out.println("  -c <asm 1> <asm 2> ...  Create a new PLP project <plpfile> and import");
        System.out.println("                            <asm 1>, <asm 2>, ... into the project");
        System.out.println("  -p <port>               Program PLP target board with <plpfile> using serial");
        System.out.println("                            port <port>");
        System.out.println("  -a                      Assemble <plpfile>");
        System.out.println("  -i <asm 1> <asm 2> ...  Import <asm 1>, <asm 2>, ... into <plpfile>");
        System.out.println("  -d <directory>          Import all files within <directory> into <plpfile>");
        System.out.println("  -e <index> <file>       Export source file with <index> as <file>");
        System.out.println("  -r <index>              Remove source file with <index> from <plpfile>");
        System.out.println("  -s <index>              Set source file with <index> as the main program");
        System.out.println("  -m <index> <new index>  Move the source file with <index> to a new position");
        System.out.println("                            <new index> in the project");
        System.out.println();
    }
}
