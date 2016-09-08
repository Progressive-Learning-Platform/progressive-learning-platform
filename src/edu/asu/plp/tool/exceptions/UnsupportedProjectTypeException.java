package edu.asu.plp.tool.exceptions;

public class UnsupportedProjectTypeException extends Exception
{
	private String type;
	
	public UnsupportedProjectTypeException(String type)
	{
		super();
		this.type = type;
	}
	
	public UnsupportedProjectTypeException(String type, String message)
	{
		super(message);
		this.type = type;
	}
	
	public String getType()
	{
		return type;
	}
}
