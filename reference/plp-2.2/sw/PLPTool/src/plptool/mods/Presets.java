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
            "PLP 2.2 I/O Configuration",

            new Integer[]{0, 1, 2, 4, 7, 9, 8},

            new Long[]{0x0L,
                       0xf0200000L,
                       0xf0100000L,
                       0xf0500000L,
                       0xf0600000L,
                       0xf0a00000L,
                       0xf0000000L},

            new Long[]{0x10000000L,
                       1L,
                       1L,
                       2L,
                       1L,
                       1L,
                       4L,}
        },

        /**********************************************************************/
        {
            "PLP VGA Module at 0xf0400000",

            new Integer[]{6},

            new Long[]{0xf0400000L},

            new Long[]{2L}
        },

        /**********************************************************************/
        {
            "PLP 2.1 I/O Configuration",

            new Integer[]{0, 1, 2, 4, 6},

            new Long[]{0x0L,
                       0xf0200000L,
                       0xf0100000L,
                       0xf0500000L,
                       0xf0400000L},

            new Long[]{0x10000000L,
                       1L,
                       1L,
                       2L,
                       2L}
        },
        /**********************************************************************/
        {
            "2 LEDs",

            new Integer[]{1, 1},

            new Long[]{0x4000L,
                       0x4004L},

            new Long[]{1L,
                       1L}
        },
        /**********************************************************************/
        {
            "Tracer covering whole memory space",

            new Integer[]{3},

            new Long[]{0x0L},

            new Long[]{0xffffffffL}
        }
    };


}
