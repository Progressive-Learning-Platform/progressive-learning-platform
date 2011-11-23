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
public class Bus extends Signal {
    public Bus() {
        super();
    }

    public void addEdge(double time, long signal) {
        super.edges.add(new BusEdge(time, signal));
    }

    public long getSignal(double time) {
        BusEdge tempEdge;

        for(int i = 0; i < super.edges.size(); i++) {
            tempEdge = (BusEdge) super.edges.get(i);
            if(tempEdge.getTime() == time)
                return tempEdge.getSignal();
        }

        return -1;
    }

    @Override
    public ArrayList<BusEdge> getEdgesWithinRange(double startTime, double endTime) {
        ArrayList<BusEdge> tempEdges = new ArrayList<BusEdge>();
        BusEdge tempEdge;

        for(int i = 0; i < super.edges.size(); i++) {
            tempEdge = (BusEdge) super.edges.get(i);
            if(tempEdge.getTime() <= endTime && tempEdge.getTime() >= startTime)
                tempEdges.add(tempEdge);
        }
        return tempEdges;
    }
}