package junit;

import static org.junit.Assert.*;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.PrintStream;

import org.junit.After;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import plptool.PLPAsm;
import plptool.PLPToolbox;

public class PLPToolboxTest
{
	private static String numInt;
	private static String numHexWithXSuffix;
	private static String numHexWithHSuffix;
	private static String numBinaryWithBSuffix;
	
	private static ByteArrayOutputStream outStream = new ByteArrayOutputStream();
	
	@BeforeClass
	public static void beforeClassSetUp()
	{
		numInt = "12";
		numHexWithXSuffix = "0xF";
		numHexWithHSuffix = "0hF";
		numBinaryWithBSuffix = "0b101";
	}
	
	@Before
	public void startUp()
	{
		System.setErr(new PrintStream(outStream));
	}
	
	@After
	public void tearDown() throws IOException
	{
		outStream.reset();
		System.setErr(null);
	}
	
	// parseNum
	@Test
	public void parseNumHexOrBinaryIntoUnsignedTest()
	{
		long numIntExpected = 12;
		long numHexWithXSuffixExpected = 15;
		long numHexWithHSuffixExpected = 15;
		long numBinaryWithBSuffixExpected = 5;
		
		assertEquals("parseNum " + numInt + " == " + numIntExpected,
				numIntExpected, PLPToolbox.parseNum(numInt));
		assertEquals("parseNum " + numHexWithXSuffix + " == "
				+ numHexWithXSuffixExpected, numHexWithXSuffixExpected,
				PLPToolbox.parseNum(numHexWithXSuffix));
		assertEquals("parseNum " + numHexWithHSuffix + " == "
				+ numHexWithHSuffixExpected, numHexWithHSuffixExpected,
				PLPToolbox.parseNum(numHexWithHSuffix));
		assertEquals("parseNum " + numBinaryWithBSuffix + " == "
				+ numBinaryWithBSuffixExpected, numBinaryWithBSuffixExpected,
				PLPToolbox.parseNum(numBinaryWithBSuffix));
		
		assertEquals("parseNum 0b000000000 == 0", 0,
				PLPToolbox.parseNum("0b000000000"));
		
		assertEquals("Parsing 0xf = 15", 15, PLPToolbox.parseNum("0xf"));
		
		assertEquals("parseNum -5 == -5", -5, PLPToolbox.parseNum("-5"));
	}
	
	@Test
	public void parseNumHexOrBinaryIntoUnsignedOutputTest() throws IOException
	{
		assertEquals("parseNum returns -1 on 0Xf", -1,
				PLPToolbox.parseNum("0Xf"));
		
		assertEquals(
				"parseNum stderr is [ERROR] Number error: '0Xf' is not a valid number",
				"[ERROR] Number error: '0Xf' is not a valid number".trim(),
				outStream.toString().trim());
		
		outStream.reset();
		
		assertEquals("parseNum returns -1 on 0HF", -1,
				PLPToolbox.parseNum("0Hf"));
		
		assertEquals(
				"parseNum stderr is [ERROR] Number error: '0Hf' is not a valid number",
				"[ERROR] Number error: '0Hf' is not a valid number".trim(),
				outStream.toString().trim());
		
	}
	
	// parseNumSilent
	@Test
	public void parseNumSilentHexOrBinaryIntoUnsignedTest()
	{
		parseNumHexOrBinaryIntoUnsignedTest();
	}
	
	// parseNumInt
	@Test
	public void parseNumIntHexOrBinaryIntoUnsignedTest()
	{
		int numIntExpected = 12;
		int numHexWithXSuffixExpected = 15;
		int numHexWithHSuffixExpected = 15;
		int numBinaryWithBSuffixExpected = 5;
		
		assertEquals("parseNum " + numInt + " == " + numIntExpected,
				numIntExpected, PLPToolbox.parseNum(numInt));
		assertEquals("parseNum " + numHexWithXSuffix + " == "
				+ numHexWithXSuffixExpected, numHexWithXSuffixExpected,
				PLPToolbox.parseNum(numHexWithXSuffix));
		assertEquals("parseNum " + numHexWithHSuffix + " == "
				+ numHexWithHSuffixExpected, numHexWithHSuffixExpected,
				PLPToolbox.parseNum(numHexWithHSuffix));
		assertEquals("parseNum " + numBinaryWithBSuffix + " == "
				+ numBinaryWithBSuffixExpected, numBinaryWithBSuffixExpected,
				PLPToolbox.parseNum(numBinaryWithBSuffix));
		
		assertEquals("parseNum 0b000000000 == 0", 0,
				PLPToolbox.parseNum("0b000000000"));
		
		assertEquals("Parsing 0xf = 15", 15, PLPToolbox.parseNum("0xf"));
		
		assertEquals("parseNum -5 == -5", -5, PLPToolbox.parseNum("-5"));
	}
	
	// tryResolveLabel
	@Test
	public void tryResolveLabelResolveOrParseAddress()
	{
		String label = "";
		PLPAsm asm = null;
		// fail("Not implemented");
	}
	
	// format32Hex
	// Function says format a long as 8 digit hex in String prefixed with
	// '0x'
	@Test
	public void format32Hex8DigitsWithPrefix0X()
	{
		long intMaxLength = Integer.MAX_VALUE;
		long longMaxLength = Long.MAX_VALUE;
		
		// 0x is length 2 + 8 digit hex
		assertEquals("Formatted", 10, PLPToolbox.format32Hex(intMaxLength)
				.length());
		
		assertEquals("Formatted", 10, PLPToolbox.format32Hex(longMaxLength)
				.length());
	}
	
}
