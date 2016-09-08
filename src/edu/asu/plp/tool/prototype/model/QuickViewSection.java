package edu.asu.plp.tool.prototype.model;

import java.util.ArrayList;
import java.util.List;

public class QuickViewSection
{
	private String title;
	private List<QuickViewEntry> entries;
	private String contentHeader;
	private String descriptionHeader;
	
	public QuickViewSection(String title, List<QuickViewEntry> entries)
	{
		super();
		this.title = title;
		this.entries = entries;
		setHeaders("Content", "Description");
	}
	
	public QuickViewSection(String title)
	{
		this(title, new ArrayList<>());
	}
	
	public void setHeaders(String contentHeader, String descriptionHeader)
	{
		this.contentHeader = contentHeader;
		this.descriptionHeader = descriptionHeader;
	}
	
	public void addEntry(QuickViewEntry entry)
	{
		entries.add(entry);
	}
	
	public void addEntry(String content, String description)
	{
		QuickViewEntry entry = new QuickViewEntry(content, description);
		entries.add(entry);
	}
	
	public String getTitle()
	{
		return title;
	}
	
	public List<QuickViewEntry> getEntries()
	{
		return entries;
	}
	
	public String getContentHeader()
	{
		return contentHeader;
	}
	
	public void setContentHeader(String contentHeader)
	{
		this.contentHeader = contentHeader;
	}
	
	public String getDescriptionHeader()
	{
		return descriptionHeader;
	}
	
	public void setDescriptionHeader(String descriptionHeader)
	{
		this.descriptionHeader = descriptionHeader;
	}
}
