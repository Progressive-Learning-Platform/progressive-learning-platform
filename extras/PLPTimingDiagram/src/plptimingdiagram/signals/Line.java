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

import java.util.ArrayList;

/**
 *
 * @author wira
 */
public class Line extends Signal {
    public Line() {
        super();
    }

    public void addEdge(double time, boolean signal) {
        super.edges.add(new LineEdge(time, signal));
    }

    public int getSignal(double time) {
        LineEdge tempEdge;

        for(int i = 0; i < super.edges.size(); i++) {
            tempEdge = (LineEdge) super.edges.get(i);
            if(tempEdge.getTime() == time)
                return tempEdge.getSignal() ? 1 : 0;
        }

        return -1;
    }

    @Override
    public synchronized ArrayList<LineEdge> getEdgesWithinRange(double startTime, double endTime) {
        ArrayList<LineEdge> tempEdges = new ArrayList<LineEdge>();
        LineEdge tempEdge;
        for(int i = 0; i < super.edges.size(); i++) {
            tempEdge = (LineEdge) super.edges.get(i);
            if(tempEdge.getTime() < endTime && tempEdge.getTime() >= startTime) {
                tempEdges.add(tempEdge);
            }
        }
        return tempEdges;
    }
}
