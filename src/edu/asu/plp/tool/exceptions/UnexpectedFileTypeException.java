package edu.asu.plp.tool.exceptions;

import java.io.IOException;

public class UnexpectedFileTypeException extends IOException
{
	public UnexpectedFileTypeException()
	{
		super();
	}
	
	public UnexpectedFileTypeException(String message, Throwable cause)
	{
		super(message, cause);
	}
	
	public UnexpectedFileTypeException(String message)
	{
		super(message);
	}
	
	public UnexpectedFileTypeException(Throwable cause)
	{
		super(cause);
	}
}
