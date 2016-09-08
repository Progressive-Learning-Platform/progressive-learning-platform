package edu.asu.plp.tool.exceptions;

import edu.asu.plp.tool.prototype.model.Project;

public class ProjectAlreadyOpenException extends Exception
{
	private Project existingProject;
	
	public ProjectAlreadyOpenException(Project existingProject)
	{
		super();
		this.existingProject = existingProject;
	}
	
	public ProjectAlreadyOpenException(Project existingProject, String message)
	{
		super(message);
		this.existingProject = existingProject;
	}
	
	public Project getExistingProject()
	{
		return existingProject;
	}
}
