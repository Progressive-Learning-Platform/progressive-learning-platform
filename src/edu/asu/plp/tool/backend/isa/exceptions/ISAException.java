package edu.asu.plp.tool.backend.isa.exceptions;

public class ISAException extends Exception
{
	public ISAException(String exception)
	{
		super(exception);
	}
	
	public ISAException()
	{
		super();
	}
	
	public ISAException(String message, Throwable cause, boolean enableSuppression,
			boolean writableStackTrace)
	{
		super(message, cause, enableSuppression, writableStackTrace);
	}
	
	public ISAException(String message, Throwable cause)
	{
		super(message, cause);
	}
	
	public ISAException(Throwable cause)
	{
		super(cause);
	}
}
