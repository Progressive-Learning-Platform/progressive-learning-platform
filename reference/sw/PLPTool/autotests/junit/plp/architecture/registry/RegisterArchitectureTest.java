package junit.plp.architecture.registry;

import static org.junit.Assert.*;

import org.junit.Test;

import plptool.ArchRegistry;
import plptool.Constants;
import plptool.PLPArchitecture;
import plptool.gui.ProjectDriver;
import plptool.mips.Architecture;

/**
 * Unit testing of {@link ArchRegistry#registerArchitecture(Class, int, String, String)}
 * 
 * @author Moore, Zachary
 *
 */
public class RegisterArchitectureTest
{
	private static ProjectDriver driver = new ProjectDriver(Constants.PLP_DEFAULT);
	private static int ID = 1;
	
	@Test
	public void testRegisterNullArchitecture()
	{
		// Use an id that won't be registered in the lifetime of this test thread
		final int id = 1000;
		
		try
		{
			int initialRegistrySize = ArchRegistry.getArchList().length;
			int result = ArchRegistry.registerArchitecture(null, id, "Null",
					"Null Architechture");
			int registrySize = ArchRegistry.getArchList().length;
			
			// ensure the registration was not processed
			assertEquals(initialRegistrySize, registrySize);
			assertFalse(result == Constants.PLP_OK);
			assertNull(ArchRegistry.getArchitecture(driver, id));
			assertNull(ArchRegistry.getRegisteredArchitectureClass(id));
			assertNull(ArchRegistry.getDescription(id));
			assertNull(ArchRegistry.getStringID(id));
		}
		catch (Exception e)
		{
			fail("ArchRegistry is expected to return an appropriate error code instead of raising an exception: "
					+ e.getClass().getSimpleName());
		}
		
	}
	
	@Test
	public void testRegisterInvalidArchitecture()
	{
		final int id = ID++;
		
		try
		{
			int initialRegistrySize = ArchRegistry.getArchList().length;
			int result = ArchRegistry.registerArchitecture(String.class, id, "Invalid",
					"Invalid Architechture does not implement Architecture");
			int registrySize = ArchRegistry.getArchList().length;
			
			// ensure the registration was not processed
			assertEquals(initialRegistrySize, registrySize);
			assertFalse(result == Constants.PLP_OK);
			assertNull(ArchRegistry.getArchitecture(driver, id));
			assertNull(ArchRegistry.getRegisteredArchitectureClass(id));
			assertNull(ArchRegistry.getDescription(id));
			assertNull(ArchRegistry.getStringID(id));
		}
		catch (Exception e)
		{
			fail("ArchRegistry is expected to return an appropriate error code instead of raising an exception: "
					+ e.getClass().getSimpleName());
		}
	}
	
	@Test
	public void testRegisterObjectClass()
	{
		final int id = ID++;
		
		try
		{
			int initialRegistrySize = ArchRegistry.getArchList().length;
			int result = ArchRegistry.registerArchitecture(Object.class, id, "Object",
					"Invalid Architecture");
			int registrySize = ArchRegistry.getArchList().length;
			
			// ensure the registration was not processed
			assertEquals(initialRegistrySize, registrySize);
			assertFalse(result == Constants.PLP_OK);
			assertNull(ArchRegistry.getArchitecture(driver, id));
			assertNull(ArchRegistry.getRegisteredArchitectureClass(id));
			assertNull(ArchRegistry.getDescription(id));
			assertNull(ArchRegistry.getStringID(id));
		}
		catch (Exception e)
		{
			fail("ArchRegistry is expected to return an appropriate error code instead of raising an exception: "
					+ e.getClass().getSimpleName());
		}
	}
	
	/**
	 * The success of this test depends on the validity of ArchRegistry's get methods,
	 * particularly getArchitecture.
	 * <p>
	 * If this test fails in addition to one or more getArchitecture tests, the root cause
	 * is likely to be found in getArchitecture. (i.e. check getArchitecture before
	 * debugging registerArchitecture)
	 */
	@Test
	public void testRegisterValidArchitecture()
	{
		final int id = ID++;
		
		try
		{
			String description = "Valid Architechture implements Architecture";
			String stringID = "Valid";
			int initialRegistrySize = ArchRegistry.getArchList().length;
			int result = ArchRegistry.registerArchitecture(Architecture.class, id,
					stringID, description);
			int registrySize = ArchRegistry.getArchList().length;
			
			// ensure the registration was processed successfully
			assertEquals(initialRegistrySize + 1, registrySize);
			assertTrue(result == Constants.PLP_OK);
			assertEquals(Architecture.class,
					ArchRegistry.getRegisteredArchitectureClass(id));
			assertEquals(description, ArchRegistry.getDescription(id));
			assertEquals(stringID, ArchRegistry.getStringID(id));
			
			PLPArchitecture architecture = ArchRegistry.getArchitecture(driver, id);
			assertNotNull(architecture);

			assertEquals(id, architecture.getID());
			assertEquals(stringID, architecture.getStringID());
		}
		catch (Exception e)
		{
			fail("Unexpected exception: " + e.getClass().getSimpleName());
		}
	}
	
	@Test
	public void testRegisterArchitectureWithNegativeID()
	{
		final int id = -(ID++);
		
		try
		{
			String description = "Valid Architechture implements Architecture";
			String stringID = "Valid";
			int initialRegistrySize = ArchRegistry.getArchList().length;
			int result = ArchRegistry.registerArchitecture(Architecture.class, id,
					stringID, description);
			int registrySize = ArchRegistry.getArchList().length;
			
			// ensure the registration was processed successfully
			assertEquals(initialRegistrySize + 1, registrySize);
			assertTrue(result == Constants.PLP_OK);
			assertEquals(Architecture.class,
					ArchRegistry.getRegisteredArchitectureClass(id));
			assertEquals(description, ArchRegistry.getDescription(id));
			assertEquals(stringID, ArchRegistry.getStringID(id));
			
			PLPArchitecture architecture = ArchRegistry.getArchitecture(driver, id);
			assertNotNull(architecture);

			assertEquals(id, architecture.getID());
			assertEquals(stringID, architecture.getStringID());
		}
		catch (Exception e)
		{
			fail("Unexpected exception: " + e.getClass().getSimpleName());
		}
	}
	
	@Test
	public void testRegisterArchitectureWithNegativeIDAndNullStringID()
	{
		final int id = -(ID++);
		
		try
		{
			String description = "Valid Architechture implements Architecture";
			String stringID = null;
			int initialRegistrySize = ArchRegistry.getArchList().length;
			int result = ArchRegistry.registerArchitecture(Architecture.class, id,
					stringID, description);
			int registrySize = ArchRegistry.getArchList().length;
			
			// ensure the registration was processed successfully
			assertEquals(initialRegistrySize + 1, registrySize);
			assertTrue(result == Constants.PLP_OK);
			assertEquals(Architecture.class,
					ArchRegistry.getRegisteredArchitectureClass(id));
			assertEquals(description, ArchRegistry.getDescription(id));
			assertEquals(stringID, ArchRegistry.getStringID(id));
			
			PLPArchitecture architecture = ArchRegistry.getArchitecture(driver, id);
			assertNotNull(architecture);

			assertEquals(id, architecture.getID());
			assertEquals(stringID, architecture.getStringID());
		}
		catch (Exception e)
		{
			fail("Unexpected exception: " + e.getClass().getSimpleName());
		}
	}
	
	@Test
	public void testRegisterArchitectureWithNullStringID()
	{
		final int id = ID++;
		
		try
		{
			String description = "Valid Architechture implements Architecture";
			String stringID = null;
			int initialRegistrySize = ArchRegistry.getArchList().length;
			int result = ArchRegistry.registerArchitecture(Architecture.class, id,
					stringID, description);
			int registrySize = ArchRegistry.getArchList().length;
			
			// ensure the registration was processed successfully
			assertEquals(initialRegistrySize + 1, registrySize);
			assertTrue(result == Constants.PLP_OK);
			assertEquals(Architecture.class,
					ArchRegistry.getRegisteredArchitectureClass(id));
			assertEquals(description, ArchRegistry.getDescription(id));
			assertEquals(stringID, ArchRegistry.getStringID(id));
			
			PLPArchitecture architecture = ArchRegistry.getArchitecture(driver, id);
			assertNotNull(architecture);

			assertEquals(id, architecture.getID());
			assertEquals(stringID, architecture.getStringID());
		}
		catch (Exception e)
		{
			fail("Unexpected exception: " + e.getClass().getSimpleName());
		}
	}
	
	@Test
	public void testRegisterArchitectureWithNullStringIDAndDescription()
	{
		final int id = ID++;
		
		try
		{
			String description = null;
			String stringID = null;
			int initialRegistrySize = ArchRegistry.getArchList().length;
			int result = ArchRegistry.registerArchitecture(Architecture.class, id,
					stringID, description);
			int registrySize = ArchRegistry.getArchList().length;
			
			// ensure the registration was processed successfully
			assertEquals(initialRegistrySize + 1, registrySize);
			assertTrue(result == Constants.PLP_OK);
			assertEquals(Architecture.class,
					ArchRegistry.getRegisteredArchitectureClass(id));
			assertEquals(description, ArchRegistry.getDescription(id));
			assertEquals(stringID, ArchRegistry.getStringID(id));
			
			PLPArchitecture architecture = ArchRegistry.getArchitecture(driver, id);
			assertNotNull(architecture);

			assertEquals(id, architecture.getID());
			assertEquals(stringID, architecture.getStringID());
		}
		catch (Exception e)
		{
			fail("Unexpected exception: " + e.getClass().getSimpleName());
		}
	}
	
	@Test
	public void testRegisterArchitectureWithEmptyStringID()
	{
		final int id = ID++;
		
		try
		{
			String description = "Valid Description";
			String stringID = "";
			int initialRegistrySize = ArchRegistry.getArchList().length;
			int result = ArchRegistry.registerArchitecture(Architecture.class, id,
					stringID, description);
			int registrySize = ArchRegistry.getArchList().length;
			
			// ensure the registration was processed successfully
			assertEquals(initialRegistrySize + 1, registrySize);
			assertTrue(result == Constants.PLP_OK);
			assertEquals(Architecture.class,
					ArchRegistry.getRegisteredArchitectureClass(id));
			assertEquals(description, ArchRegistry.getDescription(id));
			assertEquals(stringID, ArchRegistry.getStringID(id));
			
			PLPArchitecture architecture = ArchRegistry.getArchitecture(driver, id);
			assertNotNull(architecture);

			assertEquals(id, architecture.getID());
			assertEquals(stringID, architecture.getStringID());
		}
		catch (Exception e)
		{
			fail("Unexpected exception: " + e.getClass().getSimpleName());
		}
	}
	
	@Test
	public void testRegisterArchitectureWithEmptyStringIDAndDescription()
	{
		final int id = ID++;
		
		try
		{
			String description = "";
			String stringID = "";
			int initialRegistrySize = ArchRegistry.getArchList().length;
			int result = ArchRegistry.registerArchitecture(Architecture.class, id,
					stringID, description);
			int registrySize = ArchRegistry.getArchList().length;
			
			// ensure the registration was processed successfully
			assertEquals(initialRegistrySize + 1, registrySize);
			assertTrue(result == Constants.PLP_OK);
			assertEquals(Architecture.class,
					ArchRegistry.getRegisteredArchitectureClass(id));
			assertEquals(description, ArchRegistry.getDescription(id));
			assertEquals(stringID, ArchRegistry.getStringID(id));
			
			PLPArchitecture architecture = ArchRegistry.getArchitecture(driver, id);
			assertNotNull(architecture);

			assertEquals(id, architecture.getID());
			assertEquals(stringID, architecture.getStringID());
		}
		catch (Exception e)
		{
			fail("Unexpected exception: " + e.getClass().getSimpleName());
		}
	}
	
	/**
	 * Test adding a valid architecture, but assert only that the given data was
	 * registered successfully. Do not assert that getArchitecture returns the appropriate
	 * value.
	 * <p>
	 * For a test that does validate data returned by getArchitecture, see
	 * {@link #testRegisterValidArchitecture()}
	 */
	@Test
	public void testRegisterValidArchitectureAndAssertDataOnly()
	{
		final int id = ID++;
		
		try
		{
			String description = "Valid Architechture implements Architecture";
			String stringID = "Valid";
			int initialRegistrySize = ArchRegistry.getArchList().length;
			int result = ArchRegistry.registerArchitecture(Architecture.class, id,
					stringID, description);
			int registrySize = ArchRegistry.getArchList().length;
			
			// ensure the registration was processed successfully
			assertEquals(initialRegistrySize + 1, registrySize);
			assertTrue(result == Constants.PLP_OK);
			assertEquals(Architecture.class,
					ArchRegistry.getRegisteredArchitectureClass(id));
			assertEquals(description, ArchRegistry.getDescription(id));
			assertEquals(stringID, ArchRegistry.getStringID(id));
		}
		catch (Exception e)
		{
			fail("Unexpected exception: " + e.getClass().getSimpleName());
		}
	}
	
	@Test
	public void testRegisterInterface()
	{
		final int id = ID++;
		
		try
		{
			String description = "Interfaces are invalid architectures";
			String stringID = "Interface";
			int initialRegistrySize = ArchRegistry.getArchList().length;
			int result = ArchRegistry.registerArchitecture(PLPArchitecture.class, id,
					stringID, description);
			int registrySize = ArchRegistry.getArchList().length;
			
			// ensure the registration was not processed
			assertEquals(initialRegistrySize, registrySize);
			assertFalse(result == Constants.PLP_OK);
			assertNull(ArchRegistry.getArchitecture(driver, id));
			assertNull(ArchRegistry.getRegisteredArchitectureClass(id));
			assertNull(ArchRegistry.getDescription(id));
			assertNull(ArchRegistry.getStringID(id));
		}
		catch (Exception e)
		{
			fail("ArchRegistry is expected to return an appropriate error code instead of raising an exception: "
					+ e.getClass().getSimpleName());
		}
	}
}
