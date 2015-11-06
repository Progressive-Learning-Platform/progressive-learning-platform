package junit.plptest;

import java.util.Deque;
import java.util.LinkedList;
import java.util.Map;

import junit.plp.core.modules.MockModule;

import org.apache.commons.collections15.map.HashedMap;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import plptool.Constants;
import plptool.PLPSimBus;
import plptool.PLPSimBusModule;
import plptool.dmf.Callback;
import plptool.dmf.CallbackRegistry;
import plptool.mips.MIPSInstr;
import plptool.mods.MemModule;
import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertTrue;
import static org.junit.Assert.*;

public class PLPSimBusTest
{
	private static class CallbackTracker implements Callback
	{
		public int callCount = 0;
		
		@Override
		public boolean callback(int callbackNum, Object param)
		{
			callCount++;
			return true;
		}
		
		public boolean wasCalled()
		{
			return callCount > 0;
		}
		
		public void reset()
		{
			callCount = 0;
		}
	}
	
	private static CallbackTracker addCallbackTracker;
	private MemModule memModule;
	private MockModule mockModule;
	private PLPSimBus plpSimBus;
	
	@BeforeClass
	public static void setupClass()
	{
		addCallbackTracker = new CallbackTracker();
		CallbackRegistry.register(addCallbackTracker, CallbackRegistry.BUS_ADD);
	}
	
	@Before
	public void setup()
	{
		memModule = new MemModule(100L, 200, true);
		mockModule = new MockModule();
		addCallbackTracker.reset();
		plpSimBus = new PLPSimBus();
	}
	
	@Test
	public void testAddSuccessfully()
	{
		int numberOfModulesPrior = plpSimBus.getNumOfMods();
		int expectedNumberOfModules = numberOfModulesPrior + 1;
		int index = plpSimBus.add(memModule);
		boolean errorOccurred = (index == -1);
		int numberOfModulesAfter = plpSimBus.getNumOfMods();
		
		assertFalse(errorOccurred);
		assertEquals(expectedNumberOfModules, numberOfModulesAfter);
		assertTrue(addCallbackTracker.wasCalled());
		assertSame(memModule, plpSimBus.getRefMod(index));
	}
	
	/**
	 * This test relies on {@link PLPSimBus#getNumOfMods()}. If
	 * {@link PLPSimBus#getNumOfMods()} fails in conjunction with this test, it is
	 * possible that the problem resides there rather than in this method.
	 * 
	 * Null is an invalid value for a module, and should not be added
	 */
	@Test
	public void testAddNull()
	{
		int numberOfModulesPrior = plpSimBus.getNumOfMods();
		int index = plpSimBus.add(null);
		int numberOfModulesAfter = plpSimBus.getNumOfMods();
		
		boolean errorOccurred = (index == -1);
		
		assertTrue(errorOccurred);
		assertEquals(numberOfModulesPrior, numberOfModulesAfter);
		assertFalse(addCallbackTracker.wasCalled());
	}
	
	@Test
	public void testAddMultipleValidModules()
	{
		int numberOfModulesPrior = plpSimBus.getNumOfMods();
		int index1 = plpSimBus.add(memModule);
		int index2 = plpSimBus.add(mockModule);
		int expectedOfModules = numberOfModulesPrior + 2;
		int numberOfModulesAfter = plpSimBus.getNumOfMods();
		
		boolean errorOccurred = (index1 == -1) || (index2 == -1);
		
		assertFalse(errorOccurred);
		assertEquals(expectedOfModules, numberOfModulesAfter);
		assertEquals(2, addCallbackTracker.callCount);
	}
	
	@Test
	public void testAddMultipleEquivalentModules()
	{
		int numberOfModulesPrior = plpSimBus.getNumOfMods();
		MemModule memModule1 = new MemModule(0, 512, true);
		MemModule memModule2 = new MemModule(0, 512, true);
		int index1 = plpSimBus.add(memModule1);
		int index2 = plpSimBus.add(memModule2);
		int expectedOfModules = numberOfModulesPrior + 2;
		int numberOfModulesAfter = plpSimBus.getNumOfMods();
		
		boolean errorOccurred = (index1 == -1) || (index2 == -1);
		
		assertFalse(errorOccurred);
		assertEquals(expectedOfModules, numberOfModulesAfter);
		assertEquals(2, addCallbackTracker.callCount);
	}
	
	@Test
	public void testAddMultipleInstances()
	{
		int numberOfModulesPrior = plpSimBus.getNumOfMods();
		int index1 = plpSimBus.add(memModule);
		int index2 = plpSimBus.add(memModule);
		int expectedOfModules = numberOfModulesPrior + 1;
		int numberOfModulesAfter = plpSimBus.getNumOfMods();
		
		boolean errorOccurredOn1 = (index1 == -1);
		boolean errorOccurredOn2 = (index2 == -1);
		
		assertFalse(errorOccurredOn1);
		assertTrue(errorOccurredOn2);
		assertEquals(expectedOfModules, numberOfModulesAfter);
		assertEquals(1, addCallbackTracker.callCount);
		assertSame(memModule, plpSimBus.getRefMod(index1));
	}
	
	/** 
	 * Validate that the given indexes remain consistent even after removing modules
	 */
	@Test
	public void testIndexUniquenessOnAdd()
	{
		final int AMOUNT = 5;
		LinkedList<Integer> indexes = new LinkedList<>();
		
		for (int i = 0; i < AMOUNT; i++)
		{
			MockModule module = new MockModule();
			int index = plpSimBus.add(module);
			boolean errorOccurred = (index == -1);
			assertFalse(errorOccurred);
			assertFalse(indexes.contains(indexes));
			indexes.add(index);
		}
	}
	
	/** Checks to see if remove() and getNumOfMods() is working or not */
	@Test
	public void testRemove()
	{
		int numberOfModulesPrior = plpSimBus.getNumOfMods();
		int index = plpSimBus.add(memModule);
		int result = plpSimBus.remove(index);
		
		assertEquals(result, Constants.PLP_OK);
		assertEquals(numberOfModulesPrior, plpSimBus.getNumOfMods());
	}
	
	/** 
	 * Validate that the given indexes remain consistent even after removing modules
	 */
	@Test
	public void testIndexConsistencyOnRemoveEnd()
	{
		final int AMOUNT = 5;
		Deque<Integer> indexes = new LinkedList<>();
		Map<Integer, PLPSimBusModule> modules = new HashedMap<>();
		int expectedOfModules = AMOUNT - 1;
		
		for (int i = 0; i < AMOUNT; i++)
		{
			MockModule module = new MockModule();
			int index = plpSimBus.add(module);
			modules.put(index, module);
			indexes.add(index);
		}
		
		int lastIndex = indexes.pollLast();
		int result = plpSimBus.remove(lastIndex);
		
		assertEquals(result, Constants.PLP_OK);
		assertEquals(expectedOfModules, plpSimBus.getNumOfMods());
		
		for (int index : indexes)
			assertSame(modules.get(index), plpSimBus.getRefMod(index));
	}
	
	/** 
	 * Validate that the given indexes remain consistent even after removing modules
	 */
	@Test
	public void testIndexConsistencyOnRemoveFirst()
	{
		final int AMOUNT = 5;
		Deque<Integer> indexes = new LinkedList<>();
		Map<Integer, PLPSimBusModule> modules = new HashedMap<>();
		int expectedOfModules = AMOUNT - 1;
		
		for (int i = 0; i < AMOUNT; i++)
		{
			MockModule module = new MockModule();
			int index = plpSimBus.add(module);
			modules.put(index, module);
			indexes.add(index);
		}
		
		int firstIndex = indexes.pollFirst();
		int result = plpSimBus.remove(firstIndex);
		
		assertEquals(result, Constants.PLP_OK);
		assertEquals(expectedOfModules, plpSimBus.getNumOfMods());
		
		for (int index : indexes)
			assertSame(modules.get(index), plpSimBus.getRefMod(index));
	}
	
	/** 
	 * Validate that the given indexes remain consistent even after removing modules
	 */
	@Test
	public void testIndexConsistencyOnRemoveMiddle()
	{
		final int AMOUNT = 5;
		LinkedList<Integer> indexes = new LinkedList<>();
		Map<Integer, PLPSimBusModule> modules = new HashedMap<>();
		int expectedOfModules = AMOUNT - 1;
		
		for (int i = 0; i < AMOUNT; i++)
		{
			MockModule module = new MockModule();
			int index = plpSimBus.add(module);
			modules.put(index, module);
			indexes.add(index);
		}
		
		int targetIndex = AMOUNT / 2;
		int middleIndex = indexes.get(targetIndex);
		indexes.remove(targetIndex);
		int result = plpSimBus.remove(middleIndex);
		
		assertEquals(result, Constants.PLP_OK);
		assertEquals(expectedOfModules, plpSimBus.getNumOfMods());
		
		for (int index : indexes)
			assertSame(modules.get(index), plpSimBus.getRefMod(index));
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
	public void testWriteOutOfBoundsOfModule()
	{
		//100L and 200
		plpSimBus.add(memModule);
		
		long lowerBound = 99L;
		long higherBound = 201L;
		
		assertNotSame(Constants.PLP_OK, plpSimBus.write(lowerBound, Long.MAX_VALUE, false));
		assertNotSame(Constants.PLP_OK, plpSimBus.write(higherBound, Long.MAX_VALUE, false));
		
		//No module in range, no read
		assertEquals(-1, plpSimBus.read(lowerBound));
		assertEquals(-1, plpSimBus.read(higherBound));
	}
	
	@Test
	public void testWriteAddressBoundsNoModule()
	{
		long negativeLongBound = Long.MIN_VALUE;
		long negativeIntegerLowerBound = Integer.MIN_VALUE;
		long lowerBound = 0L;
		long positiveIntegerMaxBound = Integer.MAX_VALUE;
		long positiveLongBound = Long.MIN_VALUE;
		
		assertNotSame(Constants.PLP_OK, plpSimBus.write(negativeLongBound, Long.MAX_VALUE, false));
		assertNotSame(Constants.PLP_OK, plpSimBus.write(negativeIntegerLowerBound, Long.MAX_VALUE, false));
		assertNotSame(Constants.PLP_OK, plpSimBus.write(lowerBound, Long.MAX_VALUE, false));
		assertNotSame(Constants.PLP_OK, plpSimBus.write(positiveIntegerMaxBound, Long.MAX_VALUE, false));
		assertNotSame(Constants.PLP_OK, plpSimBus.write(positiveLongBound, Long.MAX_VALUE, false));
	}
	
	@Test
	public void testWriteAddressBoundsWithModuleNoOverlap()
	{
		plpSimBus.add(memModule);
		long negativeLongBound = Long.MIN_VALUE;
		long negativeIntegerLowerBound = Integer.MIN_VALUE;
		long lowerBound = 0L;
		long positiveIntegerMaxBound = Integer.MAX_VALUE;
		long positiveLongBound = Long.MIN_VALUE;
		
		assertNotSame(Constants.PLP_OK, plpSimBus.write(negativeLongBound, Long.MAX_VALUE, false));
		assertNotSame(Constants.PLP_OK, plpSimBus.write(negativeIntegerLowerBound, Long.MAX_VALUE, false));
		assertNotSame(Constants.PLP_OK, plpSimBus.write(lowerBound, Long.MAX_VALUE, false));
		assertNotSame(Constants.PLP_OK, plpSimBus.write(positiveIntegerMaxBound, Long.MAX_VALUE, false));
		assertNotSame(Constants.PLP_OK, plpSimBus.write(positiveLongBound, Long.MAX_VALUE, false));
	}
	
	@Test
	public void testWriteToUnEnabledModuleInRange()
	{
		int addedIndex = plpSimBus.add(memModule);
		plpSimBus.disableMod(addedIndex);
		
		//100L to 200
		long inclusiveLowerBound = 100L;
		long inclusiveUpperBound = 200L;
		long mediumBound = 150L;
		
		assertNotSame(Constants.PLP_OK, plpSimBus.write(inclusiveLowerBound, Long.MAX_VALUE, false));
		assertNotSame(Constants.PLP_OK, plpSimBus.write(mediumBound, Long.MAX_VALUE, false));
		assertNotSame(Constants.PLP_OK, plpSimBus.write(inclusiveUpperBound, Long.MAX_VALUE, false));
		
		//Disabled but should still allow read, initial values
		assertEquals(0L, plpSimBus.read(inclusiveUpperBound));
		assertEquals(0L, plpSimBus.read(mediumBound));
		assertEquals(0L, plpSimBus.read(inclusiveUpperBound));
	}
	
	@Test
	public void testWriteToEnabledModuleInRangeConstantValue()
	{
		int addedIndex = plpSimBus.add(memModule);
		plpSimBus.enableMod(addedIndex);
		
		//100L to 200
		long inclusiveLowerBound = 100L;
		long inclusiveUpperBound = 200L;
		long mediumBound = 150L;
		
		assertEquals(Constants.PLP_OK, plpSimBus.write(inclusiveLowerBound, Long.MAX_VALUE, false));
		assertEquals(Constants.PLP_OK, plpSimBus.write(mediumBound, Long.MAX_VALUE, false));
		assertEquals(Constants.PLP_OK, plpSimBus.write(inclusiveUpperBound, Long.MAX_VALUE, false));
		
		//Should read correct value
		assertEquals(Long.MAX_VALUE, plpSimBus.read(inclusiveUpperBound));
		assertEquals(Long.MAX_VALUE, plpSimBus.read(mediumBound));
		assertEquals(Long.MAX_VALUE, plpSimBus.read(inclusiveUpperBound));
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
