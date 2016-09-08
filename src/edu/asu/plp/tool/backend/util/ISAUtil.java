package edu.asu.plp.tool.backend.util;

import edu.asu.plp.tool.backend.isa.exceptions.AssemblyException;

public class ISAUtil
{
	public static final long MASK_32 = 0xFFFFFFFF;
	public static final long MASK_16 = 0xFFFF;
	
	public static long sanitize32bits(String number) throws AssemblyException
	{
		if (number.startsWith("0x") || number.startsWith("0h"))
		{
			return Long.parseLong(number.substring(2), 16) & MASK_32;
		}
		else if (number.startsWith("0b"))
		{
			return Long.parseLong(number.substring(2), 2) & MASK_32;
		}
		else if (number.startsWith("'") && number.endsWith("'"))
		{
			return parseEscapeCharacter(number);
		}
		
		return Long.parseLong(number) & 0xFFFFFFFF;
	}
	
	public static long sanitize16bits(String number) throws AssemblyException
	{
		if (number.startsWith("0x") || number.startsWith("0h"))
		{
			return Long.parseLong(number.substring(2), 16) & MASK_16;
		}
		else if (number.startsWith("0b"))
		{
			return Long.parseLong(number.substring(2), 2) & MASK_16;
		}
		else if (number.startsWith("'") && number.endsWith("'"))
		{
			return parseEscapeCharacter(number);
		}
		
		return Long.parseLong(number) & 0xFFFF;
	}
	
	/**
	 * This method assumes that the given string starts and ends with single quotes (')
	 * 
	 * @param string
	 * @return
	 */
	public static long parseEscapeCharacter(String string) throws AssemblyException
	{
		if (string.length() == 3)
			return string.charAt(1);
		else if (string.length() == 4)
		{
			if (string.charAt(1) == '\\')
			{
				switch (string.charAt(2))
				{
					case 'n':
						return '\n';
					case 'r':
						return '\r';
					case 't':
						return '\t';
					case '\\':
						return '\\';
					case '\"':
						return '\"';
					case '\'':
						return '\'';
					case '0':
						return '\0';
					default:
						throw new AssemblyException("Invalid escape character");
				}
			}
			else
				throw new AssemblyException("Invalid character format");
		}
		else
			throw new AssemblyException("Invalid character format");
	}
}
