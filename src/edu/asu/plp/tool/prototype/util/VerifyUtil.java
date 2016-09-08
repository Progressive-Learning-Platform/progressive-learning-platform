package edu.asu.plp.tool.prototype.util;

/**
 * Utility methods for checking verifying if input is valid. These methods only verify, they never return the input in
 * a
 * different form.
 * <p>
 * e.g. When checking if a string is an integer, it will never return an integer. Only a boolean saying whether or not
 * it is an integer.
 *
 * @author by Morgan on 3/8/2016.
 */
public class VerifyUtil
{
	/**
	 * Checks if provided input is an integer, returns true if is, false otherwise.
	 * <p>
	 * NOTE: Implemented using Try-Catch. Use this if you don't need the number back and it is not getting consistently
	 * called.
	 *
	 * @param input
	 *
	 * @return True if input is an integer, false otherwise.
	 */
	public static boolean simpleIntegerCheck( String input )
	{
		try
		{
			Integer.parseInt(input);
			return true;
		}
		catch ( NumberFormatException exception )
		{
			return false;
		}
	}

	/**
	 * Checks if provided Strings are integers
	 * <p>
	 * NOTE: Implemented using Try-Catch. Use this if you don't need the numbers back and it is not getting
	 * consistently called.
	 *
	 * @param input
	 * 		Array of Strings
	 *
	 * @return True if all strings are integers, false otherwise
	 */
	public static boolean simpleIntegerCheck( String... input )
	{
		try
		{
			for ( String value : input )
			{
				Integer.parseInt(value);
			}
			return true;
		}
		catch ( NumberFormatException exception )
		{
			return false;
		}
	}
}
