/*
    Copyright 2010-2011 David Fritz, Brian Gordon, Wira Mulia

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

import plptool.gui.frames.ConsoleFrame;
import org.jdesktop.application.Application;
import org.jdesktop.application.SingleFrameApplication;
import plptool.Msg;
import plptool.Constants;
import plptool.ArchRegistry;
import plptool.DynamicModuleFramework;

import java.io.FileInputStream;
import java.io.File;
import java.util.Scanner;

/**
 * The main class of the application.
 */
public class PLPToolApp extends SingleFrameApplication {
    
    static String plpFilePath = null;
    static boolean open = false;
    static boolean serialTerminal = false;
    ConsoleFrame con;

    /**
     * At startup create and show the main frame of the application.
     */
    @Override protected void startup() {

        if(java.awt.GraphicsEnvironment.isHeadless()) {
            Msg.E("Can not launch GUI in a headless environment!",
                  Constants.PLP_BACKEND_GUI_ON_HEADLESS_ENV, null);
            System.exit(-1);
        }

        if(serialTerminal) {
            plptool.gui.SerialTerminal term = new plptool.gui.SerialTerminal(true);
            term.setVisible(true);
            
        } else {
            ProjectDriver.loadConfig();
            ProjectDriver plp = new ProjectDriver(Constants.PLP_GUI_START_IDE, ArchRegistry.ISA_PLPMIPS); // default to plpmips for now
            if(Constants.debugLevel > 0) {
                con = new ConsoleFrame(plp);
                con.setVisible(true);
            }
            plp.app = this;

            Msg.setOutput(plp.g_dev.getOutput());
            if(plpFilePath != null)
                plp.open(plpFilePath, true);
        }

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

/******************** RUN AND QUIT COMMANDS ***********************************/

        // Print third party licensing information and quit
        if(args.length >= 1 && args[0].equals("--license")) {
            Msg.M("\n" + Constants.copyrightString + "\n");
            Msg.M(Constants.thirdPartyCopyrightString + "\n");
            return;
        }

        // Print buildinfo and quit
        if(args.length >= 1 && args[0].equals("--buildinfo")) {
            Msg.M(plptool.Version.stamp);
            return;
        }     

        int activeArgIndex = 0;
        java.io.File fileToOpen = null;

        // Save launching path
        Constants.launchPath = (new File(".")).getAbsolutePath();

/******************* COMMAND LINE ARGUMENTS THAT CAN BE COMBINED **************/

        for(int i = 0; i < args.length; i++) {
            // Silent mode
            if(args.length >= activeArgIndex + 1 && args[i].equals("--suppress-output")) {
                Msg.silent = true;
                activeArgIndex++;
            }

            // Suppress warnings
            if(args.length >= activeArgIndex + 1 && args[i].equals("--suppress-warning")) {
                Msg.suppressWarnings = true;
                activeArgIndex++;
            }

            // Debug level setting
            if(args.length >= activeArgIndex + 2 && args[i].equals("-d")) {
                Constants.debugLevel = Integer.parseInt(args[i + 1]);
                Msg.M("Debug level set to " + Constants.debugLevel);
                activeArgIndex += 2;
            }

            // Remove config file / reset config
            if(args.length >= activeArgIndex + 1 && args[i].equals("--remove-config")) {
                ProjectDriver.removeConfig();
                activeArgIndex++;
            }

            // Dynamic module load
            if(args.length >= activeArgIndex + 3 && args[i].equals("--load-class")) {
                File classFile = new File(args[i+1]);
                if(!DynamicModuleFramework.loadModuleClass(args[i+2], args[i+1]))
                    System.exit(-1);
                activeArgIndex += 3;
            }

            // Load all classes from a jar
            if(args.length >= activeArgIndex + 2 && args[i].equals("--load-jar")) {
                if(!DynamicModuleFramework.loadAllFromJar(args[i+1]))
                    System.exit(-1);
                activeArgIndex += 2;
            }
        }

        Msg.M("\n" + Constants.copyrightString + "\n");

        if(args.length > activeArgIndex) {
            String[] newargs = new String[args.length - activeArgIndex];
            System.arraycopy(args, activeArgIndex, newargs, 0, newargs.length);
            args = newargs;
        } else
            args = new String[0];

        if(Constants.debugLevel >= 1)
            plptool.PLPToolbox.getOS(true);

/****************** EXCLUSIVE ARGUMENTS ***************************************/

        if(args.length == 1) {
            fileToOpen = new java.io.File(args[0]);
        }

        if(args.length == 1 && args[0].equals("--serialterminal")) {
            serialTerminal = true;
            launch(PLPToolApp.class, args);

        } else if(args.length > 0 && args[0].equals("-s")) {
            if(args.length == 2) {
                ProjectDriver plp = new ProjectDriver(Constants.PLP_DEFAULT, ArchRegistry.ISA_PLPMIPS);
                if(!(plp.open(args[1], true) == Constants.PLP_OK)) return;
                if(plp.asm.isAssembled())
                    plp.simulate();
                plp.getArch().launchSimulatorCLI();
            } else if(args.length == 3) {
                try {
                    FileInputStream in = new FileInputStream(new File(args[2]));
                    Scanner sIn = new Scanner(in);
                    ProjectDriver plp = new ProjectDriver(Constants.PLP_DEFAULT, ArchRegistry.ISA_PLPMIPS);
                    if(!(plp.open(args[1], true) == Constants.PLP_OK)) return;
                    if(plp.asm.isAssembled())
                        plp.simulate();
                    Msg.silent = false;
                    while(sIn.hasNext())
                        plp.getArch().simCLICommand(sIn.nextLine());
                } catch(Exception e) {
                    System.out.println("Unable to open/run the script '" + args[2] + "'");
                }
            } else {
                System.out.println("Usage: PLPTool -s <plpfile> [script]");
            }

        } else  if(args.length > 0 && args[0].equals("-plp")) {
            ProjectFileManipulator.CLI(args);

        } else if(args.length == 1 && fileToOpen != null && fileToOpen.exists()) {
            open = true;
            plpFilePath = args[0];
            launch(PLPToolApp.class, args);

        } else if(args.length == 0) {
            launch(PLPToolApp.class, args);
            
        } else {
            if(!args[0].equals("--help")) {
                Msg.E("Invalid argument(s).", Constants.PLP_TOOLAPP_ERROR, null);
                System.out.println();
            }
            System.out.println("Usage:\n");
            System.out.println("  java -jar PLPTool.jar");
            System.out.println("       Launch PLP Tool GUI");
            System.out.println();
            System.out.println("  java -jar PLPTool.jar <plpfile>");
            System.out.println("       Launch PLP Tool GUI and open <plpfile>");
            System.out.println();
            System.out.println("Non-GUI options:\n");
            System.out.println("  -s   <plpfile> [script]");
            System.out.println("       Launch the command line simulator to simulate <plpfile>. Providing a [script] will ");
            System.out.println("       launch the simulator in non-interactive mode.");
            System.out.println();
            System.out.println("  -plp <plpfile> [command]");
            System.out.println("       If no command is specified, print out the list of source files contained in <plpfile>.");
            System.out.println("       This will also create <plpfile> if it does not exist.");
            System.out.println("       Use '-plp' by itself for command listing.");
            System.out.println();
        }
    }
}
