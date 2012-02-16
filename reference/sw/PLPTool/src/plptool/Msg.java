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
import javax.swing.text.html.HTMLEditorKit;
import javax.swing.text.html.HTMLDocument;
import java.awt.Font;
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
    private static JTextPane output = null;

    /**
     * Reference to the HTML document of an output pane
     */
    private static HTMLDocument doc = null;

    /**
     * Reference to the editor kit of an output pane
     */
    private static HTMLEditorKit kit = null;

    /**
     * Preformatted String buffer
     */
    private static String p = "";

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

    public static void setOutput(JTextPane newOutput) {
        output = newOutput;
        doc = (HTMLDocument) output.getDocument();
        kit = (HTMLEditorKit) output.getEditorKit();
        output.setFont(new Font("Monospaced", Font.PLAIN, 12));
    }

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
            
        try {
        
        if(objIdentifier != null)


            if(output == null)
                System.out.println("[ERROR]" + (errorCode == -1 ? "" : " #"+errorCode) + " " + objIdentifier.toString() + ": " + errStr);
            else {
                kit.insertHTML(doc, doc.getLength(), "<b><font color=red>[ERROR]</font></b> " +
                        (errorCode == -1 ? "" : " #"+errorCode) + " " + objIdentifier.toString() + ": " + errStr + "\n"
                        , 0, 0, null);
                output.setCaretPosition(doc.getLength());
            }
        else
            if(output == null)
                System.out.println("[ERROR]" + (errorCode == -1 ? "" : " #"+errorCode) + " " + errStr);
            else {
                kit.insertHTML(doc, doc.getLength(), "<b><font color=red>[ERROR]</font></b> " +
                        (errorCode == -1 ? "" : " #"+errorCode) + " " + errStr + "\n"
                        , 0, 0, null);
                output.setCaretPosition(doc.getLength());
            }

        } catch(Exception e) {
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

        try {

        if(objIdentifier != null)
            if(output == null)
                System.out.println("[WARNING] " + objIdentifier.toString() + ": " + warningStr);
            else {
                kit.insertHTML(doc, doc.getLength(), "<b><font color=red>[WARNING]</font></b> "
                        + objIdentifier.toString() + ": " + warningStr + "<br />", 0, 0, null);
                output.setCaretPosition(doc.getLength());
            }
        else
            if(output == null)
                System.out.println("[WARNING] " + warningStr);
            else {
                kit.insertHTML(doc, doc.getLength(), "<b><font color=red>[WARNING]</font></b> "
                        + warningStr + "<br />", 0, 0, null);
                output.setCaretPosition(doc.getLength());
            }
        } catch(Exception e) {
            
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

        try {

        if(objIdentifier != null)
            if(output == null)
                System.out.println(objIdentifier.toString() + ": " + infoStr);
            else {
                kit.insertHTML(doc, doc.getLength(),
                        "<font face=\"sans-serif\"><font color=gray>&bull;</font> " + objIdentifier.toString() + ": " + infoStr + "</font><br />", 0, 0, null);
                output.setCaretPosition(doc.getLength());
            }
        else
            if(output == null)
                System.out.println(infoStr);
            else {
                kit.insertHTML(doc, doc.getLength(),
                        "<font face=\"sans-serif\"><font color=gray>&bull;</font> " + infoStr + "</font><br />", 0, 0, null);
                output.setCaretPosition(doc.getLength());
            }

        } catch(Exception e) {
            
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
            try {

            if(objIdentifier != null) {
                System.out.println("[DEBUG] " + objIdentifier.toString() + ": " + debugStr);
                if(output != null) {
                    kit.insertHTML(doc, doc.getLength(), "<b><font color=gray>[DEBUG]</font></b> "
                        + objIdentifier.toString() + ": <font color=\"#444444\">" + debugStr + "</font><br />", 0, 0, null);
                    output.setCaretPosition(doc.getLength());
                }
            }
            else {
                System.out.println("[DEBUG] " + debugStr);
                if(output != null) {
                    kit.insertHTML(doc, doc.getLength(), "<b><font color=gray>[DEBUG]</font></b> "
                        + "<font color=\"#444444\">" + debugStr + "</font><br />", 0, 0, null);
                    output.setCaretPosition(doc.getLength());
                }
            }

            } catch (Exception e) {
                
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

        try {

        if(output == null)
            System.out.println(msgStr);
        else {
            kit.insertHTML(doc, doc.getLength(), "<font face=monospaced size=12pt>" +
                        msgStr.replace(" ", "&nbsp;") + "</font><br />", 0, 0, null);
            output.setCaretPosition(doc.getLength());
        }

        } catch(Exception e) {

        }
    }

    /**
     * Re-routable System.out.print(String)
     *
     * @param msgStr Message string
     */
    public static void m(String msgStr) {
        if(silent) return;

        try {

        if(output == null)
            System.out.print(msgStr);
        else {
            kit.insertHTML(doc, doc.getLength(), "<font face=monospaced size=12pt>" +
                        msgStr.replace(" ", "&nbsp;") + "</font>", 0, 0, null);
            output.setCaretPosition(doc.getLength());
        }

        } catch(Exception e) {

        }
    }

    /**
     * Print pre-formatted string (HTML's <pre> tag)
     *
     * @param printStr Preformatted string
     */
    public static void P(String printStr) {
        if(silent) return;

        try {

        if(output == null)
            System.out.println(printStr);
        else {
            kit.insertHTML(doc, doc.getLength(), "<pre><font face=monospaced size=10pt>" +
                        printStr + "</font></pre>", 0, 0, null);
            output.setCaretPosition(doc.getLength());
        }

        } catch(Exception e) {

        }
    }

    /**
     * Append string to the preformatted string buffer
     *
     * @param appendStr String to append
     */
    public static void p(String appendStr) {
        p += appendStr + "\n";
    }

    /**
     * Append string to the preformatted string buffer with no newline added
     * in the end
     *
     * @param appendStr String to append
     */
    public static void pn(String appendStr) {
        p += appendStr;
    }

    /**
     * Print out buffered preformatted string
     */
    public static void P() {
        if(silent) return;

        try {

        if(output == null)
            System.out.println(p);
        else {
            kit.insertHTML(doc, doc.getLength(), "<pre><font face=monospaced size=10pt>" +
                        p + "</font></pre>", 0, 0, null);
            output.setCaretPosition(doc.getLength());
        }
        p = "";

        } catch(Exception e) {
            p = "";
        }
    }

    /**
     * Debug marking.
     */
    public static void mark() {
        try {

        if(output == null)
            System.out.println("[DEBUG] " + markCounter + " We're here!");
        else {
            kit.insertHTML(doc, doc.getLength(), "<b><font color=gray>[DEBUG]</font></b> "
                        + ": <font color=darkgray>mark: " + markCounter + "</font><br />", 0, 0, null);
                    output.setCaretPosition(doc.getLength());
        }

        } catch(Exception e) {
            
        }

        markCounter++;
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

