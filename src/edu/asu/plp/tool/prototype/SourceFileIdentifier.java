package edu.asu.plp.tool.prototype;

public class SourceFileIdentifier
{
	private String projectName;
	private String fileName;
	
	public SourceFileIdentifier(String projectName, String fileName)
	{
		super();
		this.projectName = projectName;
		this.fileName = fileName;
	}
	
	public String getID()
	{
		return projectName + "/" + fileName;
	}
	
	@Override
	public boolean equals(Object other)
	{
		if (other instanceof SourceFileIdentifier)
			return hashCode() == other.hashCode();
		else
			return false;
	}
	
	@Override
	public int hashCode()
	{
		return getID().hashCode();
	}
}
