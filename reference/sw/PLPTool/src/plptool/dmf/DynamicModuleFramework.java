/*
    Copyright 2011-2013 David Fritz, Brian Gordon, Wira Mulia

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

package plptool.dmf;

import java.io.*;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.jar.*;
import javax.swing.JOptionPane;
import java.awt.image.BufferedImage;
import javax.imageio.ImageIO;
import plptool.*;

/**
 * This class is the dynamic module loading framework for PLPTool. The
 * class loader and the class load routine are generic, they can be used
 * to load generic classes and attach them to the list to be used in plptool
 * later for any reason. Object instantiations of dynamic classes are
 * superclass specific. Instantiation routines are expected to return a null
 * reference if they failed and call error #1040 (e.g. typecasting error).
 *
 * @author wira
 */
public class DynamicModuleFramework {
    /**
     * Index tracker for loaded module classes
     */
    private static int index = 0;

    /**
     * A list of references to dynamic module classes
     */
    private static ArrayList<Class> dynamicModuleClasses = new ArrayList<Class>();

    /**
     * Dynamic PLPTool >= 5.x module instances holder
     */
    private static ArrayList<Object> dynamicInstances = new ArrayList<Object>();

    /**
     * Path to the file where the module class is (.class or .jar file)
     */
    private static ArrayList<String> dynamicModuleClassPath = new ArrayList<String>();

    /**
     * Boolean to give warning about loading 3rd party modules once
     */
    public static boolean warned = false;

    /**
     * Reference to the class loader
     */
    private static PLPDynamicModuleClassLoader loader;

    /**
     * Module class loading routine for PLPTool dynamic module framework. This
     * function is called by PLPToolApp class during program startup via
     * the --load-class parameter.
     *
     * @param path Path of the Java class file
     * @param className Java class name (package.subpackage...class)
     * @return True if class is loaded and attached to the list, false otherwise
     */
    public static boolean loadModuleClass(String path, String className) {
        if(!warned) {
            Msg.W("YOU ARE LOADING A DYNAMIC MODULE, THIS COULD POTENTIALLY BE DANGEROUS", null);
            Msg.W("Make sure you only use trusted third party modules!", null);
            warned = true;
        }
        Msg.D("[" + index + "] Loading module class " + className + " from " + path +
              " ... ", 2, null);
        if(loader == null) {
            ClassLoader parent = DynamicModuleFramework.class.getClassLoader();
            loader = new PLPDynamicModuleClassLoader(parent);
        }
        loader.setClassToLoad(path, className);
        try {
            Class dynamicModuleClass = loader.loadClass(className);
            if(dynamicModuleClass == null) {
                return false;
            }
            dynamicModuleClasses.add(dynamicModuleClass);
            dynamicModuleClassPath.add(path);
            index++;
        } catch(ClassNotFoundException e) {
            Msg.E("The class " + className + " is not found in " + path,
                  Constants.PLP_DMOD_CLASS_NOT_FOUND_ERROR, null);
            return false;
        } catch(Exception e) {
            Msg.E("Unable to load module. Set debug level to 2 or higher for" +
                  " stack trace.", Constants.PLP_DMOD_GENERAL_ERROR, null);
            if(Constants.debugLevel >= 2)
                e.printStackTrace();
            return false;
        }
        return true;
    }

    /**
     * Get a reference to a registered module class specified by its index
     *
     * @param index Index of the class
     * @return Reference to the module class
     */
    public static Class getDynamicModuleClass(int index) {
        if(index < 0 || index >= dynamicModuleClasses.size()) {
            Msg.E("Invalid index.", Constants.PLP_DMOD_INVALID_CLASS_INDEX, null);
            return null;
        }
        
        return dynamicModuleClasses.get(index);
    }

    /**
     * Get the number of classes that are loaded
     *
     * @return Number of classes loaded
     */
    public static int getNumberOfClasses() {
        return dynamicModuleClasses.size();
    }

    /**
     * Check whether the specified module class is registered and return the
     * index of the class in the list
     *
     * @param name Name of the class to look up
     * @return The index of the class if found, -1 otherwise
     */
    public static int isModuleClassRegistered(String name) {
        for(int i = 0; i < dynamicModuleClasses.size(); i++)
            if(dynamicModuleClasses.get(i).getName().equals(name))
                return i;

        return -1;
    }
      
    /**
     * Instantiate a dynamic module and add a reference
     * to it in the dynamicInstances list.
     *
     * @param index Index of the class
     * @return Index of the new module in the list, -1 if an error occurred
     */
    public static int newModuleInstance(int index) {
        try {
            Class moduleClass = getDynamicModuleClass(index);
            if(moduleClass == null)
                return Constants.PLP_GENERIC_ERROR;
            dynamicInstances.add(moduleClass.newInstance());
            return dynamicInstances.size() - 1;

        } catch (InstantiationException e) {
            Msg.E("Instantiation exception for module " + getDynamicModuleClass(index).getName() + ". " +
                  "Generic modules have to extend plptool.PLPGenericModule class.",
                  Constants.PLP_DMOD_INSTANTIATION_ERROR, null);
            return Constants.PLP_GENERIC_ERROR;
        } catch(IllegalAccessException e) {
            Msg.E("Illegal access exception for module " + getDynamicModuleClass(index).getName(),
                  Constants.PLP_DMOD_ILLEGAL_ACCESS, null);
            return Constants.PLP_GENERIC_ERROR;
        }
    }

    /**
     * Return the number of instantiated dynamic module objects
     *
     * @return Number of instantiated dynamic module objects
     */
    public static int getNumberOfModuleInstances() {
        return dynamicInstances.size();
    }

    /**
     * Return a reference to the specified dynamic module instance
     *
     * @param index Index of the module object to return
     * @return Reference to the specified object
     */
    public static Object getModuleInstance(int index) {
        if(index < 0 || index >= dynamicInstances.size()) {
            Msg.E("Invalid index.",
                  Constants.PLP_DMOD_INVALID_MODULE_INDEX, null);
            return null;
        }

        return dynamicInstances.get(index);
    }

    /**
     * Check whether an instance of the specified class is already loaded
     *
     * @param className Class name of the module to check
     * @return True if the framework determines that an instance is already
     * loaded, false otherwise
     */
    public static boolean isModuleInstanceLoaded(String className) {
        for(int i = 0; i < getNumberOfModuleInstances(); i++) {
            if(getModuleInstance(i).getClass().getCanonicalName().equals(className))
                return true;
        }

        return false;
    }

    /**
     * Remove the reference to the specified module instance
     *
     * @param index Index of the object reference in the list
     * @return The reference to object if found, null otherwise
     */
    public static Object removeModuleInstance(int index) {
        if(index < 0 || index >= dynamicInstances.size()) {
            Msg.E("Invalid index.",
                  Constants.PLP_DMOD_INVALID_MODULE_INDEX, null);
            return null;
        }

        return dynamicInstances.remove(index);
    }

    /**
     * Autoload modules from the specified directory and apply manifest files
     * to the provided ProjectDriver instance. If prompt is true, this method
     * will prompt the user for module autoload.
     *
     * @param autoloadPath
     * @param plp
     * @param prompt
     */
    public static void autoloadModules(String autoloadPath,
            plptool.gui.ProjectDriver plp, boolean prompt) {
        Msg.D("dmf: Loading modules from " + autoloadPath + "...", 1, null);
        File autoloadDir = new File(autoloadPath);
        if(autoloadDir.exists() && autoloadDir.isDirectory()) {
            File[] files = autoloadDir.listFiles();

            if(files.length > 0) {
                String message = "PLPTool module autoloading is enabled." +
                            " If you click yes, all saved modules will be loaded." +
                            " Continue with module autoload?\n" +
                            "You can disable this prompt from Tools->Options->Miscellaneous.";
                if(plp.g() && prompt) {
                    int ret = JOptionPane.showConfirmDialog(plp.g_dev,
                            message, "Autoload Modules", JOptionPane.YES_NO_OPTION);
                    if(ret == JOptionPane.CLOSED_OPTION ||
                       ret == JOptionPane.NO_OPTION)
                        return;
                } else if(prompt) {
                    System.out.print(message + " (Y/N) ");
                    try {
                        char response = PLPToolbox.readLine().charAt(0);
                        if(response != 'Y')
                            return;
                    } catch(Exception e) { }
                }
            }

            for(int i = 0; i < files.length; i++) {
                if(files[i].getName().endsWith(".jar")) {
                    String[] manifest = loadJarWithManifest(files[i].getAbsolutePath());
                    if(manifest != null && plp != null)
                        applyManifestEntries(files[i].getAbsolutePath(), manifest, plp);
                }
            }
        }
    }

    /**
     * Delete ~/.plp/autoload directory
     */
    public static void removeAutoloadModules() {
        Msg.I("Removing " + PLPToolbox.getConfDir() + "/autoload...", null);
        File autoloadDir = new File(PLPToolbox.getConfDir() + "/autoload");
        if(autoloadDir.exists() && autoloadDir.isDirectory()) {
            File[] files = autoloadDir.listFiles();
            for(int i = 0; i < files.length; i++)
                files[i].delete();
            autoloadDir.delete();

            if(autoloadDir.exists())
                Msg.E("Failed to remove autoload directory. " +
                      "Are the modules loaded? Try '--delete-autoload-dir' " +
                      "option from the command line.",
                      Constants.PLP_GENERIC_ERROR, null);
        }
    }

    /**
     * Create a new instance of a module class and cast it to PLPSimBusModule.
     *
     * @param index Index of the class
     * @return Reference to the newly instantiated object casted to
     * a simulation bus module. Returns null pointer if instantiation failed
     * for any reason.
     */
    public static PLPSimBusModule newBusModuleInstance(int index) {
        try {
            Class moduleClass = getDynamicModuleClass(index);
            return moduleClass == null ? null : (PLPSimBusModule) moduleClass.newInstance();

        } catch (InstantiationException e) {
            Msg.E("Instantiation exception for module " + getDynamicModuleClass(index).getName() + ". " +
                  "Dynamic bus modules have to extend plptool.PLPSimBusModule class.",
                  Constants.PLP_DMOD_INSTANTIATION_ERROR, null);
            return null;
        } catch(IllegalAccessException e) {
            Msg.E("Illegal access exception for module " + getDynamicModuleClass(index).getName(),
                  Constants.PLP_DMOD_ILLEGAL_ACCESS, null);
            return null;
        }
    }

    /**
     * Load all classes from a jar file
     *
     * @param path Path to the jar file
     */
    public static boolean loadAllFromJar(String path) {
        try {
            JarFile jar = new JarFile(path);
            Enumeration<JarEntry> entries =  jar.entries();
            JarEntry entry;
            String className;

            while(entries.hasMoreElements()) {
                entry = entries.nextElement();
                if(entry.getName().endsWith(".class")) {
                    className = entry.getName().replace("/", ".");
                    className = className.substring(0, className.length() - 6);
                    loadModuleClass(path, className);
                }
            }

            jar.close();

            return true;

        } catch(IOException e) {
            Msg.E("Failed to load classes from '" + path + "'",
                  Constants.PLP_DMOD_FAILED_TO_LOAD_ALL_JAR, null);
            return false;
        }
    }

    /**
     * Load classes from a JAR file that contains the plp.manifest class
     * listing file
     *
     * @param path Path to the JAR file
     * @return Manifest lines on successful load, null otherwise
     */
    public static String[] loadJarWithManifest(String path) {
        try {
            Msg.I("Loading " + path + "...", null);
            JarFile jar = new JarFile(path);
            JarEntry entry = jar.getJarEntry("plp.manifest");
            boolean manifestFound = false;

            if(entry != null) {
                manifestFound = true;

                byte[] array = new byte[1024];
                ByteArrayOutputStream out = new ByteArrayOutputStream(array.length);
                InputStream in = jar.getInputStream(entry);
                int length = in.read(array);
                while(length > 0) {
                    out.write(array, 0, length);
                    length = in.read(array);
                }
                String[] lines = (new String(out.toByteArray())).split("\\r?\\n");

                for(int i = 0; i < lines.length; i++) {
                    if(lines[i].startsWith("class::"))
                        loadModuleClass(path, lines[i].replace("class::", ""));
                }
                jar.close();
                return lines;
            }

            if(!manifestFound) {
                Msg.E("No plp.manifest file found in the JAR archive",
                        Constants.PLP_DMOD_FAILED_TO_LOAD_ALL_JAR, null);
                jar.close();
                return null;
            }

        } catch(IOException e) {
            Msg.E("Failed to load classes from '" + path + "'",
                  Constants.PLP_DMOD_FAILED_TO_LOAD_ALL_JAR, null);
            return null;
        }

        return null;
    }

    /**
     * Apply manifest entries to the project
     *
     * @param jar Path to jar file that contains the manifest
     * @param manifestLines Manifest lines returned by loadJarWithManifest
     * @param plp Reference to the ProjectDriver to apply the manifest to
     */
    public static void applyManifestEntries(String jar, String[] manifestLines,
            plptool.gui.ProjectDriver plp) {
        for(int i = 0; i < manifestLines.length; i++)
            ManifestHandlers.handleEntry(jar, plp, manifestLines[i]);
    }

    /**
     * Check if the specified jar file contains the plp.manifest file
     *
     * @param path Path to the JAR file
     * @return True if the file exists, false otherwise
     */
    public static boolean checkForManifest(String path) {
        JarFile jar;
        try {
            jar = new JarFile(path);       
            boolean ret = !(jar.getJarEntry("plp.manifest") == null);
            jar.close();
            return ret;
            
        } catch(Exception e) {
            return false;
        }
    }

    /**
     * Get a manifest entry from a module's JAR file
     *
     * @param path Path to the JAR file
     * @param key Manifest entry key
     * @return Manifest entry
     */
    public static String getManifestEntry(String path, String key) {
        String ret = null;

        try {
            Msg.D("Finding manifest entry with key '" + key + "' from '" + path
                  + "' ...", 3, null);
            JarFile jar = new JarFile(path);
            JarEntry entry;
            boolean manifestFound = false;
            entry = jar.getJarEntry("plp.manifest");
            if(entry == null)
                manifestFound = false;
            else {
                byte[] array = new byte[1024];
                ByteArrayOutputStream out = new ByteArrayOutputStream(array.length);
                InputStream in = jar.getInputStream(entry);
                int length = in.read(array);
                while(length > 0) {
                    out.write(array, 0, length);
                    length = in.read(array);
                }
                String[] lines = (new String(out.toByteArray())).split("\\r?\\n");

                for(int i = 0; i < lines.length; i++) {
                    if(lines[i].startsWith(key + "::")) {
                        String tokens[] = lines[i].split("::", 2);
                        jar.close();
                        if(tokens.length > 1)
                            return tokens[1];
                    }
                }
            }

            if(!manifestFound) {
                Msg.E("No plp.manifest file found in the JAR archive",
                        Constants.PLP_DMOD_NO_MANIFEST_FOUND, null);
                jar.close();
                return null;
            }

        } catch(IOException e) {
            Msg.E("Failed to load '" + path + "'",
                  Constants.PLP_DMOD_FAILED_TO_LOAD_ALL_JAR, null);
            return null;
        }

        return ret;
    }

    /**
     * Generate a plp.manifest file for a given Java package as specified
     * by the path. This method will recursively go through the directories,
     * resolve interface/superclass dependencies and generate the manifest file.
     *
     * @param path Path to Java package directory
     * @param connectorClass Class implementing PLPGenericModule that will be
     * loaded by PLPTool when the module is loaded
     * @return String of the plp.manifest file
     */
    public static String generateManifest(String path, String title,
            String author, String license, String description, String version) {
        String manifest = "";
        String t;
        ArrayList<File> classes = new ArrayList<File>();
        ArrayList<String> classNames = new ArrayList<String>();
        ArrayList<String> resources = new ArrayList<String>();
        File packageDir = new File(path);
        Msg.M("*******************************************************************************");
        Msg.M("Enumerating JAR entries...");
        if(packageDir.getName().endsWith(".jar")) {
            try {
                JarFile jar = new JarFile(path);
                Enumeration<JarEntry> entries =  jar.entries();
                JarEntry entry;
                String className;

                while(entries.hasMoreElements()) {
                    entry = entries.nextElement();
                    if(entry.getName().endsWith(".class")) {
                        className = entry.getName().replace("/", ".");
                        className = className.substring(0, className.length() - 6);
                        classNames.add(className);
                        classes.add(packageDir);
                        if(className.endsWith("Main"))
                            resources.add("main::" + className);
                    } else {
                        t = checkValidResource(entry.getName());
                        if(t != null)
                            resources.add(t + "::" + entry.getName());
                    }
                }

                jar.close();
            } catch(IOException e) {
                Msg.E("Manifest generation failed.", Constants.PLP_GENERIC_ERROR,
                    null);
                Msg.trace(e);
                return null;
            }
        } else if(!packageDir.exists()) {
            Msg.E("'" + path + "' does not exist.",
                    Constants.PLP_GENERIC_ERROR, null);
            return null;
        } else if(!packageDir.isDirectory()) {
            Msg.E("'" + path + "' is not a directory or a jar file.",
                    Constants.PLP_GENERIC_ERROR, null);
            return null;
        } else
            generateManifestTraverseDirectory(packageDir, packageDir,
                    classes, classNames, resources);

        // Write out header information
        manifest += "title::" + title + "\n";
        manifest += "author::" + author + "\n";
        manifest += "license::" + license + "\n";
        manifest += "description::" + description + "\n";
        manifest += "version::" + version + "\n";

        // Write out miscellaneous resources
        for(String s : resources) {
            Msg.M("- Adding resource: " + s);
            manifest += s + "\n";
        }
        Msg.M("...done.");
        Msg.M(resources.size() + " resource entries added.");

        // Resolve dependencies. We'll do it in the most naive way possible:
        // try to load all classes, if some of them failed due to interfaces
        // or superclasses not being loaded first, do a second pass,
        // and then keep doing passes with alternating order until we don't
        // have any errors! If we don't get to load all classes after N
        // alternative iterations (the number of classes found), then something
        // is wrong and we just give up.
        boolean done;
        boolean ascending = true;
        int i;
        int iteration = 0;
        int iterationLimit = classes.size();
        Msg.M(classNames.size() + " Java classes were enumerated.");
        Msg.m("Determining loading order (disregard class warnings and ");
        Msg.M("errors)...");
        Msg.M("-------------------------------------------------------------------------------");
        do {
            done = true;
            if(ascending) {
                ascending = false;
                for(i = 0; i < classes.size(); i++) {
                    if (!(isModuleClassRegistered(classNames.get(i)) != -1) &&
                            loadModuleClass(classes.get(i).getAbsolutePath(), classNames.get(i))) {
                        manifest += "class::" + classNames.get(i) + "\n";
                        classes.remove(i);
                        classNames.remove(i);
                        i--;
                    } else
                        done = false;
                }
            } else {
                ascending = true;
                for(i = classes.size()-1; i >= 0; i--) {
                    if(!(isModuleClassRegistered(classNames.get(i)) != -1) &&
                            loadModuleClass(classes.get(i).getAbsolutePath(), classNames.get(i))) {
                        manifest += "class::" + classNames.get(i) + "\n";
                        classes.remove(i);
                        classNames.remove(i);
                    } else
                        done = false;
                }
            }
            iteration++;
        } while(!done && iteration < iterationLimit);

        if(!done) {
            Msg.M("-------------------------------------------------------------------------------");
            Msg.E("Manifest generation failed.", Constants.PLP_GENERIC_ERROR,
                    null);
            return null;
        }

        // Find classes that implement the ModuleInterface5 interface.
        // This class will be loaded when the module is loaded in PLPTool
        boolean found = false;
        for(i = 0; i < dynamicModuleClasses.size(); i++) {
            Class[] interfaces = dynamicModuleClasses.get(i).getInterfaces();
            for(int j = 0; j < interfaces.length; j++) {
                if(interfaces[j].getCanonicalName().equals("plptool.dmf.ModuleInterface5")) {
                    Msg.M("PLPTool 5 module interface found!");
                    manifest += "loadmodule5::" +
                            dynamicModuleClasses.get(i).getCanonicalName() + "\n";
                    found = true;
                }
            }

            if(dynamicModuleClasses.get(i).getSuperclass().getCanonicalName().equals("plptool.PLPArchitecture")) {
                Msg.M("PLP ISA implementation found!");
                manifest += "registerisa::" +
                        dynamicModuleClasses.get(i).getCanonicalName() + "::";
                Msg.m("Please enter ISA string ID: ");
                manifest += PLPToolbox.readLine() + "::";
                Msg.m("Please enter ISA numerical ID: ");
                manifest += PLPToolbox.readLine() + "::";
                Msg.m("Please enter ISA description: ");
                manifest += PLPToolbox.readLine() + "\n";
            }
        }
        
        Msg.M("-------------------------------------------------------------------------------");
        if(!found)
            Msg.W("No connector module for this package was found.\n", null);
        Msg.M("Manifest generation completed! You can include the plp.manifest file in the ");
        Msg.M("package jar file to allow PLPTool to load the module into a PLPTool session. If");
        Msg.M("no class implementing the connector interface was found, PLPTool will load the");
        Msg.M("classes, but the module functionality will only be able to be accessed through");
        Msg.M("the development module manager.");

        return manifest;
    }

    /**
     * Helper recursive function for generateManifest. This function populates
     * the classes list from the specified directory
     *
     * @param path Path to recurse into
     * @param classes Array list of classes
     */
    private static void generateManifestTraverseDirectory(File path,
            File rootPath, ArrayList<File> classes, ArrayList<String> classNames,
            ArrayList<String> resources) {
        String className;

        File[] list = path.listFiles();
        for(int i = 0; i < list.length; i++) {
            if(list[i].isDirectory())
                generateManifestTraverseDirectory(list[i], rootPath, classes, classNames, resources);
            else if(list[i].isFile() && list[i].getName().endsWith(".class")) {
                Msg.D("- Enumerating " + list[i].getAbsolutePath(), 3, null);
                classes.add(list[i]);
                className = list[i].getAbsolutePath().substring(rootPath.getAbsolutePath().length()+1);
                className = className.replaceAll("/|\\\\", ".");
                className = className.substring(0, className.length() - 6);
                classNames.add(className);
            } else
                resources.add(list[i].getName());
        }
    }

    /**
     * Check what kind of files are to be unpacked as resources for the module
     *
     * @param extension File extension (with period, e.g. ".png")
     * @return Manifest entry for the filetype, null if it is not recognizable
     */
    private static String checkValidResource(String file) {
        String ret = null;

        if(file.endsWith(".png"))       return "image";
        else if(file.endsWith(".jpg"))  return "image";
        else if(file.endsWith(".jpeg")) return "image";        

        return ret;
    }

    /**
     * Turn off first-time module warning
     */
    public static void disableWarning() {
        warned = true;
    }
}

/**
 * This class contains the handlers for manifest entries that the module
 * framework supports
 *
 * @author wira
 */
class ManifestHandlers {
    private static String entry;
    private static String jar;
    private static plptool.gui.ProjectDriver plp;

    public static void handleEntry(String j, plptool.gui.ProjectDriver p, String e) {
        entry = e;
        plp = p;
        jar = j;

        // Load PLPTool 5.x module interface class
        if(e.startsWith("loadmodule5::"))
            m_loadmodule5();

        // Register an ISA metaclass
        else if(e.startsWith("registerisa::"))
            m_registerisa();

        // Extract a file from a JAR file to temporary directory
        else if(e.startsWith("extracttotemp::"))
            m_extracttotemp();

        // Unpack module resource to temporary directory
        else if(e.startsWith("unpack::"))
            m_unpack();

        // Add an image to PLPToolApp image map
        else if(e.startsWith("image::"))
            m_image();
    }
    
    public static void m_loadmodule5() {
        int retLoad;
        String className = entry.replace("loadmodule5::", "");
        int cIndex = -1;
        int ret = DynamicModuleFramework.isModuleClassRegistered(className);
        if(ret > -1)
            cIndex = DynamicModuleFramework.newModuleInstance(ret);
        if(cIndex > -1) {
            Msg.D("Applying manifest entry: " + entry, 2, null);
            ModuleInterface5 mod = (ModuleInterface5)DynamicModuleFramework.getModuleInstance(cIndex);
            int[] minVersion = mod.getMinimumPLPToolVersion();
            if(Text.version[0] < minVersion[0] ||
                    ((Text.version[0] == minVersion[0]) && Text.version[1] < minVersion[1])) {
                Msg.E("This module requires a newer version of PLPTool",
                        Constants.PLP_DMOD_INSTANTIATION_ERROR, mod.getName());
                DynamicModuleFramework.removeModuleInstance(cIndex);
            } else {
                try {
                    retLoad = mod.initialize(plp);
                } catch(Exception e) {
                    retLoad = Msg.E("Module did not initialize cleanly."
                            + " Set debug level to at least 2 for stack trace.",
                            Constants.PLP_DMOD_INSTANTIATION_ERROR, mod);
                    if(Constants.debugLevel >= 2)
                        e.printStackTrace();
                } catch(java.lang.NoClassDefFoundError e) {
                    retLoad = Msg.E("Class dependency error occured during init."
                            + " Requested class not found: " + e.getMessage()
                            + " - Is the module packaged correctly?",
                            Constants.PLP_DMOD_INSTANTIATION_ERROR, mod);
                }
                if(retLoad != Constants.PLP_OK) {
                    if(plp.g())
                        PLPToolbox.showErrorDialog(plp.g_dev, "Module initialization failed."
                                + " PLPTool may need to be restarted.");
                    Msg.E("Module initialization failed. PLPTool may need to be restarted.",
                            Constants.PLP_DMOD_INSTANTIATION_ERROR, null);
                    DynamicModuleFramework.removeModuleInstance(cIndex);
                }
            }
        }
    }

    public static void m_registerisa() {
        String tokens[] = entry.split("::");
        if(tokens.length != 5) {
            Msg.E("Invalid 'registerisa' manifest entry usage",
                    Constants.PLP_DMOD_INVALID_MANIFEST_ENTRY, null);
            return;
        }
        int ret = DynamicModuleFramework.isModuleClassRegistered(tokens[1]);
        if(ret > -1) {
            Msg.D("Registering ISA: " + tokens[2] + " ID: " + tokens[3], 2, null);
            ArchRegistry.registerArchitecture(
                    DynamicModuleFramework.getDynamicModuleClass(ret),
                    Integer.parseInt(tokens[3]), tokens[2], tokens[4]);
        }
    }

    public static void m_extracttotemp() {
        String tokens[] = entry.split("::");
        if(tokens.length != 3)
            return;
        Msg.D("Applying manifest entry: " + entry, 2, null);
        PLPToolbox.checkCreateTempDirectory();
        PLPToolbox.copyFromJar(jar, tokens[1],
                PLPToolbox.getTmpDir() + "/" + tokens[2]);
    }

    public static void m_unpack() {
        String tokens[] = entry.split("::");
        if(tokens.length != 2)
            return;
        Msg.D("Applying manifest entry: " + entry, 2, null);
        PLPToolbox.checkCreateTempDirectory();
        File f = new File(PLPToolbox.getTmpDir() + "/" + tokens[1].replace('/', '.'));
        if(f.exists())
            f.delete();
        PLPToolbox.copyFromJar(jar, tokens[1], f.getAbsolutePath());
    }

    public static void m_image() {
        String tokens[] = entry.split("::");
        if(tokens.length != 2)
            return;
        Msg.D("Applying manifest entry: " + entry, 2, null);
        PLPToolbox.checkCreateTempDirectory();
        File f = new File(PLPToolbox.getTmpDir() + "/" + tokens[1].replace('/', '.'));
        if(f.exists())
            f.delete();
        PLPToolbox.copyFromJar(jar, tokens[1], f.getAbsolutePath());
        try {
            BufferedImage img = ImageIO.read(f.getAbsoluteFile());
            plptool.gui.PLPToolApp.putImage(f.getName(), img);
        } catch(IOException e) {
            Msg.E("Failed to load image from archive: " + tokens[1],
                    Constants.PLP_GENERAL_IO_ERROR, null);
        }
    }
}

/**
 * PLP dynamic module class loader for the dynamic module framework.
 *
 * @author wira
 */
class PLPDynamicModuleClassLoader extends ClassLoader {

    private String path;
    private String nameToCheck;

    /**
     * Constructor.
     *
     * @param parent Parent classloader, preferably top level
     */
    public PLPDynamicModuleClassLoader(ClassLoader parent) {
        super(parent);
    }

    /**
     * Set the path of the class file and the name of the class to load when
     * loadClass routine is called
     *
     * @param path Path to the class file
     * @param name Name of the class
     */
    public void setClassToLoad(String path, String name) {
        this.path = path;
        this.nameToCheck = name;
    }

    /**
     * Load the class specified by 'name'
     *
     * @param name Name of the class to load
     * @return Reference to the Class object
     * @throws ClassNotFoundException
     */
    @Override
    public Class loadClass(String name) throws ClassNotFoundException, NoClassDefFoundError {
        if(findLoadedClass(name) != null) {
            Msg.E("Class " + name + " is already loaded.",
                  Constants.PLP_DMOD_MODULE_IS_ALREADY_LOADED, null);
            return null;
        }

        if(!name.equals(nameToCheck))
            return super.loadClass(name, true);

        try {
            File file = new File(path);
            byte[] array = new byte[1024];
            ByteArrayOutputStream out = new ByteArrayOutputStream(array.length);
            String fName = file.getName();
            Class ret = null;
            if(fName.endsWith(".class")) {
                FileInputStream in = new FileInputStream(file);
                int length = in.read(array);
                while(length >= 0) {
                    out.write(array, 0, length);
                    length = in.read(array);
                }
                out.close();
                in.close();

                try {
                    ret = defineClass(name, out.toByteArray(), 0, out.size());
                } catch(NoClassDefFoundError e) {
                    Msg.E("defineClass: " + name + " FAILED.", Constants.PLP_GENERIC_ERROR, null);
                    return null;
                }
                
            // http://weblogs.java.net/blog/malenkov/archive/2008/07/how_to_load_cla.html
            // retrieved 2011-12-09 10:32AM CDT
            } else if(fName.endsWith(".jar")) {
                JarFile jar = new JarFile(file);
                JarEntry jarEntry = jar.getJarEntry(name.replace(".", "/") + ".class");
                if(jarEntry == null) {
                    jar.close();
                    throw new ClassNotFoundException(name);
                }

                InputStream in = jar.getInputStream(jarEntry);
                int length = in.read(array);
                while(length >= 0) {
                    out.write(array, 0, length);
                    length = in.read(array);
                }
                in.close();
                jar.close();
                try {
                    ret = defineClass(name, out.toByteArray(), 0, out.size());
                } catch(NoClassDefFoundError e) {
                    Msg.E("defineClass: " + name + " FAILED: " + e, Constants.PLP_GENERIC_ERROR, null);
                    jar.close();
                    return null;
                }
            }
            return ret;
            
        } catch(Exception e) {
            Msg.E("Unable to load dynamic module " + name + " from the file " + path,
                  Constants.PLP_DMOD_PATH_ERROR, null);
            if(Constants.debugLevel >= 2)
                e.printStackTrace();
            return null;
        }
    }
}
