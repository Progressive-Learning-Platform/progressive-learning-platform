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

import java.util.ArrayList;

/**
 * Presets for I/O registry. This class is used to preserve bus modules
 * states on the project.
 *
 * @author wira
 */
public class Preset {
    
    private ArrayList<Integer> types;
    private ArrayList<Long> addresses;
    private ArrayList<Long> sizes;
    private ArrayList<Boolean> hasFrame;
    private ArrayList<Boolean> frameVisible;
    private ArrayList<Integer> frX;
    private ArrayList<Integer> frY;
    private ArrayList<Integer> frW;
    private ArrayList<Integer> frH;

    public Preset() {
        types = new ArrayList<Integer>();
        addresses = new ArrayList<Long>();
        sizes = new ArrayList<Long>();
        hasFrame = new ArrayList<Boolean>();
        frameVisible = new ArrayList<Boolean>();
        frX = new ArrayList<Integer>();
        frY = new ArrayList<Integer>();
        frW = new ArrayList<Integer>();
        frH = new ArrayList<Integer>();
    }

    public void addModuleDefinition(int type, long startAddr, long size, boolean frame, boolean visible, int X, int Y, int W, int H) {
        types.add(type);
        addresses.add(startAddr);
        sizes.add(size);
        hasFrame.add(frame);
        frameVisible.add(visible);
        frX.add(X);
        frY.add(Y);
        frW.add(W);
        frH.add(H);
    }

    public void addModuleDefinition(int type, long startAddr, long size, boolean frame, boolean visible) {
        types.add(type);
        addresses.add(startAddr);
        sizes.add(size);
        hasFrame.add(frame);
        frameVisible.add(visible);
        frX.add(0);
        frY.add(0);
        frW.add(0);
        frH.add(0);
    }

    public int getType(int index) {
        return types.get(index);
    }

    public long getAddress(int index) {
        return addresses.get(index);
    }

    public long getSize(int index) {
        return sizes.get(index);
    }

    public boolean getHasFrame(int index) {
        return hasFrame.get(index);
    }

    public boolean getVisible(int index) {
        return frameVisible.get(index);
    }

    public int size() {
        return types.size();
    }

    public int getX(int index) {
        return frX.get(index);
    }

    public int getY(int index) {
        return frY.get(index);
    }

    public int getW(int index) {
        return frW.get(index);
    }

    public int getH(int index) {
        return frW.get(index);
    }

    public final static Object[][] presets = new Object[][]{
        /**********************************************************************/
        {
            "PLP 2.2 I/O Configuration",

            new Integer[]{0, 0, 1, 2, 4, 7, 9, 8, 6, 12},

            new Long[]{0x0L,
                       0x10000000L,
                       0xf0200000L,
                       0xf0100000L,
                       0xf0500000L,
                       0xf0600000L,
                       0xf0a00000L,
                       0xf0000000L,
                       0xf0400000L,
                       0xf0300000L},

            new Long[]{0x800L,
                       0x1000000L,
                       1L,
                       1L,
                       2L,
                       1L,
                       1L,
                       4L,
                       2L,
                       3L}
        },
 
        /**********************************************************************/
        {
            "PLP 2.2 ROM and RAM only",

            new Integer[]{0, 0},

            new Long[]{0x0L,
                       0x10000000L},

            new Long[]{0x800L,
                       0x1000000L}
        },

	/**********************************************************************/
        {
            "PLP 2.2 Non-Memory I/O Configuration",

            new Integer[]{1, 2, 4, 7, 9, 8, 6, 12},

            new Long[]{0xf0200000L,
                       0xf0100000L,
                       0xf0500000L,
                       0xf0600000L,
                       0xf0a00000L,
                       0xf0000000L,
                       0xf0400000L,
                       0xf0300000L},

            new Long[]{1L,
                       1L,
                       2L,
                       1L,
                       1L,
                       4L,
                       2L,
                       3L}
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
            "Tracer covering whole memory space",

            new Integer[]{3},

            new Long[]{0x0L},

            new Long[]{0xffffffffL}
        }
    };


}
