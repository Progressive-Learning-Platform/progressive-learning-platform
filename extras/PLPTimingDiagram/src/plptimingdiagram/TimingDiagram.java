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

package plptimingdiagram;

import plptimingdiagram.signals.*;
import java.util.ArrayList;

/**
 *
 * @author wira
 */
public class TimingDiagram {

    private ArrayList<Signal> signals;

    public TimingDiagram() {
        this.signals = new ArrayList<Signal>();
    }

    public TimingDiagram(ArrayList<Signal> signals) {
        this.signals = signals;
    }

    public void addSignal(Signal signal) {
        signals.add(signal);
    }

    public Signal getSignal(int index) {
        return signals.get(index);
    }

    public int getNumberOfSignals() {
        return signals.size();
    }

    public Signal removeSignal(int index) {
        return signals.remove(index);
    }
}
