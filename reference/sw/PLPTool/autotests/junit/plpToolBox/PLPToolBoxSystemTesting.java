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
	
	private static ByteArrayOutputStream errorOutStream = new ByteArrayOutputStream();
	private static ByteArrayOutputStream standardOutStream = new ByteArrayOutputStream();
	
	@BeforeClass
	public static void beforeClassSetUp()
	{
		osName = System.getProperty("os.name");
		osArchitecture = System.getProperty("os.arch");
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
	
	@Test
	public void writeFileTesting()
	{
		String dataToBeWritten = "Am I in the clipboard?";
		String oldData = "Hope I'm not here when it comes around";
		String filePath = "autotests/junit/plpToolBox/testFileWrite.txt";
		String pathWithoutFileName = "autotests/junit/plpToolBox/";
		String fileContents;
		
		File file = new File(filePath);
		
		if (file.exists())
		{
			PrintWriter writer;
			try
			{
				writer = new PrintWriter(file);
				writer.write(oldData);
				writer.close();
				
				assertEquals("PLP write returns success when file does exist?",
						Constants.PLP_OK,
						PLPToolbox.writeFile(dataToBeWritten, filePath));
				assertEquals("Was file created", true, file.exists());
				List<String> lines = Files.readAllLines(file.toPath());
				
				StringBuilder builder = new StringBuilder();
				lines.forEach(line -> builder.append(line));
				fileContents = builder.toString();
				
				assertEquals("Are file contents written over?",
						dataToBeWritten, fileContents);
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
		
		if (file.exists())
			file.delete();
		
		assertEquals("PLP write returns success when file does not exist?",
				Constants.PLP_OK,
				PLPToolbox.writeFile(dataToBeWritten, filePath));
		assertEquals("Was file created", true, file.exists());
		
		try
		{
			List<String> lines = Files.readAllLines(file.toPath());
			
			StringBuilder builder = new StringBuilder();
			lines.forEach(line -> builder.append(line));
			fileContents = builder.toString();
			
			assertEquals("Are file contents the same on new file?",
					dataToBeWritten, fileContents);
		}
		catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		if (file.exists())
			file.delete();
		
		assertEquals("PLP write fails when an actual file is not provided",
				Constants.PLP_IO_WRITE_ERROR,
				PLPToolbox.writeFile(dataToBeWritten, pathWithoutFileName));
	}
	
}
