package edu.asu.plp.tool.prototype.util;

import netscape.javascript.JSObject;

import java.util.ArrayList;
import java.util.List;

/**
 * @author Morgan Nesbitt
 */
public class JavascriptConversionUtil
{
	public static List<?> jsArrayToList(JSObject objectArray)
	{
		List<Object> javaList = new ArrayList<>();

		int objectArrayLength = (int) objectArray.getMember("length");

		for(int index = 0; index < objectArrayLength; index++)
		{
			Object arrayVariable = objectArray.getSlot(index);
			javaList.add(arrayVariable);
		}

		return javaList;
	}
}
