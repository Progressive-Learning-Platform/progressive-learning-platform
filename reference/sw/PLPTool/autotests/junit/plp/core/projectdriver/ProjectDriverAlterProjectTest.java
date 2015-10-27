package junit.plp.core.projectdriver;

import static org.junit.Assert.*;

import java.util.ArrayList;
import java.util.List;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import plptool.ArchRegistry;
import plptool.Constants;
import plptool.PLPAsmSource;
import plptool.gui.ProjectDriver;

public class ProjectDriverAlterProjectTest
{
	// TODO provide empty project, project with many files, etc
	// projects with files with same names.
	ProjectDriver projectDriver;
	String sampleProjectPath;
	String separateAsmFilePath = "autotests/junit/plp/core/long_file.asm";
	String separateAsmWithSameNameFilePath = "autotests/junit/plp/core/main.asm";
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
	public void createProjectThenOpen()
	{
		
	}
	
	@Test
	public void openSameProjectTwice()
	{
		//Open same project twice, with no modifications on first instance
		assertEquals("Project opens, returns success", Constants.PLP_OK,
				projectDriver.open(sampleProjectPath, false));
		assertEquals("Project opens, returns success", Constants.PLP_OK,
				projectDriver.open(sampleProjectPath, false));
		
		//Open project again, but original is modified
		projectDriver = new ProjectDriver(0);
		assertEquals("Project opens, returns success", Constants.PLP_OK,
				projectDriver.open(sampleProjectPath, false));
		
		projectDriver.importAsm(separateAsmFilePath);
		assertEquals("Project is modified after import", true, projectDriver.isModified());
		
		assertNotSame("Project opens, cant open project when it has changes", Constants.PLP_OK,
				projectDriver.open(sampleProjectPath, false));
	}
	
	@Test
	public void openDifferentProjectAfterOpeningOne()
	{
		
	}
	
	@Test
	public void createProjectAfterOpeningOne()
	{
		// Can create a project if project is not dirty/modified
		assertEquals("Project opens, returns success", Constants.PLP_OK,
				projectDriver.open(sampleProjectPath, false));
		assertEquals("Project is not modified after open", false,
				projectDriver.isModified());
		assertEquals("Project is not dirty after open", false,
				projectDriver.isDirty());
		
		assertEquals(
				"Can create new project over opened if not dirty/modified",
				Constants.PLP_OK,
				projectDriver.create(ArchRegistry.ISA_PLPMIPS));
		
		assertEquals("Project opens, returns success", Constants.PLP_OK,
				projectDriver.open(sampleProjectPath, false));
		projectDriver.importAsm(separateAsmFilePath);
		assertEquals("Import does not make files dirty.", false,
				projectDriver.isDirty());
		assertEquals("Import does marks project as modified", true,
				projectDriver.isModified());
		
		assertNotSame(
				"Project driver can not create when current project is modified",
				Constants.PLP_OK,
				projectDriver.create(ArchRegistry.ISA_PLPMIPS));
	}
	
	@Test
	public void addNewFileWithSameNameAsAnExistingFile()
	{
		projectDriver.open(sampleProjectPath, false);
		assertNotSame(
				"Can you import a file, with a name that already exists in the project.",
				Constants.PLP_OK, projectDriver.importAsm(separateAsmWithSameNameFilePath));
	}
	
	@Test
	public void addFilesToProject()
	{
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
