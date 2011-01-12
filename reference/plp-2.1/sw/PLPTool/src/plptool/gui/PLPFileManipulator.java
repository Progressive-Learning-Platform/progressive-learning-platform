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

package plptool.gui;

/**
 *
 * @author wira
 */
public class PLPFileManipulator {
    public static void CLI(String[] args) {
        PLPBackend backend = new PLPBackend(false, "plpmips");
        backend.openPLPFile(args[1]);
        
        if(backend.plpfile == null)
            return;

        if(args[2].equals("-importasm") || args[2].equals("-i")) {
            backend.importAsm(args[3]);
            backend.savePLPFile();
        }
    }
}
