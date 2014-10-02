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

import plptool.Msg;

/**
 *
 * @author Wira
 */
public class Statistics {
    public int read_hits;
    public int read_accesses;
    public int write_hits;
    public int write_accesses;
    public int write_backs;

    public Statistics() {
        read_hits = 0;
        read_accesses = 0;
        write_hits = 0;
        write_accesses = 0;
        write_backs = 0;
    }

    public void reset() {
        read_hits = 0;
        read_accesses = 0;
        write_hits = 0;
        write_accesses = 0;
        write_backs = 0;
    }

    public String print() {
        String str = "";
        str += "read_hits:      " + read_hits + "\n";
        str += "read_accesses:  " + read_accesses + "\n";
        str += "write_hits:     " + write_hits + "\n";
        str += "write_accesses: " + write_accesses + "\n";
        str += "write_backs:    " + write_backs + "\n";
        
        return str;
    }
}

