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
public class PLPMsg {

    /**
     * JTextArea PLPMsg should print its output to
     */
    public static JTextArea output = null;

    public static Object lastPartyResponsible;
    public static int lastError = 0;
    public static int markCounter = 0;

    // Error message
    public static int E(String errStr, int errorCode, Object objIdentifier) {
        if(objIdentifier != null)
            if(output == null)
                System.out.println("[E] #" + errorCode + " " + objIdentifier.toString() + ": " + errStr);
            else
                output.append("[E] #" + errorCode + " " + objIdentifier.toString() + ": " + errStr + "\n");
        else
            if(output == null)
                System.out.println("[E] #" + errorCode + " " + errStr);
            else
                output.append("[E] #" + errorCode + " " + errStr + "\n");

        lastError = errorCode;
        lastPartyResponsible = objIdentifier;
        return errorCode;
    }

    // Information message
    public static void I(String infoStr, Object objIdentifier) {
        if(objIdentifier != null)
            if(output == null)
                System.out.println("[I] " + objIdentifier.toString() + ": " + infoStr);
            else
                output.append("[I] " + objIdentifier.toString() + ": " + infoStr + "\n");
        else
            if(output == null)
                System.out.println("[I] " + infoStr);
            else
                output.append("[I] " + infoStr + "\n");
    }

    // Debug message
    public static void D(String debugStr, int requestedDebugLevel, Object objIdentifier) {
        if(requestedDebugLevel <= Constants.debugLevel)
            if(objIdentifier != null)
                if(output == null)
                    System.out.println("[D] " + objIdentifier.toString() + ": " + debugStr);
                else
                    output.append("[D] " + objIdentifier.toString() + ": " + debugStr + "\n");
            else
                if(output == null)
                    System.out.println("[D] " + debugStr);
                else
                    output.append("[D] " + debugStr + "\n");
    }

    // Standard out
    public static void M(String msgStr) {
        if(output == null)
            System.out.println(msgStr);
        else
            output.append(msgStr + "\n");
    }

    // Standard out no new-line
    public static void m(String msgStr) {
        if(output == null)
            System.out.print(msgStr);
        else
            output.append(msgStr);
    }

    // Mark
    public static void mark() {
        if(output == null)
            System.out.println("[D] " + markCounter + " We're here!");
        else
            output.append("[D] " + markCounter + " We're here!" + "\n");
        markCounter++;
    }
}
