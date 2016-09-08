package edu.asu.plp.tool.prototype.model;

import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

public class QuickViewEntry
{
	private StringProperty content;
	private StringProperty description;
	
	public QuickViewEntry(String content, String description)
	{
		super();
		this.content = new SimpleStringProperty(content);
		this.description = new SimpleStringProperty(description);
	}
	
	public String getContent()
	{
		return content.get();
	}
	
	public String getDescription()
	{
		return description.get();
	}
}
