package junit.plp.core.archregistry;

import static org.junit.Assert.*;
import javafx.scene.shape.Arc;

import org.junit.After;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import plptool.ArchRegistry;
import plptool.PLPArchitecture;
import plptool.gui.ProjectDriver;

public class ArchRegistryGetArchitecture
{
	ProjectDriver projectDriver;
	MockArchitecture mockArchitecture;
	String mockDescription;
	String mockStringID;
	int registeredID;
	
	@Before
	public void startUp()
	{
		projectDriver = new ProjectDriver(0);
		mockStringID = "Mock String ID";
		mockDescription = "Mock Description";
	}
	
	@After
	public void tearDown()
	{
		projectDriver = null;
	}
	
	@Test
	public void usePLPMipsISA()
	{
		PLPArchitecture returnedArchitecture = ArchRegistry.getArchitecture(
				projectDriver, ArchRegistry.ISA_PLPMIPS);
		
		assertNotNull("static PLP MIPS ID returns null.", returnedArchitecture);
		
		assertEquals(
				"Static ArchRegistry ISA_PLPMIPS matches returned Architecture id.",
				ArchRegistry.ISA_PLPMIPS, returnedArchitecture.getID());
	}
	
	@Test
	public void testUpperBoundsID()
	{
		registeredID = Integer.MAX_VALUE;
		
		ArchRegistry.registerArchitecture(MockArchitecture.class, registeredID,
				mockStringID, mockDescription);
		
		PLPArchitecture returnedArchitecture = ArchRegistry.getArchitecture(
				projectDriver, registeredID);
		
		assertNotNull("upper bounds ID returns null.", returnedArchitecture);
		
		assertEquals(
				"Registered upper bound id doesn't matches returned Architecture id.",
				registeredID, returnedArchitecture.getID());
		
	}
	
	@Test
	public void testNegativeBoundsID()
	{
		registeredID = Integer.MIN_VALUE;
		
		ArchRegistry.registerArchitecture(MockArchitecture.class, registeredID,
				mockStringID, mockDescription);
		
		PLPArchitecture returnedArchitecture = ArchRegistry.getArchitecture(
				projectDriver, registeredID);
		
		assertNotNull(
				"New Project Driver and negative bounds ID returns null.",
				returnedArchitecture);
		
		assertEquals(
				"Registered negative bound id doesn't matches returned Architecture id.",
				registeredID, returnedArchitecture.getID());
	}
	
	@Test
	public void registerOverPreRegisteredID()
	{
		ArchRegistry.registerArchitecture(MockArchitecture.class, 0,
				mockStringID, mockDescription);
		
		PLPArchitecture returnedArchitecture = ArchRegistry.getArchitecture(
				projectDriver, registeredID);
		
		assertNotNull("overwritten ID returns null.", returnedArchitecture);
		
		assertNotSame("ArchRegistry registered was overwritten.", mockStringID,
				ArchRegistry.getStringID(0));
		
		assertNotSame("ArchRegistry registered was overwritten.",
				mockDescription, ArchRegistry.getDescription(0));
	}
	
	@Test
	public void getNonRegisteredID()
	{
		registeredID = Integer.MAX_VALUE / 2;
		
		PLPArchitecture returnedArchitecture = ArchRegistry.getArchitecture(
				projectDriver, registeredID);
		
		assertNull(returnedArchitecture);
	}
	
}
