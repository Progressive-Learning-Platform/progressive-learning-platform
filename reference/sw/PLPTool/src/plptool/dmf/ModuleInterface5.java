/*
    Copyright 2012 PLP Contributors

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

import plptool.gui.ProjectDriver;

/**
 * Module interface for PLPTool 5.x series. This supersedes the original
 * PLPGenericModule interface from PLPTool 4.1. Although the old interface
 * is still supported, this is the preferred method of implementing a PLPTool
 * module.
 *
 * @author Wira
 */
public interface ModuleInterface5 {
    /**
     * Initialization function called by the framework as the module is loaded.
     * The framework passes the current project driver to the module and the
     * module should keep this reference if it has to interact with the project
     * at all. This is also the preferred place to register callbacks.
     *
     * @param plp A reference to the currently active project driver instance
     * @return The module should return a status code. Anything other than
     * PLP_OK will trigger an error during the loading process
     */
    abstract public int initialize(ProjectDriver plp);

    /**
     * Module name to be listed in the Module Manager frame
     *
     * @return Module name as String
     */
    abstract public String getName();

    /**
     * Module version in integer array. The array needs to have AT LEAST
     * a major and minor version (index 0 and 1, respectively). If the
     * returned array length is less than 2, an error will be thrown.
     *
     * @return Module version as integer array with at least a length of 2
     */
    abstract public int[] getVersion();

    /**
     * Required minimum PLPTool version in integer array. The array needs to
     * be of exactly length of 2.
     *
     * @return Required minimum PLPTool version as integer array with at least
     * a length of 2
     */
    abstract public int[] getMinimumPLPToolVersion();

    /**
     * Return a description of the module that will be displayed in the module
     * manager
     *
     * @return Module description as string
     */
    abstract public String getDescription();
}
