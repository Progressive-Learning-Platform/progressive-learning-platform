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

package plptool.mods;

/**
 * Presets for I/O registry.
 *
 * @author wira
 */
public class Presets {
    public final static Object[][] presets = new Object[][]{
        /**********************************************************************/
        {
            "PLP 2.0 I/O Configuration",

            new Integer[]{0, 1, 2},

            new Long[]{(long) 0x10000000,
                       (long) 0xf020000 << 4,
                       (long) 0xf010000 << 4},

            new Long[]{(long) 8192,
                       (long) 1,
                       (long) 1}
        },
        /**********************************************************************/
        {
            "2 LEDs",

            new Integer[]{1, 1},

            new Long[]{(long) 0x4000,
                       (long) 0x4004},

            new Long[]{(long) 1,
                       (long) 1}
        }
    };


}
