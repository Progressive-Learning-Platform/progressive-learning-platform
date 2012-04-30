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

import plptool.Msg;
import plptool.PLPToolbox;
import java.util.ArrayList;

/**
 *
 * @author wira
 */
public class NodeCollection {
    private Node head;
    private ArrayList<Node> nodes;

    public NodeCollection() {
        nodes = new ArrayList<Node>();
    }

    public void addNode(Node node) {
        if(head == null) head = node;
        nodes.add(node);
        Msg.D("Added " + node.getType() + ": " + node.getLabel() + ":" +
                PLPToolbox.format32Hex(node.getAddress()), 3, this);
    }

    public Node getNode(int index) {
        return nodes.get(index);
    }

    public Node getNode(long address) {
        int index = hasNodeWithAddress(address);
        if(index >= 0)
            return nodes.get(index);
        return null;
    }

    public Node getNode(String label) {
        int index = hasNodeWithLabel(label);
        if(index >= 0)
            return nodes.get(index);
        return null;
    }

    public int hasNodeWithLabel(String label) {
        for(int i = nodes.size()-1; i >= 0; i--) {
            if(nodes.get(i).getLabel().equals(label))
                return i;
        }
        return -1;
    }

    public int hasNodeWithAddress(long address) {
        for(int i = nodes.size()-1; i >= 0; i--) {
            if(nodes.get(i).getAddress() == address)
                return i;
        }
        return -1;
    }

    public Node getHead() {
        return head;
    }

    public int size() {
        return nodes.size();
    }

    @Override
    public String toString() {
        return "NodeCollection";
    }
}
