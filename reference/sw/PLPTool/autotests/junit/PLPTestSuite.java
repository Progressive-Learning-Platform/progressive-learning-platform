package junit;

import junit.plp.core.PLPASMSourceTest;
import junit.plp.core.archregistry.ArchRegistryGetArchitecture;
import junit.plp.core.archregistry.ArchRegistryGets;
import junit.plp.core.projectdriver.ProjectDriverAlterProjectTest;
import junit.plp.core.projectdriver.ProjectDriverOpenTest;
import junit.plp.core.projectdriver.ProjectDriverSaveTest;
import junit.plpToolBox.PLPToolBoxSystemTesting;
import junit.plpToolBox.PLPToolboxParseTesting;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;

//@formatter:off
@RunWith(Suite.class)
@Suite.SuiteClasses({
	ArchRegistryGets.class,
	ArchRegistryGetArchitecture.class,
	ProjectDriverAlterProjectTest.class,
	ProjectDriverSaveTest.class,
	ProjectDriverOpenTest.class,
	PLPToolboxParseTesting.class,
	PLPToolBoxSystemTesting.class,
	PLPASMSourceTest.class
})
//@formatter:on
public class PLPTestSuite
{
	
}
