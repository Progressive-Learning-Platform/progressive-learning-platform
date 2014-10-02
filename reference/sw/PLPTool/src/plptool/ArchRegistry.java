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

package plptool;

import plptool.gui.ProjectDriver;

import java.util.HashMap;
import java.util.Map;

/**
 * This class associates an ISA implementation to its member classes.
 * Developers porting an ISA to PLP Tool framework will have to register their
 * implementation here.
 *
 * @author wira
 */
public class ArchRegistry {

    private static HashMap<Integer, Class> archClasses =
            new HashMap<Integer, Class>();
    private static HashMap<Integer, String> archIdentifiers =
            new HashMap<Integer, String>();
    private static HashMap<Integer, String> archDescriptions =
            new HashMap<Integer, String>();

    public static final int        ISA_PLPMIPS         =           0;

        // ... Add your ISA ID here ... //

    /**
     * Return the specified architecture meta class. Register your ISA
     * meta class here (extension of plptool.PLPArchitecture).
     *
     * @param index
     */
    public static PLPArchitecture getArchitecture(ProjectDriver plp,
            int ID) {
        PLPArchitecture arch = null;
        Class archClass = null;

        // default ISA is PLP CPU (arch ID 0)
        if(ID == 0)
            return (new plptool.mips.Architecture(ID, plp));

        // Try to load dynamic ISA module
        try {
            archClass = getRegisteredArchitectureClass(ID);
            if(archClass == null) {
                Msg.E("Invalid ISA ID: " + ID,
                        Constants.PLP_ISA_INVALID_ARCHITECTURE_ID, null);
                return null;
            }

            arch = (PLPArchitecture) getRegisteredArchitectureClass(ID).newInstance();
            arch.setProjectDriver(plp);
            return arch;
        } catch(Exception e) {
            Msg.E("Instantiation error for " +
                  getRegisteredArchitectureClass(ID).getCanonicalName(),
                  Constants.PLP_DMOD_INSTANTIATION_ERROR, null);
            Msg.trace(e);
            return arch;
        }
    }

    /**
     * Register a class as an ISA metaclass
     *
     * @param arch Architecture class that extends plptool.PLPArchitecture
     * @param ID Integer ID for the ISA
     * @return PLP_OK if the class is successfully registered, error code
     * otherwise
     */
    public static int registerArchitecture(Class arch, int ID, String strID,
            String description) {
        Msg.D("Registering ISA class " + arch.getCanonicalName() +
                " with ID=" + ID + " strID=" + strID, 2, null);

        if(!arch.getSuperclass().getCanonicalName().equals("plptool.PLPArchitecture"))
            return Msg.E("Specified class does not extend the PLP " +
                    "architecture superclass.",
                    Constants.PLP_ISA_INVALID_METACLASS, null);

        if(ID == 0 || archClasses.containsKey(ID))
            return Msg.E("ISA with ID '" + ID + "' is already defined.",
                    Constants.PLP_ISA_ALREADY_DEFINED, null);
        
        archClasses.put(ID, arch);
        archIdentifiers.put(ID, strID);
        archDescriptions.put(ID, description);

        if(Constants.debugLevel >= 5) {
            java.util.Set IDs = archClasses.keySet();
            Object stuff[] = IDs.toArray();
            Msg.D("Current list of registered ISA IDs:", 5, null);
            for(int i = 0; i < stuff.length; i++) {
                Msg.D("- " + (Integer) stuff[i], 5, null);
            }
        }

        return Constants.PLP_OK;
    }

    /**
     * Get a reference to an ISA class specified by its ID
     *
     * @param ID ID of the ISA meta class
     * @return Reference to the ISA class, or null if it is not registered
     */
    public static Class getRegisteredArchitectureClass(int ID) {
        return archClasses.get(ID);
    }

    /**
     * Get the string Identifier of the ISA implementation
     *
     * @param ID ID of the ISA meta class
     * @return the string identifier of the ISA
     */
    public static String getStringID(int ID) {
        return archIdentifiers.get(ID);
    }

    /**
     *
     * @param ID
     * @return
     */
    public static String getDescription(int ID) {
        return archDescriptions.get(ID);
    }

    /**
     * PLPToolApp calls this method during initialization
     */
    public static String[] setup(String[] args) {
        archClasses.put(0, plptool.mips.Architecture.class);
        archIdentifiers.put(0, "PLPCPU");
        archDescriptions.put(0, "PLP CPU ISA Implementation for PLPTool. " +
                "This is the default built-in ISA for the PLP project.");
        return args;
    }

    /**
     * Return a list of the ISAs currently registered in the PLPTool session.
     *
     * @return A two-dimensional object array of registered ISA classes.
     * Information fields are: 0: ISA numerical ID, 1: reference to the ISA
     * meta-class, 2: ISA string identifier.
     */
    public static Object[][] getArchList() {
        Object[][] archs = new Object[archClasses.size()][4];
        Object[] classes = archClasses.entrySet().toArray();
        for(int i = 0; i < archs.length; i++) {
            @SuppressWarnings("unchecked")
            Map.Entry<Integer, Class> entry = (Map.Entry<Integer, Class>) classes[i];
            archs[i][0] = entry.getKey();
            archs[i][1] = entry.getValue();
            archs[i][2] = archIdentifiers.get(entry.getKey());
            archs[i][3] = archDescriptions.get(entry.getKey());
        }
        return archs;
    }
}
