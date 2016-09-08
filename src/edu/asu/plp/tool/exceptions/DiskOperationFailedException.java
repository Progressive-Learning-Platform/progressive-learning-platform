package edu.asu.plp.tool.exceptions;

import java.io.IOException;

public class DiskOperationFailedException extends IOException
{
	public DiskOperationFailedException()
	{
		super();
	}

	public DiskOperationFailedException(String message, Throwable cause)
	{
		super(message, cause);
	}

	public DiskOperationFailedException(String message)
	{
		super(message);
	}

	public DiskOperationFailedException(Throwable cause)
	{
		super(cause);
	}
}
