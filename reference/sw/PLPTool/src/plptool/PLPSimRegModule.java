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

package plptool;

/**
 * PLP Register module.
 *
 * @author wira
 */
public class PLPSimRegModule {
    private long data;
    private long i_data;

    public PLPSimRegModule(long i_data) {
        this.i_data = i_data;
    }

    public int write(long data) {
        this.i_data = data;

        return Constants.PLP_OK;
    }

    public long input() {
        return i_data;
    }

    public long eval() {
        return data;
    }

    public void clock() {
        data = i_data;
    }

    public void reset(long i_data) {
        data = -1;
        this.i_data = i_data;
    }
}
