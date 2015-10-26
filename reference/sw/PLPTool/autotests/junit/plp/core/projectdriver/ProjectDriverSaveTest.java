package junit.plp.core.projectdriver;

import org.junit.After;
import org.junit.Before;

import plptool.gui.ProjectDriver;

/**
 * Save current project state to the file specified by plpfile.
 *
 * @return PLP_OK on successful save, error code otherwise
 */
public class ProjectDriverSaveTest
{
	ProjectDriver projectDriver;
	
	@Before
	public void startUp()
	{
		
	}
	
	@After
	public void tearDown()
	{
		projectDriver = null;
	}
}
