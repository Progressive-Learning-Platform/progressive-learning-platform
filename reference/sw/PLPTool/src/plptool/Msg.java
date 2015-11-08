/*
    Copyright 2010-2013 David Fritz, Brian Gordon, Wira Mulia

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

import java.awt.Font;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.util.HashSet;
import java.util.Set;

import javax.swing.JTextPane;
import javax.swing.text.html.HTMLDocument;
import javax.swing.text.html.HTMLEditorKit;

/**
 * PLPTool messaging utility class.
 *
 * @author wira
 */
public class Msg {
    /**
     * Last object that was responsible for invoking an Error messsage.
     */
    public static Object lastPartyResponsible;

    /**
     * The error code of the last error.
     */
    public static int lastError = 0;

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
    private static String preformattedStringBuffer = "";

    /**
     * Log output stream
     */
    private static Set<PrintStream> logStreams;

    /**
     * Log error stream
     */
    private static Set<PrintStream> errorStreams;
    
    static
    {
    	logStreams = new HashSet<>();
    	logStreams.add(System.out);
    	
    	errorStreams = new HashSet<>();
    	errorStreams.add(System.err);
    	
    	try
		{
			PrintStream persistantLog = new PrintStream(new FileOutputStream("log.txt", true));
	    	logStreams.add(persistantLog);
	    	errorStreams.add(persistantLog);
		}
		catch (FileNotFoundException e)
		{
			System.err.println("Unable to open log file log.txt");
		}
    }

    public static void setOutput(JTextPane newOutput) {
        output = newOutput;
        doc = (HTMLDocument) output.getDocument();
        kit = (HTMLEditorKit) output.getEditorKit();
        output.setFont(new Font("Monospaced", Font.PLAIN, 12));
    }

    /**
     * This function either prints out an error message to stderr or the
     * specified JTextArea in the output pointer. Error numbers equal or greater
     * to 1024 will always be printed to stderr.
     *
     * @param errStr Error string to print.
     * @param errorCode Error code.
     * @param objIdentifier A reference to the offending object.
     * @return The error code for further handling.
     */
    public static int error(String errStr, int errorCode, Object objIdentifier) {
        for (PrintStream stream : errorStreams)
        	logError(stream, errStr, errorCode, objIdentifier);

        lastError = errorCode;
        lastPartyResponsible = objIdentifier;
        return errorCode;
    }
    
    private static void logError(PrintStream stream, String errStr, int errorCode, Object objIdentifier) {
        try {   
            if(objIdentifier != null) {
                if(output == null || errorCode >= 1024)
                	stream.println("[ERROR]" + (errorCode == -1 ? "" : " #"+errorCode) + " " + objIdentifier.toString() + ": " + errStr.replaceAll("<[^>]*>", ""));
                if(output != null) {
                    kit.insertHTML(doc, doc.getLength(), "<b><font color=red>[ERROR]</font></b> " +
                            (errorCode == -1 ? "" : " #"+errorCode) + " " + objIdentifier.toString() + ": " + errStr + "\n"
                            , 0, 0, null);
                    output.setCaretPosition(doc.getLength());
                }
            } else {
                if(output == null || errorCode >= 1024)
                	stream.println("[ERROR]" + (errorCode == -1 ? "" : " #"+errorCode) + " " + errStr.replaceAll("<[^>]*>", ""));
                if(output != null) {
                    kit.insertHTML(doc, doc.getLength(), "<b><font color=red>[ERROR]</font></b> " +
                            (errorCode == -1 ? "" : " #"+errorCode) + " " + errStr + "\n"
                            , 0, 0, null);
                    output.setCaretPosition(doc.getLength());
                }
            }
            if(Config.cfgErrorDialog)
                PLPToolbox.showErrorDialog(null, "#" + errorCode + ": " + errStr);
        } catch(Exception e) {
        	// TODO: handle error and/or log it - do not suppress
        }
    }

    /**
     * This function either prints out an warning message to stdout or the
     * specified JTextArea in the output pointer.
     *
     * @param warningStr Error string to print.
     * @param objIdentifier A reference to the offending object.
     */
    public static void warning(String warningStr, Object objIdentifier) {
        if(suppressWarnings) return;

        for (PrintStream stream : logStreams)
        	logWarning(stream, warningStr, objIdentifier);
    }
    
    private static void logWarning(PrintStream stream, String warningStr, Object objIdentifier) {
        if(stream == null) return;

        try {

        if(objIdentifier != null)
            if(output == null)
            	stream.println("[WARNING] " + objIdentifier.toString() + ": " + warningStr.replaceAll("<[^>]*>", ""));
            else {
                kit.insertHTML(doc, doc.getLength(), "<b><font color=red>[WARNING]</font></b> "
                        + objIdentifier.toString() + ": " + warningStr + "<br />", 0, 0, null);
                output.setCaretPosition(doc.getLength());
            }
        else
            if(output == null)
            	stream.println("[WARNING] " + warningStr.replaceAll("<[^>]*>", ""));
            else {
                kit.insertHTML(doc, doc.getLength(), "<b><font color=red>[WARNING]</font></b> "
                        + warningStr + "<br />", 0, 0, null);
                output.setCaretPosition(doc.getLength());
            }
        } catch(Exception e) {
        	// TODO: handle error and/or log it - do not suppress
        }
    }

    /**
     * Print an informative message to the current output object.
     *
     * @param infoStr Information string to print out.
     * @param objIdentifier The object invoking this call.
     */
    public static void info(String infoStr, Object objIdentifier) {
        if(silent) return;

        for (PrintStream stream : logStreams)
        	logInfo(stream, infoStr, objIdentifier);
    }
    
    private static void logInfo(PrintStream stream, String infoStr, Object objIdentifier) {
        if(stream == null) return;

        try {

        if(objIdentifier != null)
            if(output == null)
            	stream.println(objIdentifier.toString() + ": " + infoStr.replaceAll("<[^>]*>", ""));
            else {
                kit.insertHTML(doc, doc.getLength(),
                        "<font face=\"sans-serif\"><font color=gray>&bull;</font> " + objIdentifier.toString() + ": " + infoStr + "</font><br />", 0, 0, null);
                output.setCaretPosition(doc.getLength());
            }
        else
            if(output == null)
            	stream.println(infoStr.replaceAll("<[^>]*>", ""));
            else {
                kit.insertHTML(doc, doc.getLength(),
                        "<font face=\"sans-serif\"><font color=gray>&bull;</font> " + infoStr + "</font><br />", 0, 0, null);
                output.setCaretPosition(doc.getLength());
            }

        } catch(Exception e) {
        	// TODO: handle error and/or log it - do not suppress
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
    public static void debug(String debugStr, int requestedDebugLevel, Object objIdentifier) {
        for (PrintStream stream : logStreams)
        	logDebug(stream, debugStr, requestedDebugLevel, objIdentifier);
    }
    
    private static void logDebug(PrintStream stream, String debugStr, int requestedDebugLevel, Object objIdentifier) {
        if(stream != null && requestedDebugLevel <= Constants.debugLevel) {
            try {

            if(objIdentifier != null) {
            	stream.println("[DEBUG] " + objIdentifier.toString() + ": " + debugStr);
                if(output != null) {
                    kit.insertHTML(doc, doc.getLength(), "<b><font color=gray>[DEBUG]</font></b> "
                        + objIdentifier.toString() + ": <font color=\"#444444\">" + debugStr + "</font><br />", 0, 0, null);
                    output.setCaretPosition(doc.getLength());
                }
            }
            else {
            	stream.println("[DEBUG] " + debugStr);
                if(output != null) {
                    kit.insertHTML(doc, doc.getLength(), "<b><font color=gray>[DEBUG]</font></b> "
                        + "<font color=\"#444444\">" + debugStr + "</font><br />", 0, 0, null);
                    output.setCaretPosition(doc.getLength());
                }
            }

            } catch (Exception e) {
            	// TODO: handle error and/or log it - do not suppress
            }
        }
    }

    /**
     * Re-routable System.out.println(String)
     *
     * @param msgStr Message string
     */
    public static void println(String msgStr) {
        try {
        	if(!silent)
            {
        		if(output == null)
        		{
        			String message = msgStr.replaceAll("<[^>]*>", "");
        			for (PrintStream stream : logStreams)
        				stream.println(message);
        		}
                else {
                    kit.insertHTML(doc, doc.getLength(), "<font face=monospaced size=12pt>" +
                                msgStr.replace(" ", "&nbsp;") + "</font><br />", 0, 0, null);
                    output.setCaretPosition(doc.getLength());
                }

             }
        } catch(Exception e) {
        	// TODO: handle error and/or log it - do not suppress
        }
    }

    /**
     * Re-routable System.out.print(String)
     *
     * @param msgStr Message string
     */
    public static void print(String msgStr) {
        try {
        	if(!silent)
            {
        		if(output == null)
        		{
        			String message = msgStr.replaceAll("<[^>]*>", "");
        			for (PrintStream stream : logStreams)
        				stream.print(message);
        		}
                else {
                	kit.insertHTML(doc, doc.getLength(), "<font face=monospaced size=12pt>" +
                            msgStr.replace(" ", "&nbsp;") + "</font>", 0, 0, null);
                	output.setCaretPosition(doc.getLength());
                }

             }
        } catch(Exception e) {
        	// TODO: handle error and/or log it - do not suppress
        }
    }

    /**
     * Print pre-formatted string (HTML's <pre> tag)
     *
     * @param message Preformatted string
     */
	public static void printPreformattedString(String message) {
        try {
        	if(!silent)
            {
        		if(output == null)
        		{
        			for (PrintStream stream : logStreams)
        				stream.println(message);
        		}
                else {
                	kit.insertHTML(doc, doc.getLength(), "<pre><font face=monospaced size=10pt>" +
                            message + "</font></pre>", 0, 0, null);
                	output.setCaretPosition(doc.getLength());
                }

             }
        } catch(Exception e) {
        	// TODO: handle error and/or log it - do not suppress
        }
    }

    /**
     * Append string to the preformatted string buffer
     *
     * @param appendStr String to append
     */
	public static void appendLine(String appendStr) {
        preformattedStringBuffer += appendStr + "\n";
    }

    /**
     * Append string to the preformatted string buffer with no newline added
     * in the end
     *
     * @param appendStr String to append
     */
    public static void append(String appendStr) {
        preformattedStringBuffer += appendStr;
    }

    /**
     * Print contents of {@link #preformattedStringBuffer}
     * <p>
     * The {@link #preformattedStringBuffer} can be built using {@link #append(String)} 
     * and {@link #appendLine(String)}
     */
    public static void printBuffer() {
    	// TODO: can combine with printPreformattedString()?
        try {
        	if(!silent)
            {
        		if(output == null)
        		{
        			for (PrintStream stream : logStreams)
        				stream.print(preformattedStringBuffer);
        		}
                else {
                	kit.insertHTML(doc, doc.getLength(), "<pre><font face=monospaced size=10pt>" +
	                        preformattedStringBuffer + "</font></pre>", 0, 0, null);
                	output.setCaretPosition(doc.getLength());
                }
        		
        		preformattedStringBuffer = "";
             }
        } catch(Exception e) {
        	preformattedStringBuffer = "";
        }
    }

    /**
     * Print the stack trace if debug level is >= 10
     *
     * @param e
     */
    public static void printStackTrace(Exception e) {
    	// TODO: integrate with Logger
        if(Constants.debugLevel >= 10)
            e.printStackTrace();
    }

    /**
     * Print stack trace if debug level is at least 2
     *
     * @param e Exception reference
     */
    public static void trace(Exception e) {
        if(Constants.debugLevel >= 2)
            e.printStackTrace();
    }
}

