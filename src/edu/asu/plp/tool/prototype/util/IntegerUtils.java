package edu.asu.plp.tool.prototype.util;

import static java.nio.ByteOrder.BIG_ENDIAN;

import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Map;
import java.util.function.Function;

public class IntegerUtils
{
	public static int smartParse(String string)
	{
		Map<String, Function<String, Integer>> prefixes = new HashMap<>();
		prefixes.put("0x", (s) -> Integer.parseInt(s.substring(2), 16));
		prefixes.put("0b", (s) -> Integer.parseInt(s.substring(2), 2));
		prefixes.put("0d", (s) -> Integer.parseInt(s.substring(2), 10));
		prefixes.put("0o", (s) -> Integer.parseInt(s.substring(2), 8));
		
		String prefix = (string.length() > 2) ? string.substring(0, 2) : null;
		Function<String, Integer> conversion = prefixes.get(prefix);
		if (conversion == null)
			conversion = (s) -> Integer.parseInt(s);
		
		return conversion.apply(string);
	}
	
	public static char[] toAsciiArray(int integer)
	{
		String asciiString = toAsciiString(integer);
		return asciiString.toCharArray();
	}
	
	public static String toAsciiString(int integer)
	{
		byte[] bytes = toByteArray(integer);
		return new String(bytes);
	}
	
	public static byte[] toByteArray(int integer)
	{
		ByteBuffer buffer = ByteBuffer.allocate(Integer.BYTES).order(BIG_ENDIAN);
		buffer.putInt(integer);
		return buffer.array();
	}
}
