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
    
    private int wordSize;
    private int blockSize;
    private int associativity;
    private int blocks;
    
    private long[][] linesBase;
    private boolean[][] dirty;
    private boolean[][] invalid;
    private int[][] lru;
        
    public DefaultCache(Engine prev, Engine...engines) {
        super(prev, engines);
    }
    
    public void setProperties(int wordSize, int blockSize, int associativity, int blocks) {
        this.wordSize = wordSize;
        this.blockSize = blockSize;
        this.associativity = associativity;
        this.blocks = blocks;
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
        boolean hit = false;
        if(!cacheInstr && PLPToolConnector.plp.sim.bus.isInstr(addr))
            return -1;
        if(!cacheData && !PLPToolConnector.plp.sim.bus.isInstr(addr))
            return -1;

        stats.read_accesses++;
        index = (int) ((addr >> (wordSize / 8 - 1)) >> (blockSize / 8 - 1))
                % (blocks / associativity);
        for(i = 0; i < associativity; i++) {
            if(linesBase[index][i] == addr && !invalid[index][i]) {
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
}
