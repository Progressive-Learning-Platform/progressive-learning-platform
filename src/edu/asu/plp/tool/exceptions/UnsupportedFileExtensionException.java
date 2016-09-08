package edu.asu.plp.tool.exceptions;

import java.io.IOException;

public class UnsupportedFileExtensionException extends IOException
{
	public UnsupportedFileExtensionException()
	{
		super();
	}
	
	public UnsupportedFileExtensionException(String message)
	{
		super(message);
	}
}
