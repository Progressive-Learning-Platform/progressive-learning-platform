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
abstract public class Signal {
    private Object meta;
    protected ArrayList edges;
    protected Object[] range;
    private String name;

    public Signal(ArrayList edges) {
        this.edges = edges;
    }

    public Signal() {
        this.edges = new ArrayList();
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public Object getEdge(int index) {
        return edges.get(index);
    }

    public void addEdge(Object edge) {
        edges.add(edge);
    }

    public int getNumberOfEdges() {
        return edges.size();
    }

    abstract public ArrayList getEdgesWithinRange(double startTime, double endTime);
}
