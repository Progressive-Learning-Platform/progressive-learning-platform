/*
    Copyright 2010 David Fritz, Brian Gordon, Wira Mulia

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

package plptool;

import javax.swing.JTextArea;

/**
 * PLPTool messaging utility class.
 *
 * @author wira
 */
public class Msg {

    /**
     * JTextArea Msg should print its output to.
     */
    public static JTextArea output = null;

    /**
     * Last object that was responsible for invoking an Error messsage.
     */
    public static Object lastPartyResponsible;

    /**
     * The error code of the last error.
     */
    public static int lastError = 0;

    /**
     * Mark counter. Debug purposes only.
     */
    public static int markCounter = 0;

    /**
     * Error counter.
     */
    public static int errorCounter = 0;

    /**
     * This function either prints out an error message to stdout or the
     * specified JTextArea in the output pointer.
     *
     * @param errStr Error string to print.
     * @param errorCode Error code.
     * @param objIdentifier A reference to the offending object.
     * @return The error code for further handling.
     */
    public static int E(String errStr, int errorCode, Object objIdentifier) {
        if(objIdentifier != null)
            if(output == null)
                System.out.println("[ERROR] #" + errorCode + " " + objIdentifier.toString() + ": " + errStr);
            else {
                output.append("[ERROR] #" + errorCode + " " + objIdentifier.toString() + ": " + errStr + "\n");
                output.setCaretPosition(output.getText().length() - 1);
            }
        else
            if(output == null)
                System.out.println("[ERROR] #" + errorCode + " " + errStr);
            else {
                output.append("[ERROR] #" + errorCode + " " + errStr + "\n");
                output.setCaretPosition(output.getText().length() - 1);
            }

        lastError = errorCode;
        lastPartyResponsible = objIdentifier;
        return errorCode;
    }

    /**
     * This function either prints out an warning message to stdout or the
     * specified JTextArea in the output pointer.
     *
     * @param warningStr Error string to print.
     * @param objIdentifier A reference to the offending object.
     */
    public static void W(String warningStr, Object objIdentifier) {
        if(objIdentifier != null)
            if(output == null)
                System.out.println("[WARNING] " + objIdentifier.toString() + ": " + warningStr);
            else {
                output.append("[WARNING] " + objIdentifier.toString() + ": " + warningStr + "\n");
                output.setCaretPosition(output.getText().length() - 1);
            }
        else
            if(output == null)
                System.out.println("[WARNING] " + warningStr);
            else {
                output.append("[WARNING] " + warningStr + "\n");
                output.setCaretPosition(output.getText().length() - 1);
            }
    }

    /**
     * Prints an informative message to the current output object.
     *
     * @param infoStr Information string to print out.
     * @param objIdentifier The object invoking this call.
     */
    public static void I(String infoStr, Object objIdentifier) {
        if(objIdentifier != null)
            if(output == null)
                System.out.println(objIdentifier.toString() + ": " + infoStr);
            else {
                output.append(objIdentifier.toString() + ": " + infoStr + "\n");
                output.setCaretPosition(output.getText().length() - 1);
            }
        else
            if(output == null)
                System.out.println(infoStr);
            else {
                output.append(infoStr + "\n");
                output.setCaretPosition(output.getText().length() - 1);
            }
    }

    /**
     * Prints out a debug message if the current debug level is lower or
     * equal to the requested level.
     *
     * @param debugStr Debug string to print out.
     * @param requestedDebugLevel Debug level requested.
     * @param objIdentifier The object invoking this call.
     */
    public static void D(String debugStr, int requestedDebugLevel, Object objIdentifier) {
        if(requestedDebugLevel <= Constants.debugLevel)
            if(objIdentifier != null)
                if(output == null)
                    System.out.println("[DEBUG] " + objIdentifier.toString() + ": " + debugStr);
                else {
                    output.append("[DEBUG] " + objIdentifier.toString() + ": " + debugStr + "\n");
                    output.setCaretPosition(output.getText().length() - 1);
                }
            else
                if(output == null)
                    System.out.println("[DEBUG] " + debugStr);
                else {
                    output.append("[DEBUG] " + debugStr + "\n");
                    output.setCaretPosition(output.getText().length() - 1);
                }
    }

    /**
     * Re-routable System.out.println(String)
     *
     * @param msgStr Message string
     */
    public static void M(String msgStr) {
        if(output == null)
            System.out.println(msgStr);
        else {
            output.append(msgStr + "\n");
            output.setCaretPosition(output.getText().length() - 1);
        }
    }

    /**
     * Re-routable System.out.print(String)
     *
     * @param msgStr Message string
     */
    public static void m(String msgStr) {
        if(output == null)
            System.out.print(msgStr);
        else {
            output.append(msgStr);
            output.setCaretPosition(output.getText().length() - 1);
        }
    }

    /**
     * Debug marking.
     */
    public static void mark() {
        if(output == null)
            System.out.println("[DEBUG] " + markCounter + " We're here!");
        else
            output.append("[DEBUG] " + markCounter + " We're here!" + "\n");
        markCounter++;
    }
}
