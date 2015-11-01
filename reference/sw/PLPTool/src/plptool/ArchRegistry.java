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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import plptool.gui.ProjectDriver;
import plptool.mips.Architecture;

/**
 * This class associates an ISA implementation to its member classes.
 * Developers porting an ISA to PLP Tool framework will have to register their
 * implementation here.
 *
 * @author wira
 */
public class ArchRegistry {
    public static final int ISA_PLPMIPS = 0;
    
    private static Map<Integer, ArchitectureInformation> architectures;
    
    static {
    	architectures = new HashMap<>();
    	
		// Register default ISA
		registerArchitecture(Architecture.class, ISA_PLPMIPS, 
        		"PLPCPU", "PLP CPU ISA Implementation for PLPTool. "
        				+ "This is the default built-in ISA for the PLP project.");
	}

    /**
     * Return the specified architecture meta class. Register your ISA
     * meta class here (extension of plptool.PLPArchitecture).
     *
     * @param index
     */
    public static PLPArchitecture getArchitecture(ProjectDriver plp,
            int ID) {
        PLPArchitecture arch = null;
        Class<?> archClass = null;

        if(ID == ISA_PLPMIPS)
            return (new plptool.mips.Architecture(ID, plp));

        // Try to load dynamic ISA module
        try {
            archClass = getRegisteredArchitectureClass(ID);
            if(archClass == null) {
                Msg.error("Invalid ISA ID: " + ID,
                        Constants.PLP_ISA_INVALID_ARCHITECTURE_ID, null);
                return null;
            }

            arch = (PLPArchitecture) getRegisteredArchitectureClass(ID).newInstance();
            arch.setProjectDriver(plp);
            return arch;
        } catch(Exception e) {
            Msg.error("Instantiation error for " +
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
    public static int registerArchitecture(Class<?> arch, int ID, String strID,
            String description) {
        Msg.debug("Registering ISA class " + arch.getCanonicalName() +
                " with ID=" + ID + " strID=" + strID, 2, null);

        if(!arch.getSuperclass().getCanonicalName().equals("plptool.PLPArchitecture"))
            return Msg.error("Specified class does not extend the PLP " +
                    "architecture superclass.",
                    Constants.PLP_ISA_INVALID_METACLASS, null);

        if(architectures.containsKey(ID)) {
        	new Exception().printStackTrace();
            return Msg.error("ISA with ID '" + ID + "' is already defined.",
                    Constants.PLP_ISA_ALREADY_DEFINED, null);
        }
        
        ArchitectureInformation architecture = 
        		new ArchitectureInformation(ID, arch, strID, description);
        architectures.put(ID, architecture);

        if(Constants.debugLevel >= 5) {
        	// TODO: cleanup
            Set<?> IDs = architectures.keySet();
            Object stuff[] = IDs.toArray();
            Msg.debug("Current list of registered ISA IDs:", 5, null);
            for(int i = 0; i < stuff.length; i++) {
                Msg.debug("- " + (Integer) stuff[i], 5, null);
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
    public static Class<?> getRegisteredArchitectureClass(int ID) {
    	ArchitectureInformation architecture = architectures.get(ID);
    	
    	if (architecture == null)
    		return null;
    	else
    		return architecture.getType();
    }

    /**
     * Get the string Identifier of the ISA implementation
     *
     * @param ID ID of the ISA meta class
     * @return the string identifier of the ISA
     */
    public static String getStringID(int ID) {
ArchitectureInformation architecture = architectures.get(ID);
    	
    	if (architecture == null)
    		return null;
    	else
    		return architecture.getIdentifier();
    }

    /**
     *
     * @param ID
     * @return
     */
    public static String getDescription(int ID) {
    	ArchitectureInformation architecture = architectures.get(ID);
    	
    	if (architecture == null)
    		return null;
    	else
    		return architecture.getDescription();
    }

    /**
	 * Return a list of the ISAs currently registered in the PLPTool session.
	 *
	 * @return A two-dimensional object array of registered ISA classes. Information
	 *         fields are: 0: ISA numerical ID, 1: reference to the ISA meta-class, 2: ISA
	 *         string identifier.
	 */
	public static List<ArchitectureInformation> getArchList()
	{
		List<ArchitectureInformation> architectureList = new ArrayList<>();
		architectureList.addAll(architectures.values());
		
		return architectureList;
	}
	
	public static class ArchitectureInformation
	{
		private int id;
		private Class<?> type;
		private String identifier;
		private String description;
		
		public ArchitectureInformation()
		{
			
		}
		
		public ArchitectureInformation(int id, Class<?> type, String identifier,
				String description)
		{
			super();
			this.id = id;
			this.type = type;
			this.identifier = identifier;
			this.description = description;
		}
		
		public int getId()
		{
			return id;
		}
		
		public Class<?> getType()
		{
			return type;
		}
		
		public String getIdentifier()
		{
			return identifier;
		}
		
		public String getDescription()
		{
			return description;
		}
	}
}
