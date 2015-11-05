package junit;

import junit.plp.architecture.registry.RegisterArchitectureTest;
import junit.plp.core.PLPASMSourceTest;
import junit.plp.core.PLPSimBusCoreTest;
import junit.plp.core.archregistry.ArchRegistryGetArchitecture;
import junit.plp.core.archregistry.ArchRegistryGets;
import junit.plp.core.modules.PLPSimBusModuleTest;
import junit.plp.core.projectdriver.ProjectDriverAlterProjectTest;
import junit.plp.core.projectdriver.ProjectDriverOpenTest;
import junit.plp.core.projectdriver.ProjectDriverSaveTest;
import junit.plpToolBox.PLPToolBoxSystemTesting;
import junit.plpToolBox.PLPToolboxParseTesting;
import junit.plptest.ArchRegistryTest;
import junit.plptest.PLPSimBusTest;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;

//@formatter:off
@RunWith(Suite.class)
@Suite.SuiteClasses({
	PLPSimBusTest.class,
	PLPSimBusModuleTest.class,
	PLPSimBusCoreTest.class,
	ArchRegistryGets.class,
	ArchRegistryGetArchitecture.class,
	ArchRegistryTest.class,
	RegisterArchitectureTest.class,
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
