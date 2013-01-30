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
    public DefaultCache(Engine prev, Engine...engines) {
        super(prev, engines);
    }

    public void reset() {
        
    }
    
    public int read(long addr, long val) {
        return 0;
    }

    public int write(long addr, long val) {
        return 0;
    }
}
