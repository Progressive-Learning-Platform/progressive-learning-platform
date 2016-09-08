package edu.asu.plp.tool.prototype.view;

import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.Queue;
import java.util.function.Function;

import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.concurrent.Worker.State;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.web.WebEngine;
import javafx.scene.web.WebView;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import edu.asu.plp.tool.prototype.util.OnLoadListener;

public class UARTPanel extends BorderPane
{
	private static class Message
	{
		private String tagType;
		private String message;
		
		public Message(String tagType, String message)
		{
			this.tagType = tagType;
			this.message = message;
		}
	}
	
	private static final String TEXT_PANE_ID = "textPane";
	private static final String TEXT_PANE_CLASS = "scrollPane";
	
	private Element textPaneElement;
	private WebEngine webEngine;
	private Queue<Message> messageQueue;
	private Map<String, Function<String, byte[]>> valueDisplayOptions;
	
	public UARTPanel()
	{
		WebView view = new WebView();
		view.setContextMenuEnabled(false);
		webEngine = view.getEngine();
		
		messageQueue = new LinkedList<>();
		
		valueDisplayOptions = new LinkedHashMap<>();
		populateDisplayOptions();
		
		ObservableValue<State> property = webEngine.getLoadWorker().stateProperty();
		OnLoadListener.register(this::onLoad, property);
		
		String content = "<html><head></head><body " + styleString() + "></body></html>";
		webEngine.loadContent(content);
		
		HBox controlPane = createControlPane();
		
		this.setCenter(view);
		this.setBottom(controlPane);
	}
	
	private HBox createControlPane()
	{
		HBox hbox = new HBox();
		setAlignment(hbox, Pos.CENTER);
		hbox.setAlignment(Pos.CENTER_LEFT);
		hbox.setPadding(new Insets(10, 10, 10, 10));
		hbox.setSpacing(5);
		
		Label label = new Label("Send");
		label.setMinWidth(Label.USE_PREF_SIZE);
		hbox.getChildren().add(label);
		
		ComboBox<String> dropdown = createDisplayOptionsDropdown();
		dropdown.setMinWidth(ComboBox.USE_PREF_SIZE);
		hbox.getChildren().add(dropdown);
		
		TextField valueField = new TextField();
		valueField.setPrefWidth(Integer.MAX_VALUE);
		hbox.getChildren().add(valueField);
		
		Button send = new Button("Send");
		send.setMinWidth(Button.USE_PREF_SIZE);
		hbox.getChildren().add(send);
		
		Button clear = new Button("Clear");
		clear.setMinWidth(Button.USE_PREF_SIZE);
		clear.setOnAction((event) -> clear());
		hbox.getChildren().add(clear);
		
		return hbox;
	}
	
	private void populateDisplayOptions()
	{
		valueDisplayOptions.put("ASCII Values", null);
		valueDisplayOptions.put("1-Byte Value", null);
		valueDisplayOptions.put("Packed Int", null);
		valueDisplayOptions.put("Space-Delimited Bytes", null);
	}
	
	private ComboBox<String> createDisplayOptionsDropdown()
	{
		ObservableList<String> options = FXCollections.observableArrayList();
		options.addAll(valueDisplayOptions.keySet());
		
		ComboBox<String> dropdown = new ComboBox<>(options);
		dropdown.getSelectionModel().select(0);
		return dropdown;
	}
	
	private String styleString()
	{
		String style = "style=\"";
		style += "background-color: black; ";
		style += "color: white; ";
		style += "font-family: Arial, Helvetica, sans-serif;";
		style += "\"";
		return style;
	}
	
	private void onLoad()
	{
		Document dom = webEngine.getDocument();
		textPaneElement = dom.createElement("div");
		textPaneElement.setAttribute("id", TEXT_PANE_ID);
		textPaneElement.setAttribute("class", TEXT_PANE_CLASS);
		
		Node body = dom.getElementsByTagName("body").item(0);
		body.appendChild(textPaneElement);
		
		for (Message message : messageQueue)
		{
			output(message.tagType, message.message);
		}
		
		messageQueue = null;
	}
	
	private void output(String tagType, String message)
	{
		Document dom = webEngine.getDocument();
		
		if (dom == null)
		{
			Message target = new Message(tagType, message);
			messageQueue.add(target);
		}
		else
		{
			Element tag = dom.createElement(tagType);
			
			Element content = dom.createElement("code");
			content.setTextContent(message);
			
			tag.appendChild(content);
			textPaneElement.appendChild(tag);
		}
	}
	
	public void print(String message)
	{
		output("span", message);
	}
	
	public void clear()
	{
		textPaneElement.setTextContent("");
	}
}
