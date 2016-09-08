package edu.asu.plp.tool.backend.isa.exceptions;

public class AssemblerException extends ISAException
{
	public AssemblerException(String exception)
	{
		super(exception);
	}
	
	public AssemblerException()
	{
		super();
	}
	
	public AssemblerException(String message, Throwable cause, boolean enableSuppression,
			boolean writableStackTrace)
	{
		super(message, cause, enableSuppression, writableStackTrace);
	}
	
	public AssemblerException(String message, Throwable cause)
	{
		super(message, cause);
	}
	
	public AssemblerException(Throwable cause)
	{
		super(cause);
	}
}
