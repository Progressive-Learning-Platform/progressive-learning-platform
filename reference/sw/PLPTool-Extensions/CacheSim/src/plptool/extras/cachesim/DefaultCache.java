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

import plptool.*;

/**
 *
 * @author Wira
 */
public class DefaultCache extends Engine {
    private boolean cacheInstr;
    private boolean cacheData;
    private boolean cacheWrite;
    private boolean writeThrough;
    private boolean writeBack;
    
    private int wordOffset;
    private int blockOffset;
    private int associativity;
    private int blocks;
    
    private long[][] linesBase;
    private boolean[][] dirty;
    private boolean[][] invalid;
    private int[][] lru;
        
    public DefaultCache(Engine prev, Engine...engines) {
        super(prev, engines);
    }
    
    public void setProperties(int wordOffset, int blockOffset, int associativity, int blocks, 
            boolean cacheInstr, boolean cacheData,
            boolean cacheWrite, boolean writeThrough, boolean writeBack) {
        this.wordOffset = wordOffset;
        this.blockOffset = blockOffset;
        this.associativity = associativity;
        this.blocks = blocks;
        this.cacheInstr = cacheInstr;
        this.cacheData = cacheData;
        this.cacheWrite = cacheWrite;
        this.writeThrough = writeThrough;
        this.writeBack = writeBack;
    }

    public final void reset() {
        int i, j;
        int setBlocks = blocks / associativity;
        linesBase = new long[setBlocks][];
        lru = new int[setBlocks][];
        dirty = new boolean[setBlocks][];
        invalid = new boolean[setBlocks][];
        for(i = 0; i < linesBase.length; i++) {
            linesBase[i] = new long[associativity];
            lru[i] = new int[associativity];
            dirty[i] = new boolean[associativity];
            invalid[i] = new boolean[associativity];
            for(j = 0; j < associativity; j++) {
                linesBase[i][j] = 0;
                lru[i][j] = 0;
                dirty[i][j] = false;
                invalid[i][j] = true;
            }
        }
    }
    
    public int read(long addr, long val) {
        int index, i, lruIndex;
        long tag;
        boolean hit = false;
        if(!cacheInstr && Log.plp.sim.bus.isInstr(addr))
            return -1;
        if(!cacheData && !Log.plp.sim.bus.isInstr(addr))
            return -1;

        stats.read_accesses++;
        index = (int) ((addr >> wordOffset) >> blockOffset)
                % (blocks / associativity);
        for(i = 0; i < associativity; i++) {
            tag = ((linesBase[index][i] >> wordOffset) >> blockOffset);
            if(tag == ((addr >> wordOffset) >> blockOffset) && !invalid[index][i]) {
                stats.read_hits++;
                lru[index][i] = 0;
                hit = true;
            } else {
                lru[index][i]++;
            }
        }

        if(!hit) {
            lruIndex = 0;
            for(i = 1; i < associativity; i++) {
                if(lru[index][i] > lru[index][lruIndex])
                    lruIndex = i;
            }
            lru[index][lruIndex] = 0;
            if(dirty[index][lruIndex]) {
                // propagate write to flush this line
            }
            dirty[index][lruIndex] = false;
            linesBase[index][lruIndex] = addr;
            invalid[index][lruIndex] = false;
            propagateRead(addr, val);
        }

        return 0;
    }

    public int write(long addr, long val) {
        if(!cacheWrite)
            return -1;

        return 0;
    }
    
    @Override
    public String dumpContents() {
        int i, j;
        String str = "";
        for(i = 0; i < linesBase.length; i++) {
            str += "set " +  i + "\t";
            for(j = 0; j < associativity;j ++) {
                str += PLPToolbox.format32Hex(linesBase[i][j]) + ":" + (dirty[i][j] ? "d" : "_") + 
                        (invalid[i][j] ? "i" : "_") + " ";
            }
            str += "\n";
        }

        return str;
    }
}
