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

package plptool.mods;

import plptool.PLPSimBusModule;
import plptool.Msg;
import plptool.PLPToolbox;
import plptool.Constants;
import plptool.PLPSimCore;

import plptimingdiagram.TimingDiagram;
import plptimingdiagram.signals.Bus;
import plptimingdiagram.signals.BusEdge;

import javax.swing.SwingUtilities;

import java.util.ArrayList;

/**
 *
 * @author wira
 */
public class BusMonitor extends PLPSimBusModule {
        private ArrayList<Long> addresses;
        private ArrayList<Long> oldValues;
        private TimingDiagram tD;
        private PLPSimCore sim;

        public BusMonitor(PLPSimCore sim) {
            super(-1, -1, false);
            addresses = new ArrayList<Long>();
            oldValues = new ArrayList<Long>();
            tD = new TimingDiagram();
            this.sim = sim;
        }

        public int eval() {
            for(int i = 0; i < addresses.size(); i++) {
                Long value = (Long) sim.bus.read(addresses.get(i));
                if(value != null && oldValues.get(i) != value) {
                    tD.getSignal(i).addEdge(new BusEdge(sim.getInstrCount(), value));
                    oldValues.set(i, value);
                }
            }

            return Constants.PLP_OK;
        }

        public int gui_eval(final Object x) {

            return Constants.PLP_OK;
        }

        public void reset() {
            for(int i = 0; i < tD.getNumberOfSignals(); i++)
                tD.getSignal(i).clearEdges();
        }

        public void addAddressToMonitor(long addr) {
            for(int i = 0; i < addresses.size(); i++) {
                if(addresses.get(i) == addr)
                    addresses.remove(i);
            }

            addresses.add(addr);
            oldValues.add(-1L);
            Bus newSignal = new Bus();
            newSignal.setName(String.format("0x%08x", addr));
            tD.addSignal(newSignal);
        }

        public TimingDiagram getTimingDiagram() {
            return tD;
        }

        public void setTimingDiagram(TimingDiagram tD) {
            this.tD = tD;

            addresses = new ArrayList<Long>();
            oldValues = new ArrayList<Long>();

            for(int i = 0; i < tD.getNumberOfSignals(); i++) {
                addresses.add(Long.parseLong(tD.getSignal(i).getName().substring(2), 16));
                oldValues.add(-1L);
            }
        }

        public PLPSimCore getSimCore() {
            return sim;
        }

        @Override
        public void clear() {
            addresses.clear();
            for(int i = 0; i < tD.getNumberOfSignals(); i++)
                tD.removeSignal(0);
        }

        @Override
        public Object read(long addr) {
            return null;
        }

        @Override
        public int write(long addr, Object data, boolean isInstr) {
            return Constants.PLP_OK;
        }

        @Override
        public String introduce() {
            return "Bus Monitor";
        }

        @Override
        public String toString() {
            return "BusMonitor";
        }
}
