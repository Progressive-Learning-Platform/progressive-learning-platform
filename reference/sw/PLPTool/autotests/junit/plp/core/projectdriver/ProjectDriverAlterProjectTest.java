package junit.plp.core.projectdriver;

import static org.junit.Assert.assertEquals;

import java.util.ArrayList;
import java.util.List;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import plptool.Constants;
import plptool.PLPAsmSource;
import plptool.gui.ProjectDriver;

public class ProjectDriverAlterProjectTest
{
	// TODO provide empty project, project with many files, etc
	// projects with files with same names.
	ProjectDriver projectDriver;
	String sampleProjectPath;
	List<String> sampleProjectAsms;
	
	@Before
	public void startUp()
	{
		projectDriver = new ProjectDriver(0);
		sampleProjectPath = "autotests/junit/plp/core/projectdriver/fsteptest.plp";
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
	public void addNewFileWithSameNameAsAnExistingFile()
	{
		String separateAsmFilePath = "autotests/junit/plp/core/main.asm";
	}
	
	@Test
	public void addFilesToProject()
	{
		String separateAsmFilePath = "autotests/junit/plp/core/long_file.asm";
		
		projectDriver.open(sampleProjectPath, false);
		assertEquals("Import asm returns success", Constants.PLP_OK,
				projectDriver.importAsm(separateAsmFilePath));
		assertEquals("Asm count reflects newly added asms.",
				sampleProjectAsms.size() + 1, projectDriver.getAsms().size());
		assertEquals("Project shows it's modified after adding file", true,
				projectDriver.isModified());
		assertEquals(
				"Project shows binary files are not dirty after adding file",
				false, projectDriver.isDirty());
		
		String fileName = separateAsmFilePath.split("/")[separateAsmFilePath
				.split("/").length - 1];
		List<PLPAsmSource> asms = projectDriver.getAsms();
		for (PLPAsmSource asmFile : asms)
		{
			if (!sampleProjectAsms.contains(asmFile.getAsmFilePath()))
				assertEquals("New asm file has new relative path?", fileName,
						asmFile.getAsmFilePath());
		}
	}
}
