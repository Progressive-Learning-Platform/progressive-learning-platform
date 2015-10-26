package junit.plp.core.projectdriver;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotSame;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.util.ArrayList;
import java.util.List;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import plptool.ArchRegistry;
import plptool.Constants;
import plptool.PLPAsmSource;
import plptool.gui.ProjectDriver;

/**
 * Save current project state to the file specified by plpfile.
 *
 * @return PLP_OK on successful save, error code otherwise
 */
public class ProjectDriverSaveTest
{
	ProjectDriver projectDriver;
	File sampleProjectFile;
	List<String> sampleProjectAsms;
	
	@Before
	public void startUp()
	{
		projectDriver = new ProjectDriver(0);
		sampleProjectFile = new File(
				"autotests/junit/plp/core/projectdriver/fsteptestSave.plp");
		
		sampleProjectAsms = new ArrayList<>();
		sampleProjectAsms.add("main.asm");
		sampleProjectAsms.add("libplp_uart.asm");
	}
	
	@After
	public void tearDown()
	{
		projectDriver = null;
	}
	
	@Test
	public void saveABlankProjectFromCreate()
	{
		assertEquals("Create returns a success", Constants.PLP_OK,
				projectDriver.create(ArchRegistry.ISA_PLPMIPS));
		
		/*
		 * This function provides a default empty source file and calls the
		 * architecture newProject() method.
		 * Does not provide a path, should not be able to save.
		 */
		assertNotSame("Can project driver save empty project",
				Constants.PLP_OK, projectDriver.save());
	}
	
	@Test
	public void saveTest()
	{
		cloneFileForEditing(new File(
				"autotests/junit/plp/core/projectdriver/fsteptest.plp"),
				sampleProjectFile);
		
		String separateAsmFilePath = "autotests/junit/plp/core/long_file.asm";
		
		projectDriver.open(sampleProjectFile.getPath(), false);
		assertEquals("Project driver returns success on save",
				Constants.PLP_OK, projectDriver.save());
		
		assertEquals("Import another file into the project, returns success",
				Constants.PLP_OK, projectDriver.importAsm(separateAsmFilePath));
		assertEquals("Project driver returns success on save",
				Constants.PLP_OK, projectDriver.save());
		
		projectDriver = null;
		projectDriver = new ProjectDriver(0);
		assertEquals("Opening modified project returns success?",
				Constants.PLP_OK,
				projectDriver.open(sampleProjectFile.getPath(), false));
		assertEquals("Amount of files in project are correct.",
				sampleProjectAsms.size() + 1, projectDriver.getAsms().size());
		
		String fileName = separateAsmFilePath.split("/")[separateAsmFilePath
				.split("/").length - 1];
		List<PLPAsmSource> asms = projectDriver.getAsms();
		for (PLPAsmSource asmFile : asms)
		{
			if (!sampleProjectAsms.contains(asmFile.getAsmFilePath()))
				assertEquals("New asm file has new relative path?", fileName,
						asmFile.getAsmFilePath());
		}
		
		if (sampleProjectFile.exists())
			sampleProjectFile.delete();
	}
	
	private void cloneFileForEditing(File originalFile, File editableFile)
	{
		if (editableFile.exists())
			editableFile.delete();
		
		try
		{
			Files.copy(originalFile.toPath(), editableFile.toPath(),
					StandardCopyOption.REPLACE_EXISTING);
		}
		catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
