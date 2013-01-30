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
    private boolean cacheWrites;
    
    private int wordSize;
    private int blockSize;
    private int associativity;
    private int blocks;
    
    private long[][] linesBase;
    private boolean[][] dirty;
    private int[] lru;
        
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
        linesBase = new long[associativity][];
        for(i = 0; i < linesBase.length; i++) {
            linesBase[i] = new long[setBlocks];
            for(j = 0; j < setBlocks; j++) {
                linesBase[i][j] = 0;
                dirty[i][j] = true;
            }
        }
        for(i = 0; i < setBlocks; i++)
            lru[i] = 0;
    }
    
    public int read(long addr, long val) {
        return 0;
    }

    public int write(long addr, long val) {
        return 0;
    }
}
