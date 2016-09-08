package edu.asu.plp.tool.prototype;

public class ASMCreationDetails
{
	private String projectName;
	private String fileName;
	
	public ASMCreationDetails(String projectName, String fileName)
	{
		this.projectName = projectName;
		this.fileName = fileName;
	}
	
	public String getProjectName()
	{
		return projectName;
	}
	
	public void setProjectName(String projectName)
	{
		this.projectName = projectName;
	}
	
	public String getFileName()
	{
		return fileName;
	}
	
	public void setFileName(String fileName)
	{
		this.fileName = fileName;
	}
}
