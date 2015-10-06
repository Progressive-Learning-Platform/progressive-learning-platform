package junit;

import static org.junit.Assert.*;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.PrintStream;

import org.junit.After;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import plptool.PLPToolbox;

public class PLPToolboxTest
{
	private static String numInt;
	private static String numHexWithXSuffix;
	private static String numHexWithHSuffix;
	private static String numBinaryWithBSuffix;
	
	private static long numIntExpected;
	private static long numHexWithXSuffixExpected;
	private static long numHexWithHSuffixExpected;
	private static long numBinaryWithBSuffixExpected;
	
	private static ByteArrayOutputStream outStream = new ByteArrayOutputStream();
	
	@BeforeClass
	public static void beforeClassSetUp()
	{
		numInt = "12";
		numHexWithXSuffix = "0xF";
		numHexWithHSuffix = "0hF";
		numBinaryWithBSuffix = "0b101";
		
		numIntExpected = 12;
		numHexWithXSuffixExpected = 15;
		numHexWithHSuffixExpected = 15;
		numBinaryWithBSuffixExpected = 5;
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
	
	@Test
	public void parseNumHexOrBinaryIntoUnsignedTest()
	{
		assertEquals("parseNum 12 == 12", numIntExpected,
				PLPToolbox.parseNum(numInt));
		assertEquals("parseNum 0xF == 15", numHexWithXSuffixExpected,
				PLPToolbox.parseNum(numHexWithXSuffix));
		assertEquals("parseNum 0hF == 15", numHexWithHSuffixExpected,
				PLPToolbox.parseNum(numHexWithHSuffix));
		assertEquals("parseNum 0b101 == 5", numBinaryWithBSuffixExpected,
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
	
	@Test
	public void parseNumSilentHexOrBinaryIntoUnsignedTest()
	{
		
	}
	
}
