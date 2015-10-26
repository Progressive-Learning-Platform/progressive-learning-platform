package junit;

import junit.plp.core.PLPASMSourceTest;
import junit.plp.core.projectdriver.ProjectDriverTest;
import junit.plpToolBox.PLPToolBoxSystemTesting;
import junit.plpToolBox.PLPToolboxParseTesting;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;

//@formatter:off
@RunWith(Suite.class)
@Suite.SuiteClasses({
	ProjectDriverTest.class,
	PLPToolboxParseTesting.class,
	PLPToolBoxSystemTesting.class,
	PLPASMSourceTest.class
})
//@formatter:on
public class PLPTestSuite
{
	
}
