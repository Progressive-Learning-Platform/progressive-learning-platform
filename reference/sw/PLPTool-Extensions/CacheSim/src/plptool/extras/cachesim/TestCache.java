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
 * Sample 4-line, 32-bit line size, direct cache, write-through
 *
 * @author Wira
 */
public class TestCache extends Engine {
    long[] lines;
    boolean[] dirty;

    public TestCache(Engine prev, Engine...engines) {
        super(prev, engines);
        reset();
    }
    
    public int read(long addr, long val) {
        // ignore instruction fetches
        if(PLPToolConnector.plp.sim.bus.isInstr(addr))
            return 0;

        Msg.M("CacheSim: TestCache non-instruction read " + PLPToolbox.format32Hex(addr));

        int index = (int) ((addr>>2) % 4);
        stats.read_accesses++;
        if(lines[index] == addr && !dirty[index])
            stats.read_hits++;
        else {
            lines[index] = addr;
            dirty[index] = false;
            propagateRead(addr, val);
        }

        return 0;
    }

    public int write(long addr, long val) {
        return 0;
    }

    public final void reset() {
        lines = new long[4];
        dirty = new boolean[4];
        for(int i = 0; i < 4; i++) {
            dirty[i] = true;
        }
    }

    @Override
    public String dumpContents() {
        String str = "";
        for(int i = 0; i < lines.length; i++) {
            str += i + "\t" + PLPToolbox.format32Hex(lines[i]) + "\t" + dirty[i] + "\n";
        }

        return str;
    }
}
