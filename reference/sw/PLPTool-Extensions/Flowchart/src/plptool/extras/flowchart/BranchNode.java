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

package plptool.extras.flowchart;

/**
 *
 * @author wira
 */
public class BranchNode extends Node {
    private Node branch;
    private boolean branchOnTrue;

    public BranchNode(String label, long address, Node branch, boolean branchOnTrue) {
        super(label, address);
        this.branch = branch;
        this.branchOnTrue = branchOnTrue;
    }

    public void setBranchDestination(Node dest) {
        this.branch = dest;
    }

    public Node getBranchDestination() {
        return branch;
    }

    public boolean isBranchOnTrue() {
        return branchOnTrue;
    }

    @Override
    public String getType() {
        return "BranchNode";
    }
}
