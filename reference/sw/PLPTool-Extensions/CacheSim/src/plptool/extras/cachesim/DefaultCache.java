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

import java.util.ArrayList;

/**
 *
 * @author Wira
 */
public class DefaultCache extends Engine {
    private boolean cacheInstr;
    private boolean cacheData;
    private boolean cacheWrite;
    private boolean writeThrough;
    private boolean writeAllocate;
    
    public int blockOffset;
    public int associativity;
    public int blocks;
    public int indexBits;
    
    public long[][] linesBase;
    public boolean[][] dirty;
    public boolean[][] invalid;
    public int[][] lru;
    
    public long lastAccess;
    public int lastAccessType;
    public boolean lastHit;
    public int lastSlot;
    public final ArrayList<Long> trace;

    public final int TRACE_SIZE = 5;
        
    public DefaultCache(Engine prev, Engine...next) {
        super(prev, next);
        trace = new ArrayList<Long>();
    }
    
    public void setProperties(int blockOffset, int associativity, int blocks, 
            boolean cacheInstr, boolean cacheData,
            boolean cacheWrite, boolean writeThrough, boolean writeAllocate) {
        this.blockOffset = blockOffset;
        this.associativity = associativity;
        this.blocks = blocks;
        this.cacheInstr = cacheInstr;
        this.cacheData = cacheData;
        this.cacheWrite = cacheWrite;
        this.writeThrough = writeThrough;
        this.writeAllocate = writeAllocate;
        indexBits = (int) (Math.log(blocks / associativity) / Math.log(2));
        trace.clear();
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
        
        lastAccess = -1;
        lastAccessType = -1;
        lastSlot = -1;
        lastHit = false;
        trace.clear();
    }
    
    public int read(long addr, long val) {
        if(!cacheInstr && Log.plp.sim.bus.isInstr(addr))
            return -1;
        if(!cacheData && !Log.plp.sim.bus.isInstr(addr))
            return -1;
        
        int index, lruIndex, i;
        long tag;
        boolean hit = false;        
        lastHit = false;
        
        lastAccess = addr;
        lastAccessType = 1;
        stats.read_accesses++;
        trace(addr);
        index = (int) (addr >> blockOffset)
                % (blocks / associativity);
        for(i = 0; i < associativity; i++) {
            tag = (linesBase[index][i] >> blockOffset);
            if(tag == (addr >> blockOffset) && !invalid[index][i]) {
                stats.read_hits++;
                lru[index][i] = 0;
                hit = true;
                lastHit = true;
                lastSlot = i;
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
                propagateWrite(linesBase[index][lruIndex], val); // TODO: store value
                stats.write_backs++;
            }
            lastSlot = lruIndex;
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
        
        int index, lruIndex, i;
        long tag;
        boolean hit = false;
        lastHit = false;
        
        lastAccess = addr;
        lastAccessType = 2;
        stats.write_accesses++;
        trace(addr);
        index = (int) (addr >> blockOffset)
                % (blocks / associativity);
        for(i = 0; i < associativity; i++) {
            tag = (linesBase[index][i] >> blockOffset);
            if(tag == (addr >> blockOffset) && !invalid[index][i]) {
                stats.write_hits++;
                lru[index][i] = 0;
                hit = true;
                lastHit = true;
                lastSlot = i;
                if(writeThrough)
                    propagateWrite(addr, val);
                else
                    dirty[index][i] = true;                    
            } else {
                lru[index][i]++;
            }
        }
        
        if(!hit && writeAllocate) {
            lruIndex = 0;
            for(i = 1; i < associativity; i++) {
                if(lru[index][i] > lru[index][lruIndex])
                    lruIndex = i;
            }
            lru[index][lruIndex] = 0;
            if(dirty[index][lruIndex]) {
                // propagate write to flush this line
                propagateWrite(linesBase[index][lruIndex], val); // TODO: store value
                stats.write_backs++;
            }
            
            // replace cache line with new data            
            lastSlot = lruIndex;
            linesBase[index][lruIndex] = addr;
            invalid[index][lruIndex] = false;
            if(writeThrough) {
                dirty[index][lruIndex] = false;
                propagateWrite(addr, val);
            } else {
                dirty[index][lruIndex] = true;                
            }
        }
        
        if(!hit && !writeAllocate)
            propagateWrite(addr, val);

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

    private void trace(long addr) {
        synchronized(trace) {
            trace.add(addr);
            if(trace.size() > TRACE_SIZE)
                trace.remove(0);
        }
    }

    public int getTraceSize() {
        synchronized(trace) {
            return trace.size();
        }
    }
    
    public Long getTraceItem(int i) {
        synchronized(trace) {
            return trace.get(i);
        }
    }
}
