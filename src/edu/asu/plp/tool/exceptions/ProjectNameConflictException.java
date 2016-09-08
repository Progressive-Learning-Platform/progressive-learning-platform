package edu.asu.plp.tool.exceptions;

import edu.asu.plp.tool.prototype.model.Project;

public class ProjectNameConflictException extends Exception
{
	private Project conflictingProject;
	
	public ProjectNameConflictException(Project conflictingProject)
	{
		super();
		this.conflictingProject = conflictingProject;
	}
	
	public ProjectNameConflictException(Project conflictingProject, String message)
	{
		super(message);
		this.conflictingProject = conflictingProject;
	}
	
	public Project getConflictingProject()
	{
		return conflictingProject;
	}
	
	@Override
	public String getMessage()
	{
		String message = super.getMessage();
		if (message == null || message.isEmpty())
			return "A project with the name " + conflictingProject.getName()
					+ " already exists";
		else
			return message;
	}
}
