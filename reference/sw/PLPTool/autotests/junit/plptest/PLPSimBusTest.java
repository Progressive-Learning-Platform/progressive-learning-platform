package junit.plptest;

import org.junit.Before;
import org.junit.Test;
import plptool.Constants;
import plptool.PLPSimBus;
import plptool.mods.MemModule;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertTrue;
import static org.junit.Assert.*;

public class PLPSimBusTest
{
	private MemModule memModule;
	private PLPSimBus plpSimBus;
	
	@Before
	public void setup()
	{
		memModule = new MemModule(100L, 200, true);
		plpSimBus = new PLPSimBus();
	}
	
	@Test
	public void testAddSuccessfully()
	{
		int index = plpSimBus.add(memModule);
		assertEquals(index, 0);
		assertNotNull(index);
	}
	
	/**
	 * This test relies on {@link PLPSimBus#getNumOfMods()}. If
	 * {@link PLPSimBus#getNumOfMods()} fails in conjunction with this test, it is
	 * possible that the problem resides there rather than in this method.
	 */
	@Test
	public void testAddNull()
	{
		memModule = null;
		assertEquals(plpSimBus.getNumOfMods(), 0);
		int index = plpSimBus.add(memModule);
		assertEquals(plpSimBus.getNumOfMods(), 0);
	}
	
	/** Checks to see if remove() and getNumOfMods() is working or not */
	@Test
	public void testRemove()
	{
		int indx = plpSimBus.add(memModule);
		
		int res = plpSimBus.remove(indx);
		
		assertEquals(res, Constants.PLP_OK);
		assertEquals(plpSimBus.getNumOfMods(), 0);
	}
	
	// removing a module not already in simbus
	@Test(expected = IndexOutOfBoundsException.class)
	public void testRemoveForException()
	{
		plpSimBus.remove(1);
	}
	
	@Test
	public void testReadSuccessfully()
	{
		int indx = plpSimBus.add(memModule);
		plpSimBus.enableMod(indx);
		plpSimBus.write(120L, 1234L, true);
		Object value = plpSimBus.read(120L);
		assertNotNull(value);
	}
	
	@Test
	public void testReadWithoutWrite()
	{
		int indx = plpSimBus.add(memModule);
		plpSimBus.enableMod(indx);
		assertEquals(0L, plpSimBus.read(120L));
		assertNull(plpSimBus.read(20L));
	}
	
	@Test
	public void testReadWithoutModule()
	{
		assertNull(plpSimBus.read(25L));
	}
	
	@Test
	public void testReadOutOfBounds()
	{
		int indx = plpSimBus.add(memModule);
		plpSimBus.enableMod(indx);
		assertNull(plpSimBus.read(20L));
	}
	
	@Test
	public void testWriteSuccessfully()
	{
		Long data = 1234L;
		plpSimBus.add(memModule);
		plpSimBus.enableMod(0);
		
		int res = plpSimBus.write(120L, data, true);
		assertEquals(res, Constants.PLP_OK);
		
		Object value = plpSimBus.read(120L);
		assertEquals((Long) value, data);
	}
	
	@Test
	public void testWriteWithoutEnableModule()
	{
		plpSimBus.add(memModule);
		plpSimBus.write(120L, 1234L, true);
		
		assertEquals(0L, plpSimBus.read(120L));
	}
	
	@Test
	public void testWriteWithoutModule()
	{
		plpSimBus.write(120L, 12L, true);
		assertNull(plpSimBus.read(120L));
	}
	
	@Test
	public void testWriteOutofbounds()
	{
		plpSimBus.add(memModule);
		plpSimBus.write(20L, 1234L, true);
		
		assertEquals(0L, plpSimBus.read(120L));
		assertNull(plpSimBus.read(20L));
	}
	
	@Test
	public void testIsInitialized()
	{
		int index = plpSimBus.add(memModule);
		assertFalse(plpSimBus.isInitialized(120L));
		plpSimBus.enableMod(index);
		
		plpSimBus.write(120L, 1234L, false);
		assertTrue(plpSimBus.isInitialized(120L));
	}
	
	@Test
	public void testIsMapped()
	{
		assertFalse(plpSimBus.isMapped(120L));
		
		int index = plpSimBus.add(memModule);
		plpSimBus.enableMod(index);
		
		plpSimBus.write(120L, 1234L, true);
		assertTrue(plpSimBus.isMapped(120L));
	}
	
	@Test
	public void testIsInstr()
	{
		int index = plpSimBus.add(memModule);
		plpSimBus.enableMod(index);
		
		plpSimBus.write(120L, 1234L, true);
		assertTrue(plpSimBus.isInstr(120L));
		
		plpSimBus.write(132L, 12L, false);
		assertFalse(plpSimBus.isInstr(132L));
	}
	
	@Test
	public void testEnableMods()
	{
		int indx1 = plpSimBus.add(new MemModule(500L, 50L, true));
		int indx2 = plpSimBus.add(new MemModule(600L, 50L, true));
		
		assertEquals(plpSimBus.enableAllModules(), Constants.PLP_OK);
		assertTrue(plpSimBus.getEnabled(indx1));
		assertTrue(plpSimBus.getEnabled(indx2));
	}
	
	@Test
	public void testDisableMods()
	{
		int indx1 = plpSimBus.add(new MemModule(500L, 50L, true));
		int indx2 = plpSimBus.add(new MemModule(600L, 50L, true));
		plpSimBus.add(new MemModule(600L, 50L, true));
		
		assertEquals(plpSimBus.enableAllModules(), Constants.PLP_OK);
		assertEquals(plpSimBus.disableAllModules(), Constants.PLP_OK);
		assertFalse(plpSimBus.getEnabled(indx1));
		assertFalse(plpSimBus.getEnabled(indx2));
	}
	
	@Test
	public void testGetEnabled()
	{
		plpSimBus.add(new MemModule(500L, 50L, true));
		plpSimBus.add(new MemModule(600L, 50L, true));
		
		assertFalse(plpSimBus.getEnabled(0));
		plpSimBus.enableAllModules();
		
		assertTrue(plpSimBus.getEnabled(0));
		assertTrue(plpSimBus.getEnabled(1));
		
	}
	
	@Test
	public void testEnableMod()
	{
		int indx1 = plpSimBus.add(new MemModule(500L, 50L, true));
		int indx2 = plpSimBus.add(new MemModule(600L, 50L, true));
		
		plpSimBus.enableMod(indx1);
		
		assertTrue(plpSimBus.getEnabled(indx1));
		assertFalse(plpSimBus.getEnabled(indx2));
	}
	
	@Test
	public void testDisableMod()
	{
		int indx1 = plpSimBus.add(new MemModule(500L, 50L, true));
		int indx2 = plpSimBus.add(new MemModule(600L, 50L, true));
		
		plpSimBus.enableMod(indx1);
		plpSimBus.enableMod(indx2);
		
		plpSimBus.disableMod(indx1);
		
		assertTrue(plpSimBus.getEnabled(indx2));
		assertFalse(plpSimBus.getEnabled(indx1));
	}
	
	@Test
	public void testGetStartValues()
	{
		plpSimBus.add(new MemModule(500L, 50L, true));
		plpSimBus.add(new MemModule(600L, 50L, true));
		
		assertEquals(plpSimBus.getModStartAddress(0), 500L);
		assertEquals(plpSimBus.getModStartAddress(1), 600L);
	}
	
	@Test
	public void testGetEndValues()
	{
		plpSimBus.add(new MemModule(500L, 50L, true));
		
		assertEquals(plpSimBus.getModEndAddress(0), 546L);
	}
	
	@Test
	public void testGetNoMods()
	{
		plpSimBus.add(new MemModule(500L, 50L, true));
		plpSimBus.add(new MemModule(600L, 50L, true));
		
		assertEquals(2, plpSimBus.getNumOfMods());
	}
	
	@Test
	public void testgetRefMods()
	{
		MemModule mod = new MemModule(500L, 50L, true);
		plpSimBus.add(mod);
		
		assertEquals(mod, plpSimBus.getRefMod(0));
	}
	
	@Test
	public void testUncheckedRead()
	{
		int indx = plpSimBus.add(memModule);
		plpSimBus.enableMod(indx);
		plpSimBus.write(120L, 1234L, true);
		Object value = plpSimBus.uncheckedRead(120L);
		assertNotNull(value);
	}
	
	@Test
	public void testEval()
	{
		int indx = plpSimBus.add(memModule);
		plpSimBus.enableMod(indx);
		
		assertEquals(Constants.PLP_OK, plpSimBus.eval(indx));
	}
	
	@Test
	public void testEvalAll()
	{
		plpSimBus.add(memModule);
		plpSimBus.enableAllModules();
		
		assertEquals(Constants.PLP_OK, plpSimBus.eval());
	}
	
	@Test
	public void testClearModRegisters()
	{
		int indx1 = plpSimBus.add(new MemModule(500L, 50L, true));
		int indx2 = plpSimBus.add(new MemModule(600L, 50L, true));
		plpSimBus.enableAllModules();
		
		plpSimBus.write(500L, 1234L, true);
		
		assertEquals(Constants.PLP_OK, plpSimBus.clearModRegisters(indx1));
		assertEquals(Constants.PLP_OK, plpSimBus.clearModRegisters(indx2));
		assertEquals(0L, plpSimBus.read(500L));
	}
	
}
