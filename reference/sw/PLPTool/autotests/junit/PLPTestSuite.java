package junit;

import junit.plp.core.PLPASMSourceTest;
import junit.plpToolBox.PLPToolBoxSystemTesting;
import junit.plpToolBox.PLPToolboxParseTesting;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;

@RunWith(Suite.class)
@Suite.SuiteClasses({
  PLPToolboxParseTesting.class,
  PLPToolBoxSystemTesting.class,
  PLPASMSourceTest.class
})

public class PLPTestSuite
{
	
}
