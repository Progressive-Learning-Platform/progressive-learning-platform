/*
    Copyright 2011 David Fritz, Brian Gordon, Wira Mulia

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

package plptool;

import java.io.*;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.jar.*;

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
     * Dynamic module instances holder
     */
    private static ArrayList<PLPGenericModule> dynamicModuleInstances = new ArrayList<PLPGenericModule>();

    /**
     * A list that denotes whether the loaded class will be saved to user
     * directory or not
     */
    private static ArrayList<Boolean> savedModuleClass = new ArrayList<Boolean>();

    /**
     * Path to the file where the module class is (.class or .jar file)
     */
    private static ArrayList<String> dynamicModuleClassPath = new ArrayList<String>();

    /**
     * Boolean to give warning about loading 3rd party modules once
     */
    private static boolean warn = false;

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
        if(!warn) {
            Msg.W("YOU ARE LOADING A DYNAMIC MODULE, THIS COULD POTENTIALLY BE DANGEROUS", null);
            Msg.W("Make sure you only use trusted third party modules!", null);
            warn = true;
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
            savedModuleClass.add(false);
            index++;
        } catch(ClassNotFoundException e) {
            Msg.E("The class " + className + " is not found in " + path,
                  Constants.PLP_DBUSMOD_CLASS_NOT_FOUND_ERROR, null);
            return false;
        } catch(Exception e) {
            Msg.E("Unable to load module. Set debug level to 1 or higher for" +
                  " stack trace.", Constants.PLP_DBUSMOD_GENERIC, null);
            if(Constants.debugLevel >= 1)
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
            Msg.E("Invalid index.", Constants.PLP_DBUSMOD_GENERIC, null);
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
     * Instantiate a dynamic module as PLPGenericModule and add a reference
     * to it in the dynamicModuleInstances list.
     *
     * @param index Index of the class
     * @return Index of the new module in the list, -1 if an error occurred
     */
    public static int newGenericModuleInstance(int index) {
        try {
            Class moduleClass = getDynamicModuleClass(index);
            if(moduleClass == null)
                return Constants.PLP_GENERIC_ERROR;
            dynamicModuleInstances.add((PLPGenericModule) moduleClass.newInstance());
            return dynamicModuleInstances.size() - 1;

        } catch (InstantiationException e) {
            Msg.E("Instantiation exception for module " + getDynamicModuleClass(index).getName() + ". " +
                  "Generic modules have to extend plptool.PLPGenericModule class.",
                  Constants.PLP_DBUSMOD_INSTANTIATION_ERROR, null);
            return Constants.PLP_GENERIC_ERROR;
        } catch(IllegalAccessException e) {
            Msg.E("Illegal access exception for module " + getDynamicModuleClass(index).getName(),
                  Constants.PLP_DBUSMOD_ILLEGAL_ACCESS, null);
            return Constants.PLP_GENERIC_ERROR;
        }
    }

    /**
     * Return the number of instantiated dynamic module objects
     *
     * @return Number of instantiated dynamic module objects
     */
    public static int getNumberOfGenericModuleInstances() {
        return dynamicModuleInstances.size();
    }

    /**
     * Return a reference to the specified dynamic module instance
     *
     * @param index Index of the module object to return
     * @return Reference to the specified object
     */
    public static PLPGenericModule getGenericModuleInstance(int index) {
        if(index < 0 || index >= dynamicModuleInstances.size()) {
            Msg.E("Invalid index.", Constants.PLP_DBUSMOD_GENERIC, null);
            return null;
        }

        return dynamicModuleInstances.get(index);
    }

    /**
     * Check whether an instance of the specified class is already loaded
     *
     * @param className Class name of the module to check
     * @return True if the framework determines that an instance is already
     * loaded, false otherwise
     */
    public static boolean isModuleInstanceLoaded(String className) {
        for(int i = 0; i < getNumberOfGenericModuleInstances(); i++) {
            if(getGenericModuleInstance(i).getClass().getCanonicalName().equals(className))
                return true;
        }

        return false;
    }

    /**
     * Remove the reference to the specified generic module instance
     *
     * @param index Index of the object reference in the list
     * @return The reference to object if found, null otherwise
     */
    public static PLPGenericModule removeGenericModuleInstance(int index) {
        if(index < 0 || index >= dynamicModuleInstances.size()) {
            Msg.E("Invalid index.", Constants.PLP_DBUSMOD_GENERIC, null);
            return null;
        }

        return dynamicModuleInstances.remove(index);
    }

    /**
     * Call the hook function to the specified dynamic module instance
     *
     * @param index Index of the generic module instance
     * @param param Parameters to pass to the hook function
     * @return Object reference returned by the hook function
     */
    public static Object hook(int index, Object param) {
        if(index < 0 || index >= dynamicModuleInstances.size()) {
            Msg.E("Invalid index.", Constants.PLP_DBUSMOD_GENERIC, null);
            return null;
        }

        return dynamicModuleInstances.get(index).hook(param);
    }

    /**
     * Call the hook function of all module instances and pass the same
     * parameters
     *
     * @param param Parameter to pass to all modules
     */
    public static void hook(Object param) {
        for(int i = 0; i < dynamicModuleInstances.size(); i++)
            dynamicModuleInstances.get(i).hook(param);
    }

    /**
     * Mark a module class to be saved to the user cache
     *
     * @param index Index of the class to save
     */
    public static void setModuleClassSave(int index, boolean s) {
        if(index < 0 || index >= dynamicModuleClasses.size()) {
            Msg.E("Invalid index.", Constants.PLP_DBUSMOD_GENERIC, null);
            return;
        }

        savedModuleClass.set(index, s);
    }

    /**
     * Auto-load modules from ~/.plp/autoload/*.jar and apply the manifest file
     */
    public static void autoloadModules(plptool.gui.ProjectDriver plp) {
        Msg.D("Auto-loading modules...", 1, null);
        File autoloadDir = new File(PLPToolbox.getConfDir() + "/autoload");
        if(autoloadDir.exists() && autoloadDir.isDirectory()) {
            File[] files = autoloadDir.listFiles();
            for(int i = 0; i < files.length; i++) {
                if(files[i].getName().endsWith(".jar")) {
                    String[] manifest = loadJarWithManifest(files[i].getAbsolutePath());
                    if(manifest != null && plp != null)
                        applyManifestEntries(manifest, plp);
                }
            }
        }
    }

    /**
     * Delete ~/.plp/autoload directory
     */
    public static void removeAutoloadModules() {
        Msg.D("Removing " + PLPToolbox.getConfDir() + "/plp/autoload...", 1, null);
        File autoloadDir = new File(PLPToolbox.getConfDir() + "/autoload");
        if(autoloadDir.exists() && autoloadDir.isDirectory()) {
            File[] files = autoloadDir.listFiles();
            for(int i = 0; i < files.length; i++)
                files[i].delete();
            autoloadDir.delete();
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
                  Constants.PLP_DBUSMOD_INSTANTIATION_ERROR, null);
            return null;
        } catch(IllegalAccessException e) {
            Msg.E("Illegal access exception for module " + getDynamicModuleClass(index).getName(),
                  Constants.PLP_DBUSMOD_ILLEGAL_ACCESS, null);
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
            
            return true;

        } catch(IOException e) {
            Msg.E("Failed to load classes from '" + path + "'",
                  Constants.PLP_DBUSMOD_FAILED_TO_LOAD_ALL_JAR, null);
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
            Enumeration<JarEntry> entries =  jar.entries();
            JarEntry entry;
            boolean manifestFound = false;

            while(entries.hasMoreElements() && !manifestFound) {
                entry = entries.nextElement();
                if(entry.getName().equals("plp.manifest")) {
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

                    return lines;
                }
            }

            if(!manifestFound) {
                Msg.E("No plp.manifest file found in the JAR archive",
                        Constants.PLP_DBUSMOD_FAILED_TO_LOAD_ALL_JAR, null);
                return null;
            }

        } catch(IOException e) {
            Msg.E("Failed to load classes from '" + path + "'",
                  Constants.PLP_DBUSMOD_FAILED_TO_LOAD_ALL_JAR, null);
            return null;
        }

        return null;
    }

    /**
     * Apply manifest entries to the project
     *
     * @param manifestLines Manifest lines returned by loadJarWithManifest
     * @param plp Reference to the ProjectDriver to apply
     */
    public static void applyManifestEntries(String[] manifestLines, plptool.gui.ProjectDriver plp) {
        for(int i = 0; i < manifestLines.length; i++) {
            if(manifestLines[i].startsWith("loadwithproject::")) {
                String className = manifestLines[i].replace("loadwithproject::", "");
                int cIndex = -1;
                int ret = isModuleClassRegistered(className);
                if(ret > -1)
                    cIndex = newGenericModuleInstance(ret);
                if(cIndex > -1) {
                    Msg.M("Applying manifest entry: " + manifestLines[i]);
                    hook(cIndex, plp);
                }
            }
        }
    }

    /**
     * Return whether a module class is marked as saved to user cache or not
     *
     * @param index Index of the module class in the list
     * @return True if the class is saved, false otherwise
     */
    public static boolean isModuleClassSaved(int index) {
        if(index < 0 || index >= dynamicModuleClasses.size()) {
            Msg.E("Invalid index.", Constants.PLP_DBUSMOD_GENERIC, null);
            return false;
        }

        return savedModuleClass.get(index);
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
                  Constants.PLP_DBUSMOD_GENERIC, null);
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
                while(length > 0) {
                    out.write(array, 0, length);
                    length = in.read(array);
                }

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
                    throw new ClassNotFoundException(name);
                }

                InputStream in = jar.getInputStream(jarEntry);
                int length = in.read(array);
                while(length > 0) {
                    out.write(array, 0, length);
                    length = in.read(array);
                }

                try {
                    ret = defineClass(name, out.toByteArray(), 0, out.size());
                } catch(NoClassDefFoundError e) {
                    Msg.E("defineClass: " + name + " FAILED: " + e, Constants.PLP_GENERIC_ERROR, null);
                    return null;
                }
            }
            return ret;
            
        } catch(Exception e) {
            Msg.E("Unable to load dynamic module " + name + " from the file " + path,
                  Constants.PLP_DBUSMOD_PATH_ERROR, null);
            return null;
        }
    }
}
