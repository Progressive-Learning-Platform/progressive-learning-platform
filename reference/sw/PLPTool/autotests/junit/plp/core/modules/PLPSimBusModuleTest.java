package junit.plp.core.modules;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import plptool.Constants;
import plptool.PLPSimBus;
import plptool.PLPSimBusModule;

public class PLPSimBusModuleTest
{
	PLPSimBusModule emptyModule;
	PLPSimBusModule mockModule;
	PLPSimBus simBus;
	
	@Before
	public void startUp()
	{
		simBus = new PLPSimBus();
		mockModule = new MockModule();
		emptyModule = new PLPSimBusModule() {
			
			@Override
			public int eval()
			{
				return 0;
			}
			
			@Override
			public int gui_eval(Object x)
			{
				return 0;
			}
			
			@Override
			public void reset()
			{
			}
			
			@Override
			public String introduce()
			{
				return null;
			}
		};
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
	
	@Test
	public void evaluateThread()
	{
		int returnedIndex = simBus.add(mockModule);
		
		assertNotSame("Error occured on adding module", -1, returnedIndex);
		
		assertEquals("Error occured in SimBus evaluate.", Constants.PLP_OK,
				simBus.eval());
		
		assertTrue(
				"Mock module thread start failed, or evaluate was not called",
				mockModule.isAlive());
	}
	
	@Test
	public void introduceTest()
	{
		int returnedIndex = simBus.add(mockModule);
		int emptyModuleIndex = simBus.add(emptyModule);
		
		assertNotSame("Error occured on adding module", -1, returnedIndex);
		
		assertEquals(mockModule.introduce(), simBus.introduceMod(returnedIndex));
		
		assertNull(simBus.introduceMod(emptyModuleIndex));
	}
	
}
