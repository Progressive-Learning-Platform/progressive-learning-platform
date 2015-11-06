package junit.plp.core;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import plptool.Constants;
import plptool.PLPSimBus;
import plptool.PLPSimBusModule;
import plptool.mips.SimCore.mem;
import plptool.mods.MemModule;

public class PLPSimBusCoreTest
{
	PLPSimBus simBus;
	PLPSimBusModule memoryModule;
	long wordBitSize;
	long endAddress;
	long startAddress;
	int moduleIndex;
	
	@Before
	public void startUp()
	{
		simBus = new PLPSimBus();
		
		wordBitSize = 32;
		endAddress = 32;
		startAddress = 0;
		
		memoryModule = new MemModule(startAddress, wordBitSize
				* endAddress, true);
		
		moduleIndex = simBus.add(memoryModule);
	}
	
	@After
	public void tearDown()
	{
		
	}
	
	@Test
	public void issueZeroesUnknownIndex()
	{
		int aboveOneIndex = simBus.getNumOfMods() + 1;
		int belowZeroIndex = -1;
		
		try
		{
			simBus.issueZeroes(Integer.MAX_VALUE);
			simBus.issueZeroes(Integer.MAX_VALUE / 2);
			simBus.issueZeroes(aboveOneIndex);
			simBus.issueZeroes(belowZeroIndex);
			simBus.issueZeroes(Integer.MIN_VALUE / 2);
			simBus.issueZeroes(Integer.MIN_VALUE);
		}
		catch (IndexOutOfBoundsException exception)
		{
			fail("index should be checked if its valid.");
		}
		catch (Exception exception)
		{
			fail(exception.getMessage());
		}
	}
	
	@Test
	public void issueZeroes()
	{
		simBus.enableMod(moduleIndex);
		
		for (int index = 0; index < endAddress; index++)
		{
			Object returnValue = memoryModule.readReg(startAddress
					+ (index * wordBitSize));
			
			assertNotSame(
					"Reading registers after initialization return PLP_ERROR_RETURN.",
					Constants.PLP_ERROR_RETURN, returnValue);
			
			// MemModule read is overrided to Long
			assertEquals("Reading registers after initialization aren't 0.",
					0L, returnValue);
		}
		
		long address = startAddress + (2 * wordBitSize);
		long writtenMemory = 5L;
		
		memoryModule.writeReg(address, writtenMemory, false);
		
		assertEquals(
				"Memory at specified address is not the same as the written data",
				writtenMemory, memoryModule.readReg(address));

		simBus.issueZeroes(moduleIndex);
		
		for (int index = 0; index < endAddress; index++)
		{
			Object returnValue = memoryModule.readReg(startAddress
					+ (index * wordBitSize));
			
			assertNotSame(
					"Reading registers after initialization return PLP_ERROR_RETURN.",
					Constants.PLP_ERROR_RETURN, returnValue);
			
			// MemModule read is overrided to Long
			assertEquals("Reading registers after initialization aren't 0.",
					0L, returnValue);
		}
	}
	
}
