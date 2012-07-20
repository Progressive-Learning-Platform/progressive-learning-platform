/*
    Copyright 2012 PLP Contributors

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

package plptool.extras.execviz.program;

import java.util.ArrayList;

/**
 *
 * @author Wira
 */
public class BasicBlock {
    private BasicBlock next;
    private DecisionNode branch;
    private ArrayList<Long> addresses;
    public Statistics stats;

    public BasicBlock(long address) {
        addresses = new ArrayList<Long>();
        addresses.add(address);
        branch = null;
        next = null;
        stats = new Statistics();
    }

    public long getStartingAddress() {
        return addresses.get(0);
    }

    public ArrayList<Long> getAddresses() {
        return addresses;
    }

    public long getAddress(int index) {
        if(index < 0 || index >= size())
            return -1L;
        
        return addresses.get(index);
    }

    public void addAddress(long addr) {
        if(!hasAddress(addr))
            addresses.add(addr);
    }

    public int size() {
        return addresses.size();
    }

    public void setNext(BasicBlock next) {
        this.next = next;
    }

    public BasicBlock getNext() {
        return next;
    }
    
    public void setBranchingNode(DecisionNode n) {
        this.branch = n;
    }

    public DecisionNode getBranchingNode() {
        return branch;
    }

    public boolean hasBranch() {
        return branch != null;
    }

    public boolean hasAddress(long addr) {
        for(int i = 0; i < size(); i++)
            if(addresses.get(i) == addr)
                return true;
        return false;
    }
}
