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

import javax.swing.JTextPane;
import javax.swing.text.StyledDocument;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import java.awt.Color;
import java.util.ArrayList;

/**
 * PLPTool messaging utility class.
 *
 * @author wira
 */
public class Msg {

    /**
     * JTextPane Msg should print its output to.
     */
    public static JTextPane output = null;

    /**
     * Aux outputs
     */
    public static ArrayList<JTextPane> outputs = new ArrayList<JTextPane>();

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
     * Silent mode.
     */
    public static boolean silent = false;

    /**
     * Suppress warnings.
     */
    public static boolean suppressWarnings = false;

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
                append("[ERROR] ", true, Color.RED);
                append("#" + errorCode + " " + objIdentifier.toString() + ": " + errStr + "\n");
            }
        else
            if(output == null)
                System.out.println("[ERROR] #" + errorCode + " " + errStr);
            else {
                append("[ERROR] ", true, Color.RED);
                append("#" + errorCode + " " + errStr + "\n");
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
        if(suppressWarnings) return;

        if(objIdentifier != null)
            if(output == null)
                System.out.println("[WARNING] " + objIdentifier.toString() + ": " + warningStr);
            else {
                append("[WARNING] ", true, Color.RED);
                append(objIdentifier.toString() + ": " + warningStr + "\n");
            }
        else
            if(output == null)
                System.out.println("[WARNING] " + warningStr);
            else {
                append("[WARNING] ", true, Color.RED);
                append(warningStr + "\n");
            }
    }

    /**
     * Print an informative message to the current output object.
     *
     * @param infoStr Information string to print out.
     * @param objIdentifier The object invoking this call.
     */
    public static void I(String infoStr, Object objIdentifier) {
        if(silent) return;

        if(objIdentifier != null)
            if(output == null)
                System.out.println(objIdentifier.toString() + ": " + infoStr);
            else {
                append(objIdentifier.toString() + ": " + infoStr + "\n");
            }
        else
            if(output == null)
                System.out.println(infoStr);
            else {
                append(infoStr + "\n");
            }
    }

    /**
     * Print out a debug message if the current debug level is lower or
     * equal to the requested level.
     *
     * @param debugStr Debug string to print out.
     * @param requestedDebugLevel Debug level requested.
     * @param objIdentifier The object invoking this call.
     */
    public static void D(String debugStr, int requestedDebugLevel, Object objIdentifier) {
        if(requestedDebugLevel <= Constants.debugLevel) {
            if(objIdentifier != null) {
                System.out.println("[DEBUG] " + objIdentifier.toString() + ": " + debugStr);
                if(output != null) {
                    append("[DEBUG] ", true, Color.GRAY);
                    append(objIdentifier.toString() + ": " + debugStr + "\n", false, Color.DARK_GRAY);
                }
            }
            else {
                System.out.println("[DEBUG] " + debugStr);
                if(output != null) {
                    append("[DEBUG] ", true, Color.GRAY);
                    append(debugStr + "\n", false, Color.DARK_GRAY);
                }
            }
        }
    }

    /**
     * Re-routable System.out.println(String)
     *
     * @param msgStr Message string
     */
    public static void M(String msgStr) {
        if(silent) return;

        if(output == null)
            System.out.println(msgStr);
        else {
            append(msgStr + "\n");
        }
    }

    /**
     * Re-routable System.out.print(String)
     *
     * @param msgStr Message string
     */
    public static void m(String msgStr) {
        if(silent) return;

        if(output == null)
            System.out.print(msgStr);
        else {
            append(msgStr);
        }
    }

    /**
     * Debug marking.
     */
    public static void mark() {
        if(output == null)
            System.out.println("[DEBUG] " + markCounter + " We're here!");
        else
            append("[DEBUG] " + markCounter + " We're here!" + "\n");
        markCounter++;
    }

    /**
     * Append to document with some style
     *
     * @param txt
     * @param bold
     * @param color
     */
    public static void append(String txt, boolean bold, Color color) {
        try {
        StyledDocument doc = output.getStyledDocument();

        SimpleAttributeSet attrib = new SimpleAttributeSet();
        StyleConstants.setBold(attrib, bold);
        StyleConstants.setForeground(attrib, color);

        doc.insertString(doc.getLength(), txt, attrib);

        for(int i = 0; i < outputs.size(); i++) {
            StyledDocument a_doc = outputs.get(i).getStyledDocument();
            a_doc.insertString(a_doc.getLength(), txt, attrib);
            outputs.get(i).setCaretPosition(a_doc.getLength() - 1);
        }

        output.setCaretPosition(doc.getLength() - 1);
        } catch(Exception e) {
            
        }
    }

    /**
     * Append regular text to the document
     *
     * @param txt
     */
    public static void append(String txt) {
        try {
        StyledDocument doc = output.getStyledDocument();

        SimpleAttributeSet attrib = new SimpleAttributeSet();
        StyleConstants.setBold(attrib, false);
        StyleConstants.setForeground(attrib, Color.BLACK);

        doc.insertString(doc.getLength(), txt, attrib);

        for(int i = 0; i < outputs.size(); i++) {
            StyledDocument a_doc = outputs.get(i).getStyledDocument();
            a_doc.insertString(a_doc.getLength(), txt, attrib);
            outputs.get(i).setCaretPosition(doc.getLength() - 1);
        }

        output.setCaretPosition(doc.getLength() - 1);
        } catch(Exception e) {

        }
    }

    /**
     * Print the stack trace if debug level is >= 10
     *
     * @param e
     */
    public static void printStackTrace(Exception e) {
        if(Constants.debugLevel >= 10)
            e.printStackTrace();
    }
}
