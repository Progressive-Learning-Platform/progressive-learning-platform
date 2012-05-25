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

/**
 * Callback interface for the dynamic module framework. Modules that need to
 * register a callback will have to implement this interface and pass an
 * instance of the callback to the appropriate register method in
 * CallbackRegistry class.
 *
 * @author Wira
 */
public interface Callback {
    /**
     * The callback function can take any type of parameter. The callback
     * should return true if it actually does work for the event (such as
     * handling a file when the project file is being open to notify
     * ProjectDriver that the file is handled).
     *
     * @param callbackNum The number of the callback event that occurred
     * @param param Reference to the parameters passed to this callback
     * @return
     */
    abstract public boolean callback(int callbackNum, Object param);
}
