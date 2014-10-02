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

package plptool.gui;

/**
 *
 * @author wira
 */
public class BusEvent {
    public final static int         READ                         = 0;
    public final static int         WRITE                        = 1;
    public final static int         READ_DATA                    = 2;
    public final static int         READ_INSTR                   = 3;

    /**
     * Event identifier
     */
    private int id;

    /**
     * Other parameters of the event
     */
    private Object parameters;

    /**
     * This constructor specifies the ID and parameters of the bus event
     *
     * @param id Numeric identifier of the event
     * @param parameters Other parameters of the event
     */
    public BusEvent(int id, Object parameters) {
        this.id = id;
        this.parameters = parameters;
    }

    /**
     * Get the event numeric identifier
     *
     * @return Event identifier
     */
    public int getIdentifier() {
        return id;
    }

    /**
     * Get the parameters object for the event
     *
     * @return Reference to the parameter object
     */
    public Object getParameters() {
        return parameters;
    }

    /**
     * Return true if this is a write event
     *
     * @return True if this is a bus write event
     */
    public boolean isWrite() {
        return id == WRITE;
    }

    /**
     * Return true if this is a read event
     *
     * @return True if this is a bus read event
     */
    public boolean isRead() {
        return id == READ;
    }
}
