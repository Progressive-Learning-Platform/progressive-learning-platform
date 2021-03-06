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

import plptool.dmf.Callback;

/**
 *
 * @author Wira
 */
public class BusReadCallback implements Callback {
    public boolean callback(int num, Object param) {
        if(Log.mode != Log.SIMULATION_STEP)
            return false;
        Object[] p = (Object[]) param;
        Log.read((Long) p[0], (Long) p[1]);
        return true;
    }
}
