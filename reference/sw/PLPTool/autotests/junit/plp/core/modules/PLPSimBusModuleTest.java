package junit.plp.core.modules;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import plptool.Constants;
import plptool.PLPSimBus;

public class PLPSimBusModuleTest
{
	MockModule mockModule;
	PLPSimBus simBus;
	
	@Before
	public void startUp()
	{
		simBus = new PLPSimBus();
		mockModule = new MockModule();
	}
	
	@After
	public void tearDown()
	{
		
	}
	
	@Test
	public void enableModule()
	{
		int returnedIndex = simBus.add(mockModule);
		
		assertNotSame("Error occured on adding module", -1, returnedIndex);
		
		assertFalse("Module starts enabled, after adding",
				simBus.getEnabled(returnedIndex));
		
		assertEquals("Simbus enable mod failed", Constants.PLP_OK,
				simBus.enableMod(returnedIndex));
		
		assertTrue("Simbus enable module failed on module end",
				mockModule.enabled());
	}
	
}
