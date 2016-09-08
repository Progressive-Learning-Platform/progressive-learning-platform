package edu.asu.plp.tool.prototype.view;

import edu.asu.plp.tool.prototype.util.JavascriptConversionUtil;
import javafx.collections.FXCollections;
import org.apache.commons.lang3.StringEscapeUtils;

import javafx.beans.InvalidationListener;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.StringProperty;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableStringValue;
import javafx.scene.AccessibleRole;
import javafx.scene.Node;
import javafx.scene.input.Clipboard;
import javafx.scene.input.ClipboardContent;
import javafx.scene.input.DataFormat;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.web.WebView;
import netscape.javascript.JSObject;
import edu.asu.plp.tool.prototype.Controller;
import edu.asu.plp.tool.prototype.model.AceEditor;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;

/**
 * Accessible CodeEditor panel supporting syntax highlighting and data binding.
 * <p>
 * This panel accepts text input while it is focused, and can be navigated and manipulated
 * using the keyboard. Changes in the text of this editor can be observed using
 * {@link #addListener(ChangeListener)}.
 * <p>
 * Note that the accessibility property of this {@link Node} is bound to this editor's
 * text value. Any changes made to this node's accessible text property using
 * {@link #accessibleTextProperty()} and {@link ObjectProperty#set(Object)} will be
 * overwritten if the text of this editor is changed. As such, it is advised that the
 * accessible text property of this node not be altered outside of this class.
 * 
 * @author Moore, Zachary
 * @author Hawks, Elliott
 * @author Nesbitt, Morgan
 *
 */
public class CodeEditor extends BorderPane implements ObservableStringValue
{	
	private WebView webView;
	private AceEditor aceEditor;
	private StringProperty codeBodyProperty;
	private StringProperty acePageContentsProperty;
	
	public CodeEditor()
	{
		webView = new WebView();
		aceEditor = new AceEditor();
		
		codeBodyProperty = aceEditor.getBodyProperty();
		acePageContentsProperty = aceEditor.getPage();
		
		initializeEngineEvents();
		
		webView.getEngine().loadContent(aceEditor.getPage().get());
		
		// Add interface to access Java model from Javascript
		JSObject jsObject = (JSObject) webView.getEngine().executeScript("window");
		jsObject.setMember("javaContentModel", this);
		
		acePageContentsProperty.addListener((observable, old, newValue) -> 
		{
			System.out.println("Loaded:\n" + newValue);
			webView.getEngine().loadContent(newValue);
		});
		
		codeBodyProperty.addListener(
				(observable, old, newValue) -> System.out.println("CodeProperty changed"));
		
		webView.setContextMenuEnabled(false);
		//TODO create custom context menu (right click menu)
		
		// TODO: move this to a js file
		aceEditor.addCustomJavascriptRoutine(() -> "editor.on(\"change\", function() {"
				+ "javaContentModel.updateTextFromJavascript(editor.getValue());"
				+ "});");

		setOnKeyReleased(event -> {
			System.out.println("key released");
			getBreakPoints();
		});

		setCenter(webView);
		this.accessibleRoleProperty().set(AccessibleRole.TEXT_AREA);
	}

	public void updateTextFromJavascript(String text)
	{
		codeBodyProperty.set(text);
		System.out.println("Code updating");
	}
	
	public void println(String string)
	{
		System.out.println(string);
	}
	
	public void undoSelectedText()
	{
		webView.getEngine().executeScript("editor.undoManager.undo();");			
	
	}
	
	public void undoSelectionToClipboard()
	{
		//http://stackoverflow.com/questions/13929371/copy-and-paste-in-codemirror-js-embeded-in-javafx-application
		final Clipboard clipboard = Clipboard.getSystemClipboard();
	    final ClipboardContent content = new ClipboardContent();
	    undoSelectedText();
	    clipboard.setContent(content);
	}
	
	public String getSelectedText()
		{
			//http://chaschev.blogspot.com/2013/10/angularjs-in-desktop-app.html
			String selectedText = (String) webView.getEngine().executeScript(
		            "editor.getCopyText();");							
			return selectedText;
		}
		
		public void copySelectionToClipboard()
		{
			//http://stackoverflow.com/questions/13929371/copy-and-paste-in-codemirror-js-embeded-in-javafx-application
			final Clipboard clipboard = Clipboard.getSystemClipboard();
		    final ClipboardContent content = new ClipboardContent();
		    content.putString(getSelectedText());
		    clipboard.setContent(content);
		}
		
		public String cutSelectedText()
		{
			String selectedText = (String) webView.getEngine().executeScript(
		            "editor.getCopyText();");	
			webView.getEngine().executeScript("editor.remove();");
			return selectedText;
		}
		
		public void cutSelectionToClipboard()
		{			
			final Clipboard clipboard = Clipboard.getSystemClipboard();
		    final ClipboardContent content = new ClipboardContent();
		    content.putString(cutSelectedText());
		    clipboard.setContent(content);
		}
		
		public void pasteSelectionToClipboard()
		{	    
		    String content = (String) Clipboard.getSystemClipboard().getContent(DataFormat.PLAIN_TEXT);
            if (content != null) 
            {
            	webView.getEngine().executeScript("editor.onPaste('" + sanitizeForAce(content) + "');");
            }		
		}		
	
	
	public void setText(String text)
	{
		// TODO: update StringEscapeUtils and use escapeJavaScript() instead
		text = StringEscapeUtils.escapeJava(text);
		codeBodyProperty.set(text);
		System.out.println("Setting value");
		webView.getEngine().executeScript(
				"window.onload = function() {"
				+ "editor.on(\"change\", function(){});"
				+ "editor.setValue(\"" + text + "\","
						+ text.length() + ");"
				+ "editor.on(\"change\", function() {"
				+ "javaContentModel.updateTextFromJavascript(editor.getValue());"
				+ "});"
				+ "javaContentModel.println(\"Value Set\");"
				+ "};"
				);
	}

	public List<Integer> getBreakPoints()
	{
		List<Integer> breakpoints = null;

		JSObject  breakPointNumberArray = (JSObject) webView.getEngine().executeScript("getBreakPoints()");

		List<?> convertedArray = JavascriptConversionUtil.jsArrayToList(breakPointNumberArray);
		breakpoints = (List<Integer>) convertedArray;

		return breakpoints;
	}
	
	@Override
	public String get()
	{
		return codeBodyProperty.get();
	}
	
	@Override
	public String getValue()
	{
		return codeBodyProperty.getValue();
	}
	
	@Override
	public void addListener(ChangeListener<? super String> listener)
	{
		codeBodyProperty.addListener(listener);
	}
	
	@Override
	public void removeListener(ChangeListener<? super String> listener)
	{
		codeBodyProperty.removeListener(listener);
	}
	
	@Override
	public void addListener(InvalidationListener listener)
	{
		codeBodyProperty.addListener(listener);
	}
	
	@Override
	public void removeListener(InvalidationListener listener)
	{
		codeBodyProperty.removeListener(listener);
	}
	
	public void jumpToLine(int lineNumber)
	{
		String script = "editor.gotoLine(" + lineNumber + ", 0, true);";
		webView.getEngine().executeScript(script);
	}
	
	private void initializeEngineEvents()
	{
		webView.addEventFilter(KeyEvent.KEY_PRESSED, e -> {
			if( e.isShortcutDown() && e.getCode() == KeyCode.V) 
			{
				String content = (String) Clipboard.getSystemClipboard().getContent(DataFormat.PLAIN_TEXT);
	            if (content != null) 
	            {
	            	webView.getEngine().executeScript("editor.onPaste('" + sanitizeForAce(content) + "');");
	            }
            }
		});
	}
	
	private String sanitizeForAce(String content)
	{
		String intermediary = content;
		
		intermediary = intermediary.replace(System.getProperty("line.separator"), "\\n");
		intermediary = intermediary.replace("\n", "\\n");
		intermediary = intermediary.replace("\r", "\\n");
		intermediary = intermediary.replace("'", "\\'");
		
		return intermediary;
	}
	public String getselectedText()
	{
		String selection = (String) webView.getEngine().executeScript("window.getSelection().toString()");
		System.out.println(selection.length());
		return selection;
		
	}
}
