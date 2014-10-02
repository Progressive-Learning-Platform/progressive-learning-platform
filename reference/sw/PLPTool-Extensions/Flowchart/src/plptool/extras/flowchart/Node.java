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
public class Node {
    private String label;
    private String code;
    private long address;
    protected Node next;

    public Node(String label, long address) {
        this.label = label;
        this.address = address;
        code = "";
    }

    public String getLabel() {
        return label;
    }

    public long getAddress() {
        return address;
    }

    public void setNext(Node next) {
        this.next = next;
    }

    public Node getNext() {
        return next;
    }

    public void append(String code) {
        this.code += code + "\n";
    }

    public String getCode() {
        return code;
    }

    public String getType() {
        return "Node";
    }
}
