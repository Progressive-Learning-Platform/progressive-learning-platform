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

package plptool.gui;

/**
 * Project event. Some actions in PLPTool will pass an instance of this class
 * to the hook function of all loaded dynamic modules.
 *
 * @author wira
 */
public class ProjectEvent {

    public final static int         GENERIC                         = 0;
    public final static int         EDITOR_CHANGE                   = 1;
    public final static int         SIMULATE                        = 2;
    public final static int         DESIMULATE                      = 3;
    public final static int         ASSEMBLE                        = 4;

    public final static int         SINGLE_STEP                     = 5;
    public final static int         AGGREGATE_STEP                  = 6;


    /**
     * Event identifier
     */
    private int id;

    /**
     * Event timestamp
     */
    private long timestamp;

    /**
     * System timestamp when this event is constructed
     */
    private long systemTimestamp;

    /**
     * Other parameters of the event
     */
    private Object parameters;

    /**
     * The constructor of this class will specify the event's id and timestamp
     *
     * @param id Numeric identifier of the event
     * @param timestamp Timestamp of when the event occurred
     * @param parameters Other parameters of the event
     */
    public ProjectEvent(int id, long timestamp, Object parameters) {
        this.id = id;
        this.timestamp = timestamp;
        this.parameters = parameters;
        systemTimestamp = System.currentTimeMillis();
    }

    /**
     * The constructor of this class will specify the event's id and timestamp
     *
     * @param id Numeric identifier of the event
     * @param timestamp Timestamp of when the event occurred
     */
    public ProjectEvent(int id, long timestamp) {
        this.id = id;
        this.timestamp = timestamp;
        this.parameters = null;
        systemTimestamp = System.currentTimeMillis();
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
     * Get the user-defined timestamp when the event was constructed
     *
     * @return User-defined timestamp
     */
    public long getTimestamp() {
        return timestamp;
    }

    /**
     * Get system timestamp when the event was constructed
     *
     * @return System time in milliseconds when the event was constructed
     */
    public long getSystemTimestamp() {
        return systemTimestamp;
    }
}
