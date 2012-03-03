/*
    Copyright 2010-2012 David Fritz, Brian Gordon, Wira Mulia

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
import plptool.Text;
import plptool.DynamicModuleFramework;

import java.io.FileInputStream;
import java.io.File;
import java.util.Scanner;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * The main class of the application.
 */
public class PLPToolApp extends SingleFrameApplication {
    
    private static String plpFilePath = null;
    private static boolean serialTerminal = false;
    private static boolean autoloadjars = true;
    private static ArrayList<String[]> manifests;
    private static HashMap<String, String> attributes;
    private static int startingArchID = ArchRegistry.ISA_PLPMIPS;
    public static ConsoleFrame con;

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
            // Launch the ProjectDriver
            ProjectDriver.loadConfig();
            ProjectDriver plp = new ProjectDriver(Constants.PLP_GUI_START_IDE);
            if(Constants.debugLevel > 0) {
                con = new ConsoleFrame(plp);
                con.setVisible(true);
            }
            plp.app = this;
            loadDynamicModules(plp);

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
        if(args.length >= 1 && args[0].equals("--gpl")) {
            Msg.M("\n" + Text.GPL + "\n");
            return;
        }

        // Print third party licensing information and quit
        if(args.length >= 1 && args[0].equals("--license")) {
            Msg.M("\n" + Text.copyrightString + "\n");
            Msg.M(Text.thirdPartyCopyrightString + "\n");
            return;
        }

        // Print buildinfo and quit
        if(args.length >= 1 && args[0].equals("--buildinfo")) {
            Msg.M(plptool.Version.stamp);
            return;
        }

        // Download a JAR file for autoloading
        if(args.length == 2 && args[0].equals("--save-module")) {
            plptool.PLPToolbox.downloadJARForAutoload(args[1], null, false);
            return;
        }

        // Delete ~/.plp/autoload
        if(args.length == 1 && args[0].equals("--delete-autoload-dir")) {
            DynamicModuleFramework.removeAutoloadModules();
            return;
        }

        int activeArgIndex = 0;
        java.io.File fileToOpen = null;

        // Save launching path
        Constants.launchPath = (new File(".")).getAbsolutePath();

/******************* COMMAND LINE ARGUMENTS THAT CAN BE COMBINED **************/

        for(int i = 0; i < args.length; i++) {
            Msg.D("args parsing: " + args[i], 4, null);

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

            // Disable dynamic module autoload from ~/.plp/autoload
            if(args.length >= activeArgIndex + 1 && args[i].equals("--no-module-autoload")) {
                autoloadjars = false;
                activeArgIndex++;
            }
            
            // Dynamic module load
            if(args.length >= activeArgIndex + 3 && args[i].equals("--load-class")) {
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

            // Load classes from a jar with a manifest file
            if(args.length >= activeArgIndex + 2 && args[i].equals("--load-jar-with-manifest")) {
		String[] manifest = DynamicModuleFramework.loadJarWithManifest(args[i+1]);
                if(manifest == null)
                    System.exit(-1);
                if(manifests == null)
                    manifests = new ArrayList<String[]>();
		manifests.add(manifest);
                activeArgIndex += 2;
            }

            // Load classes from a jar with a manifest file
            if(args.length >= activeArgIndex + 2 && args[i].equals("--isa-id")) {
		Integer archID = Integer.parseInt(args[i+1]);
                startingArchID = archID;
                activeArgIndex += 2;
            }

            // Pass an attribute pair to the ProjectDriver
            if(args.length >= activeArgIndex + 1 && args[i].startsWith("-P")) {
                if(attributes == null)
                    attributes = new HashMap<String, String>();
                String pair[] = (args[i].substring(2)).split("::", 2);
                if(pair.length == 2)
                    attributes.put(pair[0], pair[1]);
                activeArgIndex++;
            }
        }

        Msg.M("\n" + Text.copyrightString + "\n");

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
            ProjectDriver plp = new ProjectDriver(Constants.PLP_DEFAULT);
            loadDynamicModules(plp);
            if(args.length == 2) {
                if(!(plp.open(args[1], true) == Constants.PLP_OK)) return;
                if(plp.asm.isAssembled())
                    plp.simulate();
                plp.getArch().launchSimulatorCLI();
            } else if(args.length == 3) {
                try {
                    FileInputStream in = new FileInputStream(new File(args[2]));
                    Scanner sIn = new Scanner(in);
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
            ProjectFileManipulator.CLI(args, startingArchID);

        } else if(args.length == 1 && fileToOpen != null && fileToOpen.exists()) {
            plpFilePath = args[0];
            launch(PLPToolApp.class, args);

        } else if(args.length == 0) {
            launch(PLPToolApp.class, args);
            
        } else {
            if(!args[0].equals("--help") && !args[0].equals("--full-help") ) {
                Msg.E("Invalid argument: '" + args[0] + "'"
                        , Constants.PLP_TOOLAPP_ERROR, null);
                System.out.println();
            }
            printTerseHelpMessage();
            if(!args[0].equals("--full-help")) {
                System.out.println("  --full-help             Even more commands.");
                System.out.println();
            } else {
                System.out.println();
                ProjectFileManipulator.helpMessage();
                printFullHelpMessage();
            }
        }
    }

    /**
     * Print short help message to console
     */
    private static void printTerseHelpMessage() {
        System.out.println("Usage:");
        System.out.println("  java -jar PLPTool.jar [options] [plpfile]");
        System.out.println("                          Launch PLP Tool GUI. PLPTool will open [plpfile]");
        System.out.println("                            if it is provided. Run with '--full-help' for");
        System.out.println("                            options.");
        System.out.println();
        System.out.println("Non-GUI options:");
        System.out.println("  [options] -s <plpfile> [script]");
        System.out.println("                          Launch the command line simulator to simulate");
        System.out.println("                            <plpfile>. Providing a [script] will launch the");
        System.out.println("                            simulator in non-interactive mode.");
        System.out.println("  [options] -plp <plpfile> [command]");
        System.out.println("                          If no command is specified, print out the list of");
        System.out.println("                            source files contained in <plpfile>. This will also");
        System.out.println("                            create <plpfile> if it does not exist.");
        System.out.println("                            Use '-plp' by itself for command listing.");
    }

    /**
     * Print full help message to console
     */
    private static void printFullHelpMessage() {
        System.out.println("Other options:");
        System.out.println("  --buildinfo             Print build information and quit.");
        System.out.println("  --gpl                   Print GPL license text and quit.");
        System.out.println("  --license               Print third party licensing information and quit.");
        System.out.println("  --isa-id <arch id>      Force PLP Tool to use the ISA with <arch id> for");
        System.out.println("                            newly created projects.");
        System.out.println("  --remove-config         Remove saved configuration and reset all settings.");
        System.out.println("  --serialterminal        Launch serial terminal instead of the IDE.");
        System.out.println("  --suppress-output       Engage silent mode.");
        System.out.println("  --suppress-warning      Suppress all warning messages.");
        System.out.println("   -d <level>             Set debug level (0 to infinity).");
        System.out.println();
        System.out.println("Dynamic modules / extensions controls:");
        System.out.println("  --load-jar-with-manifest <jar file>");
        System.out.println("                          Load a PLPTool module JAR file, locate the manifest,");
        System.out.println("                            and interpret the file accordingly. PLPTool will");
        System.out.println("                            ONLY launch if the module is successfully loaded.");
        System.out.println("  --save-module <URL>     Fetch a module's jar file from URL, save it to the");
        System.out.println("                            the autoload directory, and quit. This module will");
        System.out.println("                            be autoloaded the next time PLPTool starts. Module");
        System.out.println("                            autoloading can be disabled by the user");
        System.out.println("                            (autoloading is enabled by default).");
        System.out.println("  --no-module-autoload    Do NOT autoload modules for this PLPTool session.");
        System.out.println("  --delete-autoload-dir   Delete the autoload cache directory and all of its");
        System.out.println("                            contents and quit.");
        System.out.println();
        System.out.println("MODULE DEBUGGING COMMANDS:");
        System.out.println("  --load-class <Java class file>");
        System.out.println("                          Load Java class file with the ClassLoader.");
        System.out.println("  --load-jar <jar file>   Load all Java classes inside the specified jar file.");
        System.out.println("  --P<key>::<value>       Pass a key-value property pair to the application.");
        System.out.println();
    }

    /**
     * Return execution-time attributes.
     *
     * @return HashMap of attribute pairs specified by the user during
     * the start of execution
     */
    public static HashMap<String, String> getAttributes() {
        if(attributes == null)
            attributes = new HashMap<String, String>();

        return attributes;
    }

    /**
     * Return the starting ISA (defaults to PLPMIPS, but can be overridden)
     *
     * @return The architecture ID of the ISA
     */
    public static int getStartingISA() {
        return startingArchID;
    }

    /**
     * Load dynamic modules and apply manifest entries
     *
     * @param plp Reference to the ProjectDriver
     */
    public static void loadDynamicModules(ProjectDriver plp) {
        // Load dynamic modules
        if(autoloadjars)
            DynamicModuleFramework.autoloadModules(plp);

        for(int i = 0; manifests != null && i < manifests.size(); i++)
            DynamicModuleFramework.applyManifestEntries(manifests.get(i), plp);
    }
}


