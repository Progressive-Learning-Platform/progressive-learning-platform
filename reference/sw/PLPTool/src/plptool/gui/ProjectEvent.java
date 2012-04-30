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

    /* IDE CONTROL EVENTS */
    public final static int         EDITOR_CHANGE                   = 1;
    public final static int         SIMULATE                        = 2;
    public final static int         DESIMULATE                      = 3;
    public final static int         ASSEMBLE                        = 4;
    public final static int         WINDOW_MOVE                     = 5;
    public final static int         WINDOW_RESIZE                   = 6;
    public final static int         PROJECT_SAVE                    = 7;
    public final static int         PROJECT_OPEN                    = 8;
    public final static int         PROJECT_OPEN_ENTRY              = 9;
    public final static int         EDITOR_INSERT                   = 10;
    public final static int         EDITOR_REMOVE                   = 11;
    public final static int         EDITOR_TEXT_SET                 = 12;
    public final static int         EXIT                            = 13;
    public final static int         NEW_PROJECT                     = 14;
    public final static int         POST_ASSEMBLE                   = 15;

    /* OPEN ROUTINES */
    public final static int         OPEN_ASM_ENTRY                  = 128;

    /* SIM EVENTS */
    public final static int         SINGLE_STEP                     = 256;
    public final static int         AGGREGATE_STEP                  = 257;
    public final static int         RUN_START                       = 258;
    public final static int         RUN_END                         = 259;
    public final static int         SIM_WINDOW_VISIBILITY_TRUE      = 260;
    public final static int         SIM_WINDOW_VISIBILITY_FALSE     = 261;
    public final static int         BREAKPOINT_SET                  = 262;
    public final static int         BREAKPOINT_REMOVED              = 263;
    public final static int         BREAKPOINT_CLEARED              = 264;
    public final static int         SIM_POST_INIT                   = 265;
    public final static int         SIM_POST_UNINIT                 = 266;
    public final static int         SIM_SPEED_CHANGED               = 267;
    public final static int         SIM_STEPSIZE_CHANGED            = 268;

    /* PROJECT CONTROL EVENTS */
    public final static int         NEW_MAIN_SOURCE                 = 1024;
    public final static int         OPENASM_CHANGE                  = 1025;
    public final static int         NEW_SOURCE                      = 1026;
    public final static int         REMOVE_SOURCE                   = 1027;

    /* MISCELLANEOUS */
    public static final int         THIRDPARTY_LICENSE              = 1028;
    public static final int         CONFIG_SAVE                     = 1029;

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
     * This constructor specifies the ID, timestamp, and parameters of the
     * event
     *
     * @param id Numeric identifier of the event
     * @param timestamp User-defined timestamp of when the event occurred
     * @param parameters Other parameters of the event
     */
    public ProjectEvent(int id, long timestamp, Object parameters) {
        this.id = id;
        this.timestamp = timestamp;
        this.parameters = parameters;
        systemTimestamp = System.currentTimeMillis();
    }

    /**
     * This constructor specifies only ID and timestamp of the event. The
     * parameters field is set to null.
     *
     * @param id Numeric identifier of the event
     * @param timestamp User-defined timestamp of when the event occurred
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

    /**
     * Set a new system time stamp for this event
     *
     * @param newSystemTimeStamp New timestamp
     */
    public void setSystemTimestamp(long newSystemTimeStamp) {
        this.systemTimestamp = newSystemTimeStamp;
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
     * Set new event parameters
     *
     * @param params Generic object reference of the parameters
     */
    public void setParameters(Object params) {
        this.parameters = params;
    }
}
