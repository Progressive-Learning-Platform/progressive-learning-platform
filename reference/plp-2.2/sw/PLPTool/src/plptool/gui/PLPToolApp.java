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

import org.jdesktop.application.Application;
import org.jdesktop.application.SingleFrameApplication;
import plptool.PLPMsg;
import plptool.Constants;

/**
 * The main class of the application.
 */
public class PLPToolApp extends SingleFrameApplication {
    
    SimShell simUI;
    static String plpFilePath = null;
    static boolean open = false;
    ConsoleFrame con;

    /**
     * At startup create and show the main frame of the application.
     */
    @Override protected void startup() {


        ProjectDriver plp = new ProjectDriver(true, "plpmips"); // default to plpmips for now
        if(Constants.debugLevel > 0) {
            con = new ConsoleFrame(plp);
            con.setVisible(true);
        }
        plp.app = this;
        
        PLPMsg.output = plp.g_dev.getOutput();
        if(plpFilePath != null)
            plp.open(plpFilePath);
        // show(mainWindow);
    }

    /**
     * This method is to initialize the specified window by injecting resources.
     * Windows shown in our application come fully initialized from the GUI
     * builder, so this additional configuration is not needed.
     */
    @Override protected void configureWindow(java.awt.Window root) {
    }

    /**
     * A convenient static getter for the application instance.
     * @return the instance of PLPToolApp
     */
    public static PLPToolApp getApplication() {
        return Application.getInstance(PLPToolApp.class);
    }

    /**
     * Main method launching the application.
     */
    public static void main(String[] args) {
        System.out.println("\n" + Constants.copyrightString);
        System.out.println();

        java.io.File fileToOpen = null;

        if(args.length >= 2 && args[0].equals("-d")) {
            Constants.debugLevel = Integer.parseInt(args[1]);
            System.out.println("Debug level set to " + Constants.debugLevel);
            if(args.length > 2) {
                String[] newargs = new String[args.length - 2];
                System.arraycopy(args, 2, newargs, 0, newargs.length);
                args = newargs;
            } else
                args = new String[0];
        }

        if(Constants.debugLevel >= 1)
            plptool.PLPToolbox.getOS(true);

        if(args.length == 1) {
            fileToOpen = new java.io.File(args[0]);
        }

        if(args.length > 0 && args[0].equals("-a")) {
            if(args.length != 3) {
                System.out.println("Usage: PLPTool -a <asm> <out>");
                System.exit(-1);
            } else {
                ProjectDriver plp = new ProjectDriver(false, "plpmips");
                if(!(plp.create(args[1]) == Constants.PLP_OK)) return;
                plp.plpfile = args[2];
                String timestamp = (new java.util.Date()).toString();
                plp.save();
                if(plp.asm.isAssembled()) {
                    plptool.mips.Formatter.symTablePrettyPrint(plp.asm.getSymTable());
                    PLPMsg.M("");
                    plptool.mips.Formatter.prettyPrint((plptool.mips.Asm) plp.asm);
                    PLPMsg.M("");
                    PLPMsg.M("Build timestamp: " + timestamp);
                    PLPMsg.M("Binary size: " + plp.asm.getObjectCode().length + " words");
                    PLPMsg.M("Starting address: " + String.format("0x%08x", plp.asm.getAddrTable()[0]));
                } else
                    PLPMsg.E("BUILD FAILED", Constants.PLP_GENERIC_ERROR, plp);
            }
        }
        else if(args.length > 0 && args[0].equals("-s")) {
            if(args.length != 2) {
                System.out.println("Usage: PLPTool -s <plpfile>");
            } else {
                ProjectDriver plp = new ProjectDriver(false, "plpmips");
                if(!(plp.open(args[1]) == Constants.PLP_OK)) return;
                plp.assemble();
                if(plp.asm.isAssembled())
                    plptool.mips.SimCLI.simCL(plp);
            }
        }
        else if(args.length > 0 && args[0].equals("-p")) {
            if(args.length != 3) {
                System.out.println("Usage: PLPTool -p <plpfile> <port>");
            } else {
                try {
                    ProjectDriver plp = new ProjectDriver(false, "plpmips");
                    if(!(plp.open(args[1]) == Constants.PLP_OK)) return;
                    plp.assemble();
                    plp.program(args[2]);
                } catch(Exception e) { }
            }
        }
        else  if(args.length > 0 && args[0].equals("-plp")) {
            ProjectFileManipulator.CLI(args);
        }
        else if(args.length == 1 && fileToOpen != null && fileToOpen.exists()) {
            open = true;
            plpFilePath = args[0];
            launch(PLPToolApp.class, args);
        }
        else if(args.length == 0) {
            launch(PLPToolApp.class, args);
        }
        else {
            PLPMsg.E("Invalid argument(s).", Constants.PLP_TOOLAPP_ERROR, null);
            System.out.println();
            System.out.println("Usage:\n");
            System.out.println("  java -jar PLPTool.jar");
            System.out.println("       Launch PLP Tool GUI");
            System.out.println();
            System.out.println("  java -jar PLPTool.jar <plpfile>");
            System.out.println("       Launch PLP Tool GUI and open <plpfile>");
            System.out.println();
            System.out.println("Non-GUI options:\n");
            System.out.println("  -a   <asm> <out>");
            System.out.println("       Assemble <asm> and write plp output to <out>.");
            System.out.println();
            System.out.println("  -s   <plpfile>");
            System.out.println("       Launches the command line simulator to simulate <plpfile>.");
            System.out.println();
            System.out.println("  -plp <plpfile>");
            System.out.println("       Prints out the list of source files contained in <plpfile>.");
            System.out.println("       Creates <plpfile> if it does not exist with main.asm as source file.");
            System.out.println();
            System.out.println("  -plp");
            System.out.println("       Prints out command-line plp file manipulator commands.");
            System.out.println();
        }
    }
}
