/*
    Copyright 2010-2014 David Fritz, Brian Gordon, Wira Mulia

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
import plptool.gui.frames.DynamicModuleManager;
import org.jdesktop.application.SingleFrameApplication;
import plptool.Msg;
import plptool.Constants;
import plptool.ArchRegistry;
import plptool.Text;
import plptool.dmf.DynamicModuleFramework;
import plptool.dmf.CallbackRegistry;
import plptool.testsuite.AutoTest;
import plptool.PLPToolbox;
import plptool.Config;

import java.io.FileInputStream;
import java.io.File;
import java.io.FileWriter;
import java.util.Scanner;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Properties;
import java.awt.image.BufferedImage;
import javax.imageio.ImageIO;

/**
 * This is the main class of PLPTool. This class handles command line arguments
 * and sets up the environment for PLPTool execution. This class is responsible
 * for setting up runtime environments (app properties), loading dynamic modules
 * specified in command line arguments, initiating module autoload procedure,
 * and determining next phase of execution (GUI or command line).
 */
public class PLPToolApp extends SingleFrameApplication {
    
    private static String plpFilePath = null;
    private static boolean newProject = false;

    private static boolean serialTerminal = false;
    private static boolean embedManifestGUI = false;
    private static boolean simulateCLI = false;
    private static boolean simulateScripted = false;
    private static boolean loadModules = true;
    private static boolean headless = false;
    private static boolean exiting = false;
    private static boolean preserveConfig = true;

    private static ArrayList<String> moduleLoadDirs;
    private static ArrayList<String> moduleLoadJars;
    private static HashMap<String, String> attributes;
    private static int startingArchID = ArchRegistry.ISA_PLPMIPS;
    private static String plpFileToSimulate;
    private static String scriptFileToRun;
    
    public static String[] commandLineArgs;
    public static ConsoleFrame con;
    private static HashMap<String, BufferedImage> images;

    @Override protected void startup() {

        if(java.awt.GraphicsEnvironment.isHeadless()) {
            Msg.E("Can not launch GUI in a headless environment!",
                  Constants.PLP_BACKEND_GUI_ON_HEADLESS_ENV, null);
            quit(Constants.PLP_BACKEND_GUI_ON_HEADLESS_ENV);
        }

        try {
            if(serialTerminal) {
                plptool.gui.SerialTerminal term = new plptool.gui.SerialTerminal(true);
                term.setVisible(true);
            } else if(embedManifestGUI) {
                DynamicModuleManager dmm = new DynamicModuleManager(null, true, null);
                dmm.setEmbedOnly();
                dmm.setVisible(true);
            } else {
                preserveConfig = false;
                images.put("__NOT_FOUND__", ImageIO.read(PLPToolApp.class.getResource("resources/invalid.png")));
                Msg.D("Creating temporary directory (" + PLPToolbox.getTmpDir() + ")...", 2, null);
                PLPToolbox.checkCreateTempDirectory();
                // Launch the ProjectDriver                
                loadConfig();
                ProjectDriver plp = new ProjectDriver(headless ? Constants.PLP_DEFAULT : Constants.PLP_GUI_START_IDE);
                CallbackRegistry.callback(CallbackRegistry.START, plp);
                if(Constants.debugLevel > 0) {
                    con = new ConsoleFrame(plp);
                    con.setVisible(true);
                }

                // Load modules from .plp/autoload, .plp/usermods, -D and -L, in
                // that order
                if(loadModules)
                    loadDynamicModules(plp, PLPToolbox.getConfDir() + "/autoload",
                                            PLPToolbox.getConfDir() + "/usermods");

                Msg.setOutput(plp.g_dev.getOutput());

                if(plpFilePath != null) {
                    if(newProject) {
                        plp.create(startingArchID);
                        plp.plpfile = new File(plpFilePath);
                        plp.refreshProjectView(false);
                    } else
                        plp.open(plpFilePath, true);
                }
            }
        } catch(Exception e) {
            System.err.println("=====================================");
            System.err.println("FATAL ERROR: Failed to initialize GUI");
            System.err.println("=====================================");
            Msg.trace(e);
            quit(-1);
        }
    }

    /**
     * Main method launching the application.
     */
    public static void main(String[] args) {        
        images = new HashMap<String, BufferedImage>();
        args = ArchRegistry.setup(args);                   // Setup ISA registry
        args = CallbackRegistry.setup(args);               // Setup callback registry
        args = AutoTest.setup(args);                       // Run unit tests
        commandLineArgs = args;

        Runtime.getRuntime().addShutdownHook(new Thread() {
        @Override
            public void run() {
                Msg.D("Shutdown hook", 2, null);
                if(!preserveConfig) {
                    saveConfig();
                }
                Msg.D("Exit callback", 2, null);
                CallbackRegistry.callback(CallbackRegistry.EXIT, null);
                try {
                    Msg.D("Removing temporary directory...", 2, null);
                    PLPToolbox.deleteRecursive(new File(PLPToolbox.getTmpDir()));
                } catch(java.io.FileNotFoundException e) {
		        	Msg.D("Failed to remove temporary directory: " + e, 2, null);
                }
            }
        });

/******************* PARSE COMMAND LINE ARGUMENTS *****************************/

        int activeArgIndex = 0;
        java.io.File fileToOpen = null;        

        // Copyright notice can only be suppressed with this as the first
        // command line argument
        if(args.length == 0
                || (args.length > 0 && !args[0].equals("--suppress-copyright-notice")))
            Msg.M("\n" + Text.copyrightString + "\n");

        // Save launching path
        Constants.launchPath = (new File(".")).getAbsolutePath();
        
        for(int i = 0; i < args.length; i++) {
            Msg.D("args[" + i + "] parsing: " + args[i], 4, null);

            if(i==0 && args[0].equals("--suppress-copyright-notice")) {
            
/******************************************************************************/
            
            // Silent mode
            } else if (args.length >= activeArgIndex + 1 && args[i].equals("--suppress-output")) {
                Msg.silent = true;
                activeArgIndex++;

            // Suppress warnings
            } else if (args.length >= activeArgIndex + 1 && args[i].equals("--suppress-warning")) {
                Msg.suppressWarnings = true;
                activeArgIndex++;

            // Debug level setting
            } else if (args.length >= activeArgIndex + 2 && args[i].equals("-d")) {
                Constants.debugLevel = Integer.parseInt(args[i + 1]);
                Msg.M("Debug level set to " + Constants.debugLevel);
                activeArgIndex += 2;
                i++;

            // Remove config file / reset config
            } else if (args.length >= activeArgIndex + 1 && args[i].equals("--remove-config")) {
                removeConfig();
                activeArgIndex++;

             // Disable dynamic module autoload from ~/.plp/autoload
            } else if (args.length >= activeArgIndex + 1 && args[i].equals("-N")) {
                loadModules = false;
                activeArgIndex++;

            // Disable the warning about loading modules
            } else if (args.length >= activeArgIndex + 1 && args[i].equals("-W")) {
                DynamicModuleFramework.warned = true;
                activeArgIndex++;

            // Delete ~/.plp/autoload
            } else if (args.length >= activeArgIndex + 1 && args[i].equals("-R")) {
                DynamicModuleFramework.removeAutoloadModules();
                activeArgIndex++;
            
            // Dynamic module load
            } else if (args.length >= activeArgIndex + 3 && args[i].equals("--load-class")) {
                if(!DynamicModuleFramework.loadModuleClass(args[i+2], args[i+1]))
                    quit(-1);
                activeArgIndex += 3;
                i+=2;

            // Load all classes from a jar
            } else if (args.length >= activeArgIndex + 2 && args[i].equals("--load-jar")) {
                if(!DynamicModuleFramework.loadAllFromJar(args[i+1]))
                    quit(-1);
                activeArgIndex += 2;
                i++;

            // Load classes from a jar with a manifest file
            } else if (args.length >= activeArgIndex + 2 && args[i].equals("-L")) {
                if(moduleLoadJars == null)
                    moduleLoadJars = new ArrayList<String>();
                moduleLoadJars.add(args[i+1]);
                activeArgIndex += 2;
                i++;

            // Load all classes from a jar within a the specified directory
            } else if (args.length >= activeArgIndex + 2 && args[i].equals("-D")) {
                if(moduleLoadDirs == null)
                    moduleLoadDirs = new ArrayList<String>();
                moduleLoadDirs.add(args[i+1]);
                activeArgIndex += 2;
                i++;

            // Override ISA for new projects
            } else if(args.length >= activeArgIndex + 2 && args[i].equals("--isa-id")) {
                Integer archID = Integer.parseInt(args[i+1]);
                startingArchID = archID;
                activeArgIndex += 2;
                i++;

            // Add an attribute pair
            } else if(args.length >= activeArgIndex + 1 && args[i].startsWith("-P")) {
                if(attributes == null)
                    attributes = new HashMap<String, String>();
                String pair[] = (args[i].substring(2)).split("::", 2);
                if(pair.length == 2)
                    attributes.put(pair[0], pair[1]);
                else if(pair.length == 1)
                    attributes.put(pair[0], "novalue");
                activeArgIndex++;

            // Launch serial terminal instead of the IDE
            } else if(args.length >= activeArgIndex + 1 && args[i].equals("--serialterminal")) {
                serialTerminal = true;
                activeArgIndex++;

            } else if(args.length >= activeArgIndex + 1 && args[i].equals("--headless")) {
                headless = true;
                activeArgIndex++;

/****************** EXCLUSIVE ARGUMENTS ***************************************/
/* These options will exit after it's executed (whichever comes first)        */
/******************************************************************************/

            // PLPTool will immediately quit when it encounters this option
            // can be used for debugging arguments
            } else if(args.length >= activeArgIndex + 1 && args[i].equals("-q")) {
                quit(Constants.PLP_OK);

            // Instantiate project driver and exit. Used by autotest
            } else if(args.length >= activeArgIndex + 1 && args[i].equals("--debug-projectdriver")) {
                Msg.M("Creating default ProjectDriver for debugging...");
                AutoTest.plp = new ProjectDriver(Constants.PLP_DEFAULT);
                CallbackRegistry.callback(CallbackRegistry.START, AutoTest.plp);
		return; // do not exit, leave it to the autotest thread

            // Print GPL license text and quit
            } else if(args.length >= activeArgIndex + 1 && args[i].equals("--gpl")) {
                Msg.M("\n" + Text.GPL + "\n");
                quit(Constants.PLP_OK);

            // Print third party licensing information and quit
            } else if (args.length >= activeArgIndex + 1 && args[i].equals("--about")) {
                Msg.M(Text.licenseBanner + "\n");
                Msg.M(Text.thirdPartyCopyrightString + "\n");
                Msg.M(Text.contactString + "\n");
                quit(Constants.PLP_OK);

            // Print buildinfo and quit
            } else if (args.length >= activeArgIndex + 1 && args[i].equals("--buildinfo")) {
                Msg.M(plptool.Version.stamp);
                Msg.M(getBuildInfo());
                quit(Constants.PLP_OK);

            // Download a JAR file for autoloading
            } else if (args.length >= activeArgIndex + 2 && args[i].equals("-S")) {
                plptool.PLPToolbox.downloadJARForAutoload(args[i+1], null, false);
                quit(Constants.PLP_OK);

            // Generate a plp.manifest file from a directory of Java classes
            } else if(args.length >= activeArgIndex + 3 && args[i].equals("--generate-manifest")) {
                Msg.M("Generating manifest for '" + args[i+1] + "'...");
                String manifest = DynamicModuleFramework.generateManifest(args[i+1],
                        "", "", "", "", "");
                if(manifest != null)
                    PLPToolbox.writeFile(manifest, args[i+2] + "/plp.manifest");
                quit(Constants.PLP_OK);

            // Generate and embed a plp.manifest file for a JAR file
            } else if(args.length >= activeArgIndex + 7 && args[i].equals("--embed-manifest")) {
                Msg.M("Generating manifest for '" + args[i+1] + "'...");
                String manifest = DynamicModuleFramework.generateManifest(args[i+1],
                        args[i+2], args[i+3], args[i+4], args[i+5], args[i+6]);
                if(manifest != null) {
                    if(PLPToolbox.addToJar(args[i+1], "plp.manifest", manifest.getBytes()) != Constants.PLP_OK) {
                        Msg.M("\nMANIFEST GENERATION FAILED");
                    } else {
                        Msg.M("\nplp.manifest has been embedded into the archive '" + args[i+1] + "'");
                    }
                } else
                    Msg.M("\nMANIFEST GENERATION FAILED, '" + args[i+1] + "' is unmodified.");
                quit(Constants.PLP_OK);

            // Bring up the embed manifest GUI
            } else if(args.length >= activeArgIndex + 1 && args[i].equals("--embed-manifest-gui")) {
                embedManifestGUI = true;

            // Pack a directory into a JAR file
            } else if(args.length >= activeArgIndex + 3 && args[i].equals("--pack")) {
                Msg.M("Packing '" + args[i+1] + "' to '" + args[i+2] + "'...");
                PLPToolbox.createJar(args[i+2], args[i+1]);
                quit(Constants.PLP_OK);

            // If we encounter '-plp', pass the rest of the arguments to the
            // project file manipulator
            } else if(args[i].equals("-plp")) {
                String[] newargs = new String[args.length - activeArgIndex];
                System.arraycopy(args, activeArgIndex, newargs, 0, newargs.length);
                ProjectFileManipulator.CLI(newargs, startingArchID, loadModules);
				return;

            // Interactive command-line simulator
            } else if (args.length >= activeArgIndex + 2 && args[i].equals("-s")) {
                simulateCLI = true;
                plpFileToSimulate = args[i+1];
                activeArgIndex+=2;
                i++;                

            // Non-interactive simulation
            } else if(args.length >= activeArgIndex + 3 && args[i].equals("-r")) {
                simulateScripted = true;
                plpFileToSimulate = args[i+1];
                scriptFileToRun = args[i+2];
                activeArgIndex+=3;
                i+=2;                

            } else if(args[i].equals("--help")) {
                printTerseHelpMessage();
                System.out.println("\nRun with '--full-help' for a complete listing of options.");
                quit(Constants.PLP_OK);

            } else if(args[i].equals("--module-debugging")) {
                printModuleDebuggingHelpMessage();
                System.out.println(Text.contactString);
                quit(Constants.PLP_OK);

            } else if(args[i].equals("--full-help")) {
                printTerseHelpMessage();
                System.out.println();
                ProjectFileManipulator.helpMessage();
                printFullHelpMessage();
                System.out.println(Text.contactString);
                quit(Constants.PLP_OK);

/****************** FILE TO OPEN / CREATE *************************************/

            // GNU style '--' to open a file that starts with a dash
            } else if(args.length >= activeArgIndex + 2 && args[i].equals("--")) {
                fileToOpen = new java.io.File(args[i+1]);
                newProject = !fileToOpen.exists();
                plpFilePath = args[i+1];
                activeArgIndex+=2;
                i++;

            // Invalid option catcher
            } else if(args[i].startsWith("-")) {
                Msg.E("Invalid argument: '" + args[i] + "'"
                        , Constants.PLP_TOOLAPP_ERROR, null);
                System.out.println("\nRun with '--full-help' for a complete listing of options.");
                quit(Constants.PLP_TOOLAPP_ERROR);

            } else {
                fileToOpen = new java.io.File(args[i]);
                newProject = !fileToOpen.exists();
                plpFilePath = args[i];
                activeArgIndex++;
            }
        }

        if(Constants.debugLevel >= 1)
            plptool.PLPToolbox.getOS(true);

        // Command line simulator handlers
        simulateCLI();

        // Headless execution handler
        if(headless)
            headless();
        else
            // here we go!
            launch(PLPToolApp.class, null);
    }

    /**
     * Print short help message to console
     */
    private static void printTerseHelpMessage() {
        System.out.println("Usage:");
        System.out.println("  java -jar PLPTool.jar [options] [plpfile]");
        System.out.println("                          Launch PLPTool GUI. PLPTool will open [plpfile]");
        System.out.println("                            if it is provided");
        System.out.println();
        System.out.println("Non-GUI options:");
        System.out.println("  -plp <plpfile> [command]");
        System.out.println("                          If no command is specified, print out the list of");
        System.out.println("                            source files contained in <plpfile>. This will also");
        System.out.println("                            create <plpfile> if it does not exist.");
        System.out.println("                            Use '-plp' by itself for command listing");
        System.out.println("  -s <plpfile>            Launch the command line simulator to simulate");
        System.out.println("                            <plpfile>");
        System.out.println("  -r <plpfile> <script>   Run the simulator in non-interactive mode");
        System.out.println(" --headless               Run in headless mode for non-GUI modules");
    }

    /**
     * Print full help message to console
     */
    private static void printFullHelpMessage() {
        System.out.println("Other options:");
        System.out.println("  --buildinfo             Print build information and quit");
        System.out.println("  --about                 Print licensing information and quit");
        System.out.println("  --gpl                   Print GPL license text and quit");        
        System.out.println("  --isa-id <arch id>      Force PLPTool to use the ISA with <arch id> for newly");
        System.out.println("                            created projects");
        System.out.println("  --remove-config         Remove saved configuration and reset all settings");
        System.out.println("  --serialterminal        Launch the serial terminal instead of the IDE");
        System.out.println("  --suppress-output       Engage silent mode");
        System.out.println("  --suppress-warning      Suppress all warning messages");
        System.out.println("   -d <level>             Set debug level (0 to infinity)");
        System.out.println("  --autotest-help         Unit test / automation framework information");
        System.out.println();
        System.out.println("Dynamic modules / extensions controls:");
        System.out.println("   -L <jar>               Load a PLPTool module JAR file, locate the manifest,");
        System.out.println("                            and interpret the file accordingly. PLPTool will");
        System.out.println("                            ONLY launch if the module is successfully loaded");
        System.out.println("   -D <path>              Attempt to load all modules in the directory ");
        System.out.println("                            specified by <path>");
        System.out.println("   -N                     Do not load any modules at startup");
        System.out.println("   -W                     Do not warn about the dangers of loading a module");
        System.out.println("   -S <URL>               Fetch a module's jar file from URL, save it to the");
        System.out.println("                            the autoload directory, and quit. This module will");
        System.out.println("                            be autoloaded the next time PLPTool starts. Module");
        System.out.println("                            autoloading can be disabled by the user");
        System.out.println("                            (autoloading is enabled by default)");
        System.out.println("   -R                     Delete the autoload cache directory and all of its ");
        System.out.println("                            contents");
        System.out.println("  --module-debugging      Print a list of module debugging options");
        System.out.println();       
    }

    private static void printModuleDebuggingHelpMessage() {
        System.out.println("Module debugging commands:");
        System.out.println("  --load-class <canonical class name> <Java class file>");
        System.out.println("                          Load Java class file with the ClassLoader");
        System.out.println("  --load-jar <jar>        Load all Java classes inside the specified jar file");
        System.out.println("  --generate-manifest <path or jar> <destination directory>");
        System.out.println("                          Generate plp.manifest of Java classes that are in");
        System.out.println("                            the specified path. This manifest file will be");
        System.out.println("                            written in the destination directory");
        System.out.println("  --embed-manifest <jar> <title> <author> <license> <description> <version>");
        System.out.println("                          Generate plp.manifest of Java classes that are in");
        System.out.println("                            the specified JAR file and embed the manifest in");
        System.out.println("                            the JAR file");
        System.out.println("  --embed-manifest-gui    Likewise, but with a GUI window");
        System.out.println("  --pack <directory> <jar>");
        System.out.println("                          Pack a directory into a JAR file");
        System.out.println("   -P<key>::<value>       Pass a key-value property pair to the application");
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
     * Load dynamic modules and apply manifest entries. Should be called just
     * ONCE in a PLPTool execution path.
     *
     * @param plp Reference to the ProjectDriver
     */
    public static void loadDynamicModules(ProjectDriver plp, String... autoloadPath) {
        for(int i = 0; i < autoloadPath.length; i++)
            DynamicModuleFramework.autoloadModules(autoloadPath[i], plp,
                    plptool.Config.cfgAskBeforeAutoloadingModules);

        if(moduleLoadDirs != null && moduleLoadDirs.size() > 0) {
            String dirPath;
            while(moduleLoadDirs.size() > 0) {
                dirPath = moduleLoadDirs.get(0);
                moduleLoadDirs.remove(0);
                Msg.D("Loading modules from " + dirPath + "...", 2, null);
                DynamicModuleFramework.autoloadModules(dirPath, plp, false);
            }
        }

        if(moduleLoadJars != null && moduleLoadJars.size() > 0) {
            String jarPath;
            while(moduleLoadJars.size() > 0) {
                jarPath = moduleLoadJars.get(0);
                moduleLoadJars.remove(0);
                Msg.D("Loading module from " + jarPath + "...", 2, null);
                String[] manifest = DynamicModuleFramework.loadJarWithManifest(jarPath);
                if(manifest == null)
                    quit(Constants.PLP_DMOD_GENERAL_ERROR);
                try {
                    DynamicModuleFramework.applyManifestEntries(jarPath, manifest, plp);
                } catch(Exception e) {
                    System.err.println("Module load routine failed for " + jarPath);
                    System.err.println(e.getMessage());
                    quit(Constants.PLP_DMOD_GENERAL_ERROR);
                } catch(java.lang.NoClassDefFoundError e) {
                    System.err.println("Module load routine failed for " + jarPath);
                    System.err.println("error: " + e.getMessage());
                    quit(Constants.PLP_DMOD_GENERAL_ERROR);
                }
            }
        }
    }

    /**
     * Handle headless execution. Note: modules have to be explicitly included
     * in the command line via -L or -D. Saved modules and usermods will NOT
     * be automatically loaded!
     */
    private static void headless() {
        if(!loadModules) {
            Msg.E("'-N' can not be used with '--headless'", Constants.PLP_TOOLAPP_ERROR, null);
            quit(Constants.PLP_TOOLAPP_ERROR);
        }
        loadConfig();
        ProjectDriver plp = new ProjectDriver(Constants.PLP_DEFAULT);
        loadDynamicModules(plp);
        if(CallbackRegistry.getCallbacks(CallbackRegistry.EVENT_HEADLESS_START).length > 0) {
            Msg.I("Running in headless mode.", null);
            CallbackRegistry.callback(CallbackRegistry.EVENT_HEADLESS_START, plp);
        } else if(headless) {
            Msg.I("No headless callbacks are registered, exiting.", null);
        }
    }

    /**
     * Handle CLI simulator options
     */
    private static void simulateCLI() {
        String ISA;
        if(attributes != null && (ISA = attributes.get("force_isa_id")) != null) {
            plptool.Config.cfgOverrideISA = Integer.parseInt(ISA);
        }

        if(simulateCLI) {
            ProjectDriver plp = new ProjectDriver(Constants.PLP_DEFAULT);
            CallbackRegistry.callback(CallbackRegistry.START, plp);
            // Autoload saved modules
            if(loadModules)
                loadDynamicModules(plp, PLPToolbox.getConfDir() + "/autoload",
                                        PLPToolbox.getConfDir() + "/usermods");
            if(!(plp.open(plpFileToSimulate, true) == Constants.PLP_OK))
                quit(Constants.PLP_GENERIC_ERROR);
            plp.simulate();
            plp.getArch().launchSimulatorCLI();
            quit(Constants.PLP_OK);

        } else if(simulateScripted) {
            ProjectDriver plp = new ProjectDriver(Constants.PLP_DEFAULT);
            CallbackRegistry.callback(CallbackRegistry.START, plp);
            // Autoload saved modules
            if(loadModules)
                loadDynamicModules(plp, PLPToolbox.getConfDir() + "/autoload",
                                        PLPToolbox.getConfDir() + "/usermods");
            try {
                FileInputStream in = new FileInputStream(new File(scriptFileToRun));
                Scanner sIn = new Scanner(in);
                if(!(plp.open(plpFileToSimulate, true) == Constants.PLP_OK))
                    quit(Constants.PLP_GENERIC_ERROR);
                plp.simulate();
                Msg.silent = false;
                while(sIn.hasNext())
                    plp.getArch().simCLICommand(sIn.nextLine());
            } catch(Exception e) {
                System.out.print("Unable to open/run the script '" + scriptFileToRun + "'. ");
                System.out.println("Set debug level to 2 or greater for stack trace.");
                Msg.trace(e);
                quit(Constants.PLP_GENERIC_ERROR);
            }
            quit(Constants.PLP_OK);
        }
    }

    /**
     * Get build information of this PLPTool build
     *
     * @return Build information in String
     */
    public static String getBuildInfo() {
        String ret = "";
        Properties prop = new Properties();
        try {
            prop.load(PLPToolApp.class.getResourceAsStream("resources/build.properties"));
        } catch(Exception e) {
            Msg.E("Unable to retrieve build information.",
                    Constants.PLP_GENERIC_ERROR, null);
            return ret;
        }
        ret += "This PLPTool package was built on " + prop.getProperty("buildstamp") + " by " + prop.getProperty("builder") + "\n";
        ret += "javac: " + prop.getProperty("javainfo") + "\n";
        ret += "ant: " + prop.getProperty("antinfo") +"\n";
        ret += "Built on: " + prop.getProperty("osinfo");
        return ret;
    }

    /**
     * Get an image from the image collection loaded in PLPToolApp
     *
     * @param key Image key (filename in JAR with slashes replaced by periods)
     * @return BufferedImage object of the image. A default image will be
     * returned if the key was not found
     */
    public static BufferedImage getImage(String key) {
        BufferedImage ret = images.get(key);
        if(ret == null) {
            Msg.W("PLPToolApp.getImage: '" + key + "' was not found.", null);
            return images.get("__NOT_FOUND__");
        }
        else
            return ret;
    }

    /**
     * Put an image to our image collection
     *
     * @param key String key to find the image
     * @param img The image as BufferedImage class
     */
    public static void putImage(String key, BufferedImage img) {
        images.put(key, img);
    }

    /**
     * Clean-up and quit
     */
    public static synchronized void quit(int status) {
		if(exiting)
			return;
		exiting = true;
        Msg.D("Exit(" + status + ")", 1, null);
        System.exit(status);
    }

/**
     * Attempt to load configuration from ~/.plp/config. Also checks whether
     * we have access to a user configuration directory (~/.plp)
     */
    public static void loadConfig() {
        File confDir = new File(PLPToolbox.getConfDir());
        if(!confDir.exists() && !confDir.mkdir()) {
            Msg.W("Unable to create configuration directory. User settings " +
                  "will not be saved and module functionality will fail!",
                  null);
        }

        if(confDir.exists() &&
                (!confDir.isDirectory() || !confDir.canWrite())) {
            Msg.W("Configuration directory is either a file or is not " +
                  "writable. User settings will not be saved and module " +
                  "functionality will fail!", null);
        }

        File config = new File(System.getProperty("user.home") + "/.plp/config");

        if(config.exists() && !config.isDirectory()) {
            Msg.D("Loading config from " + config.getAbsolutePath(), 2, null);
            try {
                FileInputStream in = new FileInputStream(config);

                String tokens[];
                byte[] str = new byte[(int) config.length()];
                in.read(str);
                in.close();
                String lines[] = new String(str).toString().split("\\r?\\n");

                for(int i = 0; i < lines.length; i++) {
                    tokens = lines[i].split("::", 2);
                    CallbackRegistry.callback(CallbackRegistry.LOAD_CONFIG_LINE, tokens);
                    if(tokens[0].equals("devFont")) {
                        Config.devFont = tokens[1];
                    } else if(tokens[0].equals("devFontSize")) {
                        Config.devFontSize = Integer.parseInt(tokens[1]);
                    } else if(tokens[0].equals("devSyntaxHighlighting")) {
                        Config.devSyntaxHighlighting = Boolean.parseBoolean(tokens[1]);
                    } else if(tokens[0].equals("simFunctional")) {
                        Config.simFunctional = Boolean.parseBoolean(tokens[1]);
                    } else if(tokens[0].equals("devWindowPositionX")) {
                        Config.devWindowPositionX = Integer.parseInt(tokens[1]);
                    } else if(tokens[0].equals("devWindowPositionY")) {
                        Config.devWindowPositionY = Integer.parseInt(tokens[1]);
                    } else if(tokens[0].equals("devWindowWidth")) {
                        Config.devWindowWidth = Integer.parseInt(tokens[1]);
                    } else if(tokens[0].equals("devWindowHeight")) {
                        Config.devWindowHeight = Integer.parseInt(tokens[1]);
                    } else if(tokens[0].equals("cfgAskBeforeAutoloadingModules")) {
                        Config.cfgAskBeforeAutoloadingModules = Boolean.parseBoolean(tokens[1]);
                    } else if(tokens[0].equals("cfgAskForISAForNewProjects")) {
                        Config.cfgAskForISAForNewProjects = Boolean.parseBoolean(tokens[1]);
                    } else if(tokens[0].equals("prgAutoDetectPorts")) {
                        Config.prgAutoDetectPorts = Boolean.parseBoolean(tokens[1]);
                    } else
                        PLPToolApp.getAttributes().put(tokens[0], tokens.length==2 ? tokens[1] : null);
                }

            } catch(Exception e) {
                Msg.E("Failed to load PLPTool configuration from disk.",
                      Constants.PLP_BACKEND_LOAD_CONFIG_FAILED, null);
            }
        }
    }

    /**
     * Save PLPTool configuration to ~/.plp/config
     */
    public static void saveConfig() {
        File configDir = new File(System.getProperty("user.home") + "/.plp");

        if(!configDir.exists()) {
            try {
                if(!configDir.mkdir()) {
                    Msg.W("Failed to save PLPTool configuration: " +
                          "Unable to create directory $[USER]/.plp", null);
                }
            } catch(Exception e) {
                Msg.E("Failed to save PLPTool configuration to disk.",
                      Constants.PLP_BACKEND_SAVE_CONFIG_FAILED, null);
                Msg.trace(e);
            }
        }

        File config = new File(System.getProperty("user.home") + "/.plp/config");

        if(config != null) {
            Msg.D("Saving config to " + config.getAbsolutePath(), 2, null);
            try {
                FileWriter out = new FileWriter(config);
                out.write("devFont::" + Config.devFont + "\n");
                out.write("devFontSize::" + Config.devFontSize + "\n");
                out.write("devSyntaxHighlighting::" + Config.devSyntaxHighlighting + "\n");
                out.write("simFunctional::" + Config.simFunctional + "\n");
                out.write("devWindowPositionX::" + Config.devWindowPositionX + "\n");
                out.write("devWindowPositionY::" + Config.devWindowPositionY + "\n");
                out.write("devWindowWidth::" + Config.devWindowWidth + "\n");
                out.write("devWindowHeight::" + Config.devWindowHeight + "\n");
                out.write("cfgAskBeforeAutoloadingModules::" + Config.cfgAskBeforeAutoloadingModules + "\n");
                out.write("cfgAskForISAForNewProjects::" + Config.cfgAskForISAForNewProjects + "\n");
                out.write("prgAutoDetectPorts::" + Config.prgAutoDetectPorts + "\n");
                for(int i = 0; i < 5; i++) {
                    String key = "develop_recent_" + i;
                    if(PLPToolApp.getAttributes().containsKey(key))
                        out.write(key + "::" + PLPToolApp.getAttributes().get(key) + "\n");
                }
                Msg.D("Calling SAVE_CONFIG callback", 4, null);
                // see if any modules want to save out their configuration
                // --- any configuration saved will be loaded by loadConfig to
                //     the application attributes, so no hook on loading is
                //     necessary. The converse is not true, the module will have
                //     to use this hook to keep its configuration
                CallbackRegistry.callback(CallbackRegistry.SAVE_CONFIG, out);
                Msg.D("Closing config file", 4, null);
                out.close();
                Msg.D("Save config done", 4, null);

            } catch(Exception e) {
                Msg.E("Failed to save PLPTool configuration to disk.",
                      Constants.PLP_BACKEND_SAVE_CONFIG_FAILED, null);
                Msg.trace(e);
            }
        }
    }

    /**
     * Delete ~/.plp/config
     */
    public static void removeConfig() {
        File config = new File(System.getProperty("user.home") + "/.plp/config");

        if(config.exists()) {
            Msg.M("Removing " + config.getAbsolutePath());
            config.delete();
        }
    }
}


