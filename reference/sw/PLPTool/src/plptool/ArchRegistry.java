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

    public static final int        ISA_PLPMIPS         =           0;

        // ... Add your ISA ID here ... //

    /**
     * Return the specified architecture meta class. Register your ISA
     * meta class here (extension of plptool.PLPArchitecture).
     *
     * @param index
     */
    public static PLPArchitecture getArchitecture(ProjectDriver plp,
            int index) {
        PLPArchitecture arch = null;

        // default ISA is PLP CPU (arch ID 0)
        if(index == 0)
            return (new plptool.mips.Architecture(index, plp));

        try {
            arch = (PLPArchitecture) archClasses.get(index).newInstance();
            return arch;
        } catch(Exception e) {
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
    public static int registerArchitecture(Class arch, int ID) {
        if(!arch.getSuperclass().getCanonicalName().equals("plptool.PLPArchitecture"))
            return Msg.E("Specified class does not extend the PLP " +
                    "architecture superclass.",
                    Constants.PLP_ISA_INVALID_METACLASS, null);

        if(ID == 0 || archClasses.containsKey(ID))
            return Msg.E("ISA with ID '" + ID + "' is already defined.",
                    Constants.PLP_ISA_ALREADY_DEFINED, null);

        archClasses.put(ID, arch);

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
}
