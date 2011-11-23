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
import plptool.gui.ProjectDriver;

/*****************************************************************************
 * IMPORT YOUR ISA PACKAGE HERE
 *****************************************************************************/
import plptool.mips.*;

// ...add 3rd party packages... //

/**
 * This class associates an ISA implementation to its member classes.
 * Developers porting an ISA to PLP system will have to register their
 * implementation here. Starting with importing their package above.
 *
 * @author wira
 */
public class ArchRegistry {

    public static final int        ISA_PLPMIPS         =           0;

        // ... Add your ISA ID here ... //

    /**
     * Return the specified architecture meta class. Register your ISA
     * meta class here (extension of plptool.PLPArchitecture).
     *
     * @param index
     */
    public static PLPArchitecture getArchitectureMetaClass(ProjectDriver plp, int index) {
        
        PLPArchitecture arch = null;
        
        switch(index) {
            
            /*****************************************************************/
            /* PLP MIPS Architecture Meta Class Instantation                 */
            /*****************************************************************/
            case 0:
                arch = new plptool.mips.Architecture(ISA_PLPMIPS, plp);
                
            // ... Add your meta class instantiation here ... ///              
        }
        
        return arch;
    }
}
