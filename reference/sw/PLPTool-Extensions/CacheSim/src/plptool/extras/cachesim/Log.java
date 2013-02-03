/*
    Copyright 2013 Wira Mulia

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

package plptool.extras.cachesim;

import plptool.gui.ProjectDriver;
import java.util.ArrayList;

/**
 *
 * @author Wira
 */
public class Log {
    public static int mode;

    public static final int EDITING = 0;
    public static final int SIMULATION_RESET = 1;
    public static final int SIMULATION_IDLE = 2;
    public static final int SIMULATION_STEP = 3;

    public static ArrayList<Engine> head = new ArrayList<Engine>();
    public static ProjectDriver plp;
    public static DefaultCacheFrame frame;

    public static void reset() {       
        for(Engine e : head) {
            e.logReset();
        }
    }

    public static void read(long addr, long val) {
        for(Engine e : head) {
            e.read(addr, val);
        }
    }

    public static void write(long addr, long val) {
        for(Engine e : head) {
            e.write(addr, val);
        }
    }
}
