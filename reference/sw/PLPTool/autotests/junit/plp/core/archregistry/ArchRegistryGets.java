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

public class ArchRegistryGets
{
	ProjectDriver projectDriver;
	PLPArchitecture mockArchitecture;
	static int registeredID;
	static String architectureDescription;
	static String architectureStringID;
	
	@BeforeClass
	public static void initialize()
	{
		registeredID = Integer.MAX_VALUE;
		architectureDescription = "Mock Architecture";
		architectureStringID = Integer.toString(registeredID);
		
		ArchRegistry.registerArchitecture(MockArchitecture.class, registeredID,
				architectureStringID, architectureDescription);
	}
	
	@Before
	public void startUp()
	{
		projectDriver = new ProjectDriver(0);
	}
	
	@After
	public void tearDown()
	{
		projectDriver = null;
	}
	
	@Test
	public void getDescriptionTest()
	{
		assertNotNull("Registered description is not null",
				ArchRegistry.getDescription(registeredID));
		
		assertEquals("Description matches registered description",
				architectureDescription,
				ArchRegistry.getDescription(registeredID));
		
		assertEquals("Returns the same string",
				ArchRegistry.getDescription(registeredID),
				ArchRegistry.getDescription(registeredID));
	}
	
	/**
     * Get the string Identifier of the ISA implementation
     *
     * @param ID ID of the ISA meta class
     * @return the string identifier of the ISA
     */
	@Test
	public void getStringID()
	{
		assertNotNull("Registered description is not null",
				ArchRegistry.getStringID(registeredID));
		
		assertEquals("String ID matches registered ID",
				architectureStringID,
				ArchRegistry.getStringID(registeredID));
		
		assertEquals("Returns the same string",
				ArchRegistry.getStringID(registeredID),
				ArchRegistry.getStringID(registeredID));
	}
}
