package junit.plpToolBox;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertNotSame;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
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
		deleteFile();
		errorOutStream.reset();
		standardOutStream.reset();
		System.setOut(null);
		System.setErr(null);
		System.setIn(null);
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
			createFileFromStringData(oldData);
			
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
		createFileFromStringData(multiLineData);
		assertEquals("PLPToolBox return multi line data as one string",
				multiLineData, PLPToolbox.readFileAsString(filePath));
		
		String featuresHex = "0xffb89a";
		createFileFromStringData(featuresHex);
		assertEquals("PLPToolBox return multi line data as one string",
				featuresHex, PLPToolbox.readFileAsString(filePath));
		
		String featuresDifferentNullTypes = "00000000 null \0 ooo";
		createFileFromStringData(featuresDifferentNullTypes);
		assertEquals("PLPToolBox return multi line data as one string",
				featuresDifferentNullTypes,
				PLPToolbox.readFileAsString(filePath));
		
		createFileFromStringData(oldData);
		assertEquals("PLPToolBox return single line data as one string",
				oldData, PLPToolbox.readFileAsString(filePath));
	}
	
	// Read a line of string from standard input
	@Test
	public void readLineTest()
	{
		String customMultiLineMessage = "Lorem Ipsum is simply dummy text of the printing and typesetting industry.\n"
				+ "Lorem Ipsum has been the industry's standard dummy text ever since the 1500s\r.";
		String longOneLineMessage = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. "
				+ "Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an "
				+ "unknown printer took a galley of type and scrambled it to make a type specimen book. "
				+ "It has survived not only five centuries, but also the leap into electronic typesetting, "
				+ "remaining essentially unchanged. It was popularised in the 1960s with the release of "
				+ "Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing "
				+ "software like Aldus PageMaker including versions of Lorem Ipsum.";
		String hasNullHalfway = "00000000 null \0 00000000";
		String onlyNewLines = "\n\n\n\n\n\n\n";
		String newLinesWithOneOtherCharacter = "\n\n\n\n\n\n\n'";
		
		provideUserInput(hasNullHalfway);
		assertEquals(
				"Reads in string that contains different types of nulls throughout. ",
				hasNullHalfway, PLPToolbox.readLine());
		
		provideUserInput(longOneLineMessage);
		assertEquals("Reads in a long (one line) message succesfully.",
				longOneLineMessage, PLPToolbox.readLine());
		
		provideUserInput(customMultiLineMessage);
		assertEquals(
				"Reads in only a single line of a multi line message successfully.",
				"Lorem Ipsum is simply dummy text of the printing and typesetting industry.",
				PLPToolbox.readLine());
		
		provideUserInput(onlyNewLines);
		assertEquals("Reads in a string of newlines as an empty string", "",
				PLPToolbox.readLine());
		
		provideUserInput(newLinesWithOneOtherCharacter);
		assertEquals(
				"Reads in a string of newlines + one character at the end as an empty string",
				"", PLPToolbox.readLine());
	}
	
	// Read a file and return it as a byte array
	@Test
	public void readFileTest()
	{
		String customMultiLineMessage = "Lorem Ipsum is simply dummy text of the printing and typesetting industry.\n"
				+ "Lorem Ipsum has been the industry's standard dummy text ever since the 1500s\r.";
		String longOneLineMessage = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. "
				+ "Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an "
				+ "unknown printer took a galley of type and scrambled it to make a type specimen book. "
				+ "It has survived not only five centuries, but also the leap into electronic typesetting, "
				+ "remaining essentially unchanged. It was popularised in the 1960s with the release of "
				+ "Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing "
				+ "software like Aldus PageMaker including versions of Lorem Ipsum.";
		String hasNullHalfway = "00000000 null \0 00000000";
		String onlyNewLines = "\n\n\n\n\n\n\n";
		String newLinesWithOneOtherCharacter = "\n\n\n\n\n\n\n'";
		
		createFileFromStringData(hasNullHalfway);
		assertArrayEquals(
				"Reads in string that contains different types of nulls throughout. ",
				hasNullHalfway.getBytes(), PLPToolbox.readFile(filePath));
		
		createFileFromStringData(longOneLineMessage);
		assertArrayEquals("Reads in a long (one line) message succesfully.",
				longOneLineMessage.getBytes(), PLPToolbox.readFile(filePath));
		
		createFileFromStringData(customMultiLineMessage);
		assertArrayEquals(
				"Reads in only a single line of a multi line message successfully.",
				customMultiLineMessage.getBytes(),
				PLPToolbox.readFile(filePath));
		
		createFileFromStringData(onlyNewLines);
		assertArrayEquals("Reads in a string of newlines as an empty string",
				onlyNewLines.getBytes(), PLPToolbox.readFile(filePath));
		
		createFileFromStringData(newLinesWithOneOtherCharacter);
		assertArrayEquals(
				"Reads in a string of newlines + one character at the end as an empty string",
				newLinesWithOneOtherCharacter.getBytes(),
				PLPToolbox.readFile(filePath));
		
		createFileFromObjectData(customMultiLineMessage);
		assertArrayEquals(
				"Is one string serialized, the same when it is serialized a serparate time.",
				serialize(customMultiLineMessage),
				PLPToolbox.readFile(filePath));
		
		createFileFromObjectData(new PLPToolBoxSystemTesting());
		assertNotSame("Two instances of an object do not match up",
				serialize(new PLPToolBoxSystemTesting()),
				PLPToolbox.readFile(filePath));
	}
	
	// Check whether the host OS is GNU/Linux
	@Test
	public void isHostLinuxTest()
	{
		if (osName.equals("Linux"))
		{
			assertEquals("Confirms host is linux, success.", true,
					PLPToolbox.isHostLinux());
		}
	}
	
	/*
	 * Takes a string representing a character enclosed by single quotes and
	 * checks for escaped characters. If escaped character is detected, the
	 * function will return the actual character code. It will only strip the
	 * single quotes otherwise. For example, 'A' will be returned as A as long,
	 * and '\n' will be returned as the newline character.
	 */
	@Test
	public void parseEscapeCharacterTest()
	{
		try
		{
			String alphaToASCII = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
			for (int byteNumber = 0; byteNumber < alphaToASCII.length(); byteNumber++)
			{
				char character = alphaToASCII.charAt(byteNumber);
				int decimalValue = character;
				String characterToPass = "'" + character + "'";
				assertEquals(
						"Correctly converts printable keys to decimal value?",
						decimalValue,
						PLPToolbox.parseEscapeCharacter(characterToPass));
			}
			
			String newLine = "'\n'";
			assertEquals("Can parse newline character", '\n',
					PLPToolbox.parseEscapeCharacter(newLine));
			
			String quoteMark = "'\"'";
			assertEquals("Escaped quotation mark converts to decimal value?",
					'\"', PLPToolbox.parseEscapeCharacter(quoteMark));
			
			String tab = "'\t'";
			assertEquals("Escaped tab mark", '\t',
					PLPToolbox.parseEscapeCharacter(tab));
			assertNotSame("Fails on string non enclosed in single quotes",
					'\t', PLPToolbox.parseEscapeCharacter("\t"));
			
			String carriageReturn = "'\r'";
			assertEquals("Parses carriage return", '\r',
					PLPToolbox.parseEscapeCharacter(carriageReturn));
		}
		catch (Exception e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	/*
	 * By default File#delete fails for non-empty directories, it works like "rm".
     * We need something a little more brutual - this does the equivalent of "rm -r"
	 */
	@Test
	public void deleteRecursiveTest()
	{
		File directory1 = new File("autotests/junit/plpToolBox/recurse1");
		File directory2 = new File("autotests/junit/plpToolBox/recurse2");
		File directory3 = new File("autotests/junit/plpToolBox/recurse2/recurse3");
		
		File normalFile = new File("autotests/junit/plpToolBox/normalFile.txt");
		File normalFile2 = new File("autotests/junit/plpToolBox/recurse1/normalFile.txt");
		File normalFile3 = new File("autotests/junit/plpToolBox/recurse1/normalFile1.txt");
		File normalFile4 = new File("autotests/junit/plpToolBox/recurse1/normalFile2.txt");
		
		File normalFile5 = new File("autotests/junit/plpToolBox/recurse2/normalFile3.txt");
		File normalFile6 = new File("autotests/junit/plpToolBox/recurse2/normalFile4.txt");
		File normalFile7 = new File("autotests/junit/plpToolBox/recurse2/recurse3/normalFile5.txt");
		
		try
		{
			directory1.mkdir();
			directory3.mkdirs();
			
			createFileFromStringData(normalFile.getAbsolutePath(), oldData);
			createFileFromStringData(normalFile2.getAbsolutePath(), oldData);
			createFileFromStringData(normalFile3.getAbsolutePath(), oldData);
			createFileFromStringData(normalFile4.getAbsolutePath(), oldData);
			createFileFromStringData(normalFile5.getAbsolutePath(), oldData);
			createFileFromStringData(normalFile6.getAbsolutePath(), oldData);
			createFileFromStringData(normalFile7.getAbsolutePath(), oldData);
			
			assertEquals("Created firstDirectory exists", true, directory1.isDirectory() && directory1.exists());
			assertEquals("Created second directory exists", true, directory2.isDirectory() && directory2.exists());
			assertEquals("Created nested directory exists", true, directory3.isDirectory() && directory3.exists());
			
			assertEquals("Created normal file exists", true, normalFile.exists());
			assertEquals("Created normal file2 exists", true, normalFile2.exists());
			assertEquals("Created normal file3 exists", true, normalFile3.exists());
			assertEquals("Created normal file4 exists", true, normalFile4.exists());
			assertEquals("Created normal file5 exists", true, normalFile5.exists());
			assertEquals("Created normal file6 exists", true, normalFile6.exists());
			assertEquals("Created normal file7 (nested) exists", true, normalFile7.exists());
			
			assertEquals("normal file has been removed, and deleteRecursive returns this", true, PLPToolbox.deleteRecursive(normalFile));
			assertEquals("normal file has been removed", false, normalFile.exists());
			
			assertEquals("first directory and files deleted, and deleteRecursive returns this", true, PLPToolbox.deleteRecursive(directory1));
			assertEquals("first directory has been removed", false, directory1.exists());
			
			assertEquals("normal file2 from directory1 has been removed", false, normalFile2.exists());
			assertEquals("normal file3 from directory1 has been removed", false, normalFile3.exists());
			assertEquals("normal file4 from directory1 has been removed", false, normalFile4.exists());
			
			assertEquals("second/third directory and files deleted, and deleteRecursive returns this", true, PLPToolbox.deleteRecursive(directory2));
			assertEquals("seconds directory has been removed", false, directory2.exists());
			assertEquals("third directory (nested) has been removed", false, directory3.exists());
			
			assertEquals("normal file5 from directory2 has been removed", false, normalFile5.exists());
			assertEquals("normal file6 from directory2 has been removed", false, normalFile6.exists());
			assertEquals("normal file7 from directory3 has been removed", false, normalFile7.exists());
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	
	private void deleteFile()
	{
		File file = new File(filePath);
		
		if (file.exists())
			file.delete();
	}
	
	private void createFileFromObjectData(Object data)
	{
		File file = new File(filePath);
		
		try (ObjectOutputStream outputStream = new ObjectOutputStream(
				new FileOutputStream(file)))
		{
			outputStream.writeObject(data);
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
	
	private byte[] serialize(Object objectToByteArray)
	{
		try (ByteArrayOutputStream serialized = new ByteArrayOutputStream();
				ObjectOutputStream outputStream = new ObjectOutputStream(
						serialized);)
		{
			outputStream.writeObject(objectToByteArray);
			return serialized.toByteArray();
		}
		catch (FileNotFoundException e)
		{
			e.printStackTrace();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
		return null;
	}
	
	private void createFileFromStringData(String path, String data)
	{
		File file = new File(path);
		
		try (PrintWriter writer = new PrintWriter(file))
		{
			writer.write(data);
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
	
	private void createFileFromStringData(String data)
	{
		createFileFromStringData(filePath, data);
	}
	
	private void provideUserInput(String input)
	{
		System.setIn(new ByteArrayInputStream(input.getBytes()));
	}
	
}
