package edu.asu.plp.tool.tests.suites;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

import edu.asu.plp.tool.tests.bidirectionalmap.TestAddition;
import edu.asu.plp.tool.tests.bidirectionalmap.TestLifetime;
import edu.asu.plp.tool.tests.bidirectionalmap.TestRemoval;

@RunWith(Suite.class)
@SuiteClasses({ TestAddition.class, TestLifetime.class, TestRemoval.class })
public class BiDirectionalOneToManySuite
{
	
}
