package junit.plp.core;

import static org.junit.Assert.*;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.util.List;

import org.junit.After;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import plptool.PLPAsmSource;

public class PLPASMSourceTest
{
	static File longFileObject;
	static File mainFileObject;
	PLPAsmSource sourceFile;
	
	@BeforeClass
	public static void onClassStartup()
	{
		mainFileObject = new File("autotests/junit/plp/core/main.asm");
		longFileObject = new File("autotests/junit/plp/core/long_file.asm");
	}
	
	@Before
	public void startUp()
	{
		if (mainFileObject.exists())
			sourceFile = new PLPAsmSource(null, mainFileObject.getPath(), 0);
	}
	
	@After
	public void tearDown()
	{
		sourceFile = null;
	}
	
	// Test PLPAsmSource.getAsmLine
	@Test
	public void linesMatchForMatch()
	{
		try
		{
			List<String> fileContentsList = Files.readAllLines(mainFileObject
					.toPath());
			
			// getAsmFile uses actual line numbers
			for (int index = 1; index <= fileContentsList.size(); index++)
			{
				assertEquals("File line " + index + "equals getAsmLine",
						fileContentsList.get(index - 1),
						sourceFile.getAsmLine(index));
			}
			
			// Since it uses the actual line numbers, zero should return null.
			// See documentation
			assertNull(sourceFile.getAsmLine(0));
			
			// Since it uses the actual line numbers, we have to go above one.
			// See documentation
			assertNull(sourceFile.getAsmLine(fileContentsList.size() + 1));
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}
	
	@Test
	public void testGetAsmString()
	{
		try
		{
			sourceFile.setAsmFilePath(mainFileObject.getPath(), true);
			assertEquals("After loading from file, ensure contents match",
					new String(Files.readAllBytes(mainFileObject.toPath())),
					sourceFile.getAsmString());
			
			sourceFile.setAsmFilePath(longFileObject.getPath(), true);
			assertEquals("After loading from file, ensure contents match",
					new String(Files.readAllBytes(longFileObject.toPath())),
					sourceFile.getAsmString());
			
			sourceFile.setAsmString("");
			assertEquals("Empty string works", "", sourceFile.getAsmString());
		}
		catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	// Test PLPAsmSource.getAsmFilePath
	@Test
	public void filePathsAreCorrect()
	{
		assertEquals("Original file path is correct?",
				mainFileObject.getPath(), sourceFile.getAsmFilePath());
		
		sourceFile.setAsmFilePath(longFileObject.getPath());
		assertEquals("New file path is correct?", longFileObject.getPath(),
				sourceFile.getAsmFilePath());
		
		assertEquals("File loads correctly in AsmSource", true,
				sourceFile.setAsmFilePath(mainFileObject.getPath(), true));
		
		assertEquals("File loads correctly in AsmSource", true,
				sourceFile.setAsmFilePath(longFileObject.getPath(), true));
	}
	
	@Test
	public void testContentsDuringChanges()
	{
		try
		{
			sourceFile.setAsmFilePath(longFileObject.getPath());
			assertEquals("New file path is correct?", longFileObject.getPath(),
					sourceFile.getAsmFilePath());
			
			assertEquals(
					"After setting path without notifing to reload from file, make sure contents are the same.",
					new String(Files.readAllBytes(mainFileObject.toPath())),
					sourceFile.getAsmString());
			
			sourceFile.loadFromFile();
			assertEquals("After loading from file, ensure contents match",
					new String(Files.readAllBytes(longFileObject.toPath())),
					sourceFile.getAsmString());
			
			sourceFile.setAsmFilePath(mainFileObject.getPath(), true);
			assertEquals("After loading from file, ensure contents match",
					new String(Files.readAllBytes(mainFileObject.toPath())),
					sourceFile.getAsmString());
			
			sourceFile.setAsmString("");
			assertEquals("Path is the same, even after string change",
					mainFileObject.getPath(), sourceFile.getAsmFilePath());
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}
}
