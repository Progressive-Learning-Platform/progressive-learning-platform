/*
    Copyright 2012-2014 PLP Contributors

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

package plptool.dmf;

import plptool.*;
import java.util.ArrayList;

/**
 * This class handles callback registration in PLPTool. Callbacks are
 * registered by passing an implementation of the Callback interface to the
 * register method along with the callback number.
 *
 * @author Wira
 */
public class CallbackRegistry {
    public static final int EXIT                        = 0;
    public static final int START                       = 1;
    public static final int COMMAND                     = 2;
    public static final int EVENT                       = 3;

    public static final int PROJECT_NEW                 = 4;
    public static final int PROJECT_SAVE                = 5;
    public static final int PROJECT_OPEN                = 6;
    public static final int PROJECT_OPEN_SUCCESSFUL     = 7;
    public static final int PROJECT_OPEN_ENTRY          = 8;
    public static final int PROJECT_OPENASM_CHANGE      = 9;
    public static final int PROJECT_NEW_ASM             = 10;
    public static final int PROJECT_REMOVE_ASM          = 11;

    public static final int LOAD_CONFIG_LINE            = 12;
    public static final int SAVE_CONFIG                 = 13;

    public static final int EVENT_SIMULATE              = 14;
    public static final int EVENT_DESIMULATE            = 15;
    public static final int EVENT_SIM_INIT              = 16;
    public static final int EVENT_SIM_POST_INIT         = 17;
    public static final int EVENT_SIM_POST_UNINIT       = 18;
    public static final int EVENT_ASSEMBLE              = 19;
    public static final int EVENT_ASSEMBLE_INIT         = 40;
    public static final int EVENT_POST_ASSEMBLE         = 20;
    public static final int EVENT_PROGRAM               = 21;
    public static final int EVENT_PROGRAM_INIT          = 41;
    public static final int EVENT_HEADLESS_START        = 42;

    public static final int SIM_STEP_AGGREGATE          = 22;
    public static final int SIM_POST_STEP_AGGREGATE     = 23;
    public static final int SIM_RUN_START               = 24;
    public static final int SIM_RUN_STOP                = 25;
    public static final int SIM_STEP                    = 26;
    public static final int SIM_POST_STEP               = 27;
    public static final int SIM_RESET                   = 28;

    public static final int BUS_READ                    = 29;
    public static final int BUS_POST_READ               = 30;
    public static final int BUS_WRITE                   = 31;
    public static final int BUS_EVAL                    = 32;
    public static final int BUS_ADD                     = 33;
    public static final int BUS_REMOVE                  = 34;
    public static final int BUS_GUI_EVAL                = 35;
    
    public static final int EDITOR_TEXT_SET             = 36;
    public static final int GUI_UPDATE                  = 37;
    public static final int GUI_VIEW_REFRESH            = 38;
    public static final int CRITICAL_ERROR              = 39;
    public static final int OPTIONS_UPDATE              = 43;

    public static final int CALLBACKS                   = 44;

    public static boolean INITIALIZED                   = false;

    @SuppressWarnings("unchecked")
    private static final ArrayList<Callback>[] callbacks = new ArrayList[CALLBACKS];

    /**
     * Runtime setup for the registry
     *
     * @param args
     * @return
     */
    public static String[] setup(String[] args) {
        String[] ret = args;

        for(int i = 0; i < CALLBACKS; i++)
            callbacks[i] = new ArrayList<Callback>();

        INITIALIZED = true;

        return ret;
    }

    /**
     * Perform callback. Should be triggered by PLPTool ONLY. Do NOT call this
     * method from a module.
     *
     * @param callbackNum Callback number to perform
     * @param param Parameter to the callback implementation
     * @return True if one of the methods registered to this callback return
     * true, false otherwise
     */
    public static boolean callback(int callbackNum, Object param) {
        boolean ret = false;
        for(int i = 0; i < callbacks[callbackNum].size(); i++) {
            Callback c = callbacks[callbackNum].get(i);
            Msg.D("callback[" + callbackNum + "]: " +
                    c.getClass().getCanonicalName(), 4, null);
            ret = c.callback(callbackNum, param) || ret;
        }
        return ret;
    }

    /**
     * Register a callback interface implementation to the specified callback
     * number.
     *
     * @param callbackNum Callback number to register for
     * @param callback Reference to an instance of the callback interface
     * implementation
     * @return False if the callback number is invalid, true otherwise
     */
    public static boolean register(Callback callback, int callbackNum) {
        if(callbackNum < 0 || callbackNum >= CALLBACKS)
            return false;

        callbacks[callbackNum].add(callback);
        return true;
    }

    /**
     * Register a callback interface implementation to multiple callback numbers
     *
     * @param callbackNumbers Callback numbers to register for
     * @param callback Reference to an instance of the callback interface
     * implementation
     * @return False if any of the specified numbers is invalid, true otherwise
     */
    public static boolean register(Callback callback, int...callbackNumbers) {
        boolean ret = true;
        for(int i = 0; i < callbackNumbers.length; i ++)
            ret = register(callback, callbackNumbers[i]) && ret;
        return ret;
    }

    /**
     * Unregister ONE instance of the callback (lowest index)
     *
     * @param callback Callback class to unregister
     * @param callbackNum Callback number
     * @return
     */
    public static boolean unregister(Callback callback, int callbackNum) {
        if(callbackNum < 0 || callbackNum >= CALLBACKS)
            return false;

        return callbacks[callbackNum].remove(callback);
    }

    /**
     * Get all callbacks for a callback event as an Object array
     *
     * @param callbackNum Callback number
     * @return Array of callbacks as objects
     */
    public static Object[] getCallbacks(int callbackNum) {
        Object[] ret = callbacks[callbackNum].toArray();
        return ret;
    }
}
