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

import java.util.ArrayList;
import java.util.Arrays;

/**
 *
 * @author Wira
 */
public abstract class Engine {
    protected ArrayList<Engine> next;
    protected Engine prev;
    protected Statistics stats;

    public abstract int read(long addr, long val);
    public abstract int write(long addr, long val);
    abstract void reset();

    public Engine(Engine prev, Engine...next) {
        this.next = new ArrayList<Engine>();
        this.next.addAll(Arrays.asList(next));
        this.prev = prev;
        this.stats = new Statistics();
    }

    public Engine getPrev() {
        return prev;
    }

    public ArrayList<Engine> getNext() {
        return next;
    }

    public void attachNext(Engine e) {
        next.add(e);
    }

    public Statistics getStatistics() {
        return stats;
    }

    protected void propagateRead(long addr, long val) {
        for(Engine e : next) {
            e.read(addr, val);
        }
    }

    protected void propagateWrite(long addr, long val) {
        for(Engine e : next) {
            e.write(addr, val);
        }
    }

    public void logReset() {
        stats.reset();
        for(Engine e : next) {
            e.logReset();
        }
        reset();
    }

    public String dumpContents() {
        return "";
    };
}
