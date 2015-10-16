package junit.plpToolBox;

import static org.junit.Assert.assertEquals;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;

import org.junit.After;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import plptool.Constants;
import plptool.PLPToolbox;

public class PLPToolBoxSystemTesting
{
	private static String osName;
	private static String osArchitecture;
	private static String filePath;
	private static String newData;
	private static String oldData;
	private static String multiLineData;
	
	private static ByteArrayOutputStream errorOutStream = new ByteArrayOutputStream();
	private static ByteArrayOutputStream standardOutStream = new ByteArrayOutputStream();
	
	@BeforeClass
	public static void beforeClassSetUp()
	{
		osName = System.getProperty("os.name");
		osArchitecture = System.getProperty("os.arch");
		filePath = "autotests/junit/plpToolBox/testFile.txt";
		newData = "Am I in the clipboard?";
		oldData = "Hope I'm not here when it comes around";
		multiLineData = "hello, am i testing?\nWaffles are delicious.\n";
	}
	
	@Before
	public void startUp()
	{
		System.setOut(new PrintStream(standardOutStream));
		System.setErr(new PrintStream(errorOutStream));
	}
	
	@After
	public void tearDown() throws IOException
	{
		errorOutStream.reset();
		standardOutStream.reset();
		System.setOut(null);
		System.setErr(null);
	}
	
	// Get host OS ID
	@Test
	public void getOSIDTest()
	{
		String windowsName = "Windows";
		String linuxName = "Linux";
		String macOSXName = "OS X";
		
		String thirtyTwoBitAMDArch = "i386";
		String thirtyTwoBitIntelArch = "x86";
		String sixtyFourBitAMDArch = "amd64";
		String sixtyFourBitIntelArch = "x86_64";
		
		switch (PLPToolbox.getOS(true))
		{
			case Constants.PLP_OS_LINUX_32:
				assertEquals("Is Linux Name", linuxName, osName);
				assertEquals("Is 32 bit Linux", true,
						osArchitecture.equals(thirtyTwoBitIntelArch)
								|| osArchitecture.equals(thirtyTwoBitAMDArch));
				break;
			case Constants.PLP_OS_LINUX_64:
				assertEquals("Is Linux Name", linuxName, osName);
				assertEquals("Is 64 bit Linux", true,
						osArchitecture.equals(sixtyFourBitIntelArch)
								|| osArchitecture.equals(sixtyFourBitAMDArch));
				break;
			case Constants.PLP_OS_WIN_32:
				assertEquals("Is Windows Name", true,
						osName.startsWith(windowsName));
				assertEquals("Is 32 bit Windows", true,
						osArchitecture.equals(thirtyTwoBitIntelArch)
								|| osArchitecture.equals(thirtyTwoBitAMDArch));
				break;
			case Constants.PLP_OS_WIN_64:
				assertEquals("Is Windows Name", true,
						osName.startsWith(windowsName));
				assertEquals("Is 64 bit Windows", true,
						osArchitecture.equals(sixtyFourBitIntelArch)
								|| osArchitecture.equals(sixtyFourBitAMDArch));
				break;
			case Constants.PLP_OS_MACOSX:
				assertEquals("Is OS X Name", true, osName.contains(macOSXName));
				break;
			case Constants.PLP_OS_UNKNOWN:
				System.out.println("OS is unknown.");
				break;
			default:
				assertEquals("No known constant returned", false, true);
				break;
		}
		
		assertEquals("Test OS Printing", osName + " " + osArchitecture,
				standardOutStream.toString().trim());
		
		standardOutStream.reset();
		
		// Make sure nothing is printed
		PLPToolbox.getOS(false);
		assertEquals("", new String(), standardOutStream.toString().trim());
	}
	
	// Testing PLPToolBox.copy
	// Copy a string to the system clipboard
	@Test
	public void testClipboardActions()
	{
		String thisWillBeInTheClipboard = "Am I in the clipboard?";
		
		PLPToolbox.copy(thisWillBeInTheClipboard);
		
		java.awt.Toolkit toolKit = java.awt.Toolkit.getDefaultToolkit();
		Clipboard clipboard = (Clipboard) toolKit.getSystemClipboard();
		
		try
		{
			assertEquals("Can we retrieve set clipboard data",
					thisWillBeInTheClipboard,
					clipboard.getData(DataFlavor.stringFlavor));
		}
		catch (UnsupportedFlavorException e)
		{
			e.printStackTrace();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}
	
	// Write a new file filled with the provided data in string
	@Test
	public void writeFileTesting()
	{
		String pathWithoutFileName = "autotests/junit/plpToolBox/";
		String fileContents;
		
		File file = new File(filePath);
		
		try
		{
			createFileFromData(oldData);
			
			assertEquals("PLP write returns success when file does exist?",
					Constants.PLP_OK, PLPToolbox.writeFile(newData, filePath));
			assertEquals("Was file created", true, file.exists());
			List<String> lines = Files.readAllLines(file.toPath());
			
			StringBuilder builder = new StringBuilder();
			lines.forEach(line -> builder.append(line));
			fileContents = builder.toString();
			
			assertEquals("Are file contents written over?", newData,
					fileContents);
		}
		catch (FileNotFoundException e)
		{
			e.printStackTrace();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
		
		deleteFile();
		
		assertEquals("PLP write returns success when file does not exist?",
				Constants.PLP_OK, PLPToolbox.writeFile(newData, filePath));
		assertEquals("Was file created", true, file.exists());
		
		try
		{
			List<String> lines = Files.readAllLines(file.toPath());
			
			StringBuilder builder = new StringBuilder();
			lines.forEach(line -> builder.append(line));
			fileContents = builder.toString();
			
			assertEquals("Are file contents the same on new file?", newData,
					fileContents);
		}
		catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		deleteFile();
		
		assertEquals("PLP write fails when an actual file is not provided",
				Constants.PLP_IO_WRITE_ERROR,
				PLPToolbox.writeFile(newData, pathWithoutFileName));
	}
	
	// Read a file and return the contents as string.
	@Test
	public void readFileAsStringTest()
	{
		createFileFromData(multiLineData);
		assertEquals("PLPToolBox return multi line data as one string",
				multiLineData, PLPToolbox.readFileAsString(filePath));
		
		String featuresHex = "0xffb89a";
		createFileFromData(featuresHex);
		assertEquals("PLPToolBox return multi line data as one string",
				featuresHex, PLPToolbox.readFileAsString(filePath));
		
		String featuresDifferentNullTypes = "00000000 null \0 ooo";
		createFileFromData(featuresDifferentNullTypes);
		assertEquals("PLPToolBox return multi line data as one string",
				featuresDifferentNullTypes,
				PLPToolbox.readFileAsString(filePath));
		
		createFileFromData(oldData);
		assertEquals("PLPToolBox return single line data as one string",
				oldData, PLPToolbox.readFileAsString(filePath));
		
		deleteFile();
	}
	
	// Read a line of string from standard input
	@Test
	public void readLineTest()
	{
		
	}
	
	
	private void deleteFile()
	{
		File file = new File(filePath);
		
		if (file.exists())
			file.delete();
	}
	
	private void createFileFromData(String data)
	{
		File file = new File(filePath);
		
		PrintWriter writer;
		try
		{
			writer = new PrintWriter(file);
			writer.write(data);
			writer.close();
		}
		catch (FileNotFoundException e)
		{
			e.printStackTrace();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}
	
}
