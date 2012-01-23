/*
    Copyright 2011 David Fritz, Brian Gordon, Wira Mulia

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

package plptimingdiagram.signals;

/**
 *
 * @author wira
 */
public class LineEdge extends Edge {
    private double time;
    private int signal;

    public LineEdge(double time, int signal) {
        this.time = time;
        this.signal = signal;
    }

    public double getTime() {
        return time;
    }

    public int getSignal() {
        return signal;
    }

    public void setTimeAndSignal(double time, int signal) {
        this.time = time;
        this.signal = signal;
    }
}

