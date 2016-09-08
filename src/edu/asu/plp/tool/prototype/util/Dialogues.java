package edu.asu.plp.tool.prototype.util;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Optional;

import javafx.scene.control.Alert;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.ButtonType;
import javafx.scene.control.TextArea;

/**
 * Support class to spawn basic dialogues with a similar look and feel. This class
 * currently supports information and error dialogues, and can accept a message or an
 * exception.
 * <p>
 * Dialogues spawned by this class will use uniform styling, including graphics, headers,
 * and text type.
 * 
 * @author Moore, Zachary
 *
 */
public class Dialogues
{
	/**
	 * Spawns an information dialogue with the specified message.
	 * 
	 * @param message
	 *            The message to display. This will appear in the context field of the
	 *            dialogue.
	 */
	public static void showInfoDialogue(String message)
	{
		String title = "Information Dialog";
		showInfoDialogue(title, message);
	}
	
	/**
	 * Spawns an information dialogue with the specified title and message.
	 * 
	 * @param title The title of the dialogue. This will appear in the title field of the
	 *            dialogue.
	 * @param message
	 *            The message to display. This will appear in the context field of the
	 *            dialogue.
	 */
	public static void showInfoDialogue(String title, String message)
	{
		Alert alert = new Alert(AlertType.INFORMATION);
		alert.setTitle(title);
		alert.setHeaderText(null);
		alert.setContentText(message);
		
		alert.showAndWait();
	}

	public static void showWarningDialogue(String title, String message)
	{
		Alert alert = new Alert(AlertType.WARNING);
		alert.setTitle(title);
		alert.setHeaderText(null);
		alert.setContentText(message);
		
		alert.showAndWait();
	}

	public static void showActionFailedDialogue(String message)
	{
		showWarningDialogue("Action Failed", message);
	}
	
	/**
	 * Spawns a confirmation dialogue with the specified message, and returns the result
	 * (OK or cancel)
	 * 
	 * @param message
	 *            The message to display. This will appear in the context field of the
	 *            dialogue.
	 */
	public static Optional<ButtonType> showConfirmationDialogue(String message)
	{
		Alert alert = new Alert(AlertType.CONFIRMATION);
		alert.setTitle("Confirmation Dialog");
		alert.setHeaderText(null);
		alert.setContentText(message);
		
		return alert.showAndWait();
	}
	
	/**
	 * Spawns an error dialogue with the default error header, and the dialogue content as
	 * the exception message, if present. See
	 * {@link #showAlertDialogue(Exception, String)} for more details.
	 * 
	 * @param exception
	 *            The exception to display
	 */
	public static void showAlertDialogue(Exception exception)
	{
		showAlertDialogue(exception, "An error has occurred!");
	}
	
	/**
	 * Spawns an error dialogue detailing the given exception.
	 * <p>
	 * The given message will be used as the dialogue's header, and the exception's stack
	 * trace will appear in the hidden "more information" dropdown.
	 * <p>
	 * If the exception has a message, it will be displayed in the dialogue's content
	 * field, prefaced by "Cause:"
	 * 
	 * @param exception
	 *            The exception to display
	 * @param message
	 *            A message to describe the context of the dialogue, usually why the
	 *            dialogue is appearing (e.g. "An error has occurred!")
	 */
	public static void showAlertDialogue(Exception exception, String message)
	{
		String context = exception.getMessage();
		boolean valid = (context != null && !context.isEmpty());
		context = (valid) ? "Cause: " + context : null;
		
		Alert alert = new Alert(AlertType.ERROR);
		alert.setTitle("Exception Dialog");
		alert.setHeaderText(message);
		alert.setContentText(context);
		alert.setGraphic(null);
		
		String exceptionText = getStackTraceAsString(exception);
		TextArea textArea = new TextArea(exceptionText);
		textArea.setEditable(false);
		textArea.setWrapText(false);
		
		alert.getDialogPane().setExpandableContent(textArea);
		alert.showAndWait();
	}
	
	/**
	 * Prints the stack trace (specified by {@link Exception#printStackTrace()}) of the
	 * given exception to a new string, with newline characters in tact.
	 * 
	 * @param exception The stack trace of this variable will be returned as a String
	 * @return A string containing the stack trace of the specified exception
	 */
	private static String getStackTraceAsString(Exception exception)
	{
		StringWriter stringWriter = new StringWriter();
		PrintWriter printWriter = new PrintWriter(stringWriter);
		exception.printStackTrace(printWriter);
		
		return stringWriter.toString();
	}
}
