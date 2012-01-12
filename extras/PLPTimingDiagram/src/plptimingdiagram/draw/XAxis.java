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

package plptimingdiagram.draw;

/**
 *
 * @author wira
 */
public class XAxis {
    private double startTime;
    private double endTime;
    private int intervals;

    public XAxis(double startTime, double endTime, int intervals) {
        this.startTime = startTime;
        this.endTime = endTime;
        this.intervals = intervals;
    }

    public void setDomain(double startTime, double endTime) {
        this.startTime = startTime;
        this.endTime = endTime;
    }

    public double getDomainStart() {
        return startTime;
    }

    public double getDomainEnd() {
        return endTime;
    }

    public int getIntervals() {
        return intervals;
    }

    public boolean nonPositiveDomain() {
        return (endTime - startTime) <= 0;
    }
}
