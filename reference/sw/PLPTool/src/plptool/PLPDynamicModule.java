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

import java.io.File;
import java.io.FileInputStream;
import java.util.ArrayList;

/**
 *
 * @author wira
 */
public class PLPDynamicModule {
    private static int index = 0;
    private static ArrayList<PLPSimBusModule> dynamicModules;
    private static boolean warn = false;

    public static boolean loadModule(String path, String className) {
        if(!warn) {
            Msg.W("YOU ARE LOADING A DYNAMIC MODULE, THIS COULD BE POTENTIALLY DANGEROUS", null);
            Msg.W("Make sure you only use trusted third party modules!", null);
            warn = true;
        }
        Msg.M("--- Loading module class " + className + " from " + path + "...");
        PLPSimBusModule mod;
        ClassLoader parent = PLPDynamicModule.class.getClassLoader();
        PLPDynamicModuleClassLoader loader = new PLPDynamicModuleClassLoader(parent, path, className);
        if(loader == null)
            return false;
        try {
            Class dynamicModuleClass = loader.loadClass(className);
            if(dynamicModuleClass == null) {
                return false;
            }
            if(dynamicModules == null)
                dynamicModules = new ArrayList<PLPSimBusModule>();
            Msg.M("--- Adding " + className + " to dynamic module registry...");
            mod = (PLPSimBusModule) dynamicModuleClass.newInstance();
            dynamicModules.add(mod);
            Msg.M("--- " + mod.toString() + " (class: " + className + ") is attached to index " + index);
            index++;
        } catch(ClassNotFoundException e) {
            Msg.E("The class " + className + " is not found in " + path,
                  Constants.PLP_DBUSMOD_CLASS_NOT_FOUND_ERROR, null);
            return false;
        } catch(InstantiationException e) {
            Msg.E("Instantiation exception for module " + className + ". " +
                  "Dynamic modules have to extend plptool.PLPSimBusModule class.",
                  Constants.PLP_DBUSMOD_INSTANTIATION_ERROR, null);
            return false;
        } catch(IllegalAccessException e) {
            Msg.E("Illegal access exception for module " + className,
                  Constants.PLP_DBUSMOD_ILLEGAL_ACCESS, null);
            return false;
        } catch(Exception e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }

    public static PLPSimBusModule getDynamicModule(int index) {
        if(index < 0 || index >= dynamicModules.size())
            return null;

        return dynamicModules.get(index);
    }
}

class PLPDynamicModuleClassLoader extends ClassLoader {

    private String path;
    private String nameToCheck;

    public PLPDynamicModuleClassLoader(ClassLoader parent, String path, String nameToCheck) {
        super(parent);
        this.path = path;
        this.nameToCheck = nameToCheck;
    }

    @Override
    public Class loadClass(String name) throws ClassNotFoundException {
        if(!name.equals(nameToCheck))
            return super.loadClass(name, true);

        try {
            File file = new File(path);
            FileInputStream in = new FileInputStream(file);
            byte[] data = new byte[(int) file.length()];
            in.read(data, 0, (int) file.length());

            Class ret = defineClass(name, data, 0, data.length);
            return ret;
            
        } catch(Exception e) {
            Msg.E("Unable to load dynamic module " + name + " from the file " + path,
                  Constants.PLP_DBUSMOD_PATH_ERROR, null);
            return null;
        }
    }
}
