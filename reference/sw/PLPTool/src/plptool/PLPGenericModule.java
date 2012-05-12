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

/**
 * PLP generic module interface. This interface is used as a framework for
 * dynamic module loading and interfacing for PLPTool version 4.1. Use
 * plptool.dmf.ModuleInterface for modules intended for newer PLPTool versions.
 *
 * @author wira
 */
public interface PLPGenericModule {
    /**
     * Generic hook function for the module.
     *
     * @param param Reference to hook parameters
     * @return Reference to return values
     */
    abstract Object hook(Object param);

    /**
     * Version identifier
     *
     * @return Version of the module in String
     */
    abstract String getVersion();
}
