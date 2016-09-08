package edu.asu.plp.tool.backend.plpisa.sim;

import java.util.HashMap;
import java.util.Map;

import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;

public class PLPMemoryModule implements MemoryModule32Bit
{
	public static final int NUMBER_OF_REGISTERS = 32;
	private static final int DEFAULT_MEMORY_VALUE = 0;
	
	private IntegerProperty[] registers;
	private Map<String, Integer> namedRegisters;
	private Map<Integer, IntegerProperty> memoryLocations;
	
	public PLPMemoryModule()
	{
		this.registers = new IntegerProperty[NUMBER_OF_REGISTERS];
		this.namedRegisters = buildNamedRegistersMap();
		this.memoryLocations = new HashMap<>();
	}
	
	private String normalizeRegisterName(String registerName)
	{
		if (registerName == null)
		{
			return null;
		}
		else if (registerName.startsWith("$"))
		{
			return registerName.substring(1);
		}
		else
		{
			return registerName;
		}
	}
	
	private int convertNameToIndex(String registerName)
	{
		registerName = normalizeRegisterName(registerName);
		Integer mappedIndex = namedRegisters.get(registerName);
		try
		{
			return (mappedIndex != null) ? mappedIndex : Integer.parseInt(registerName);
		}
		catch (Exception e)
		{
			return -1;
		}
	}
	
	@Override
	public boolean hasRegister(String registerName)
	{
		int index = convertNameToIndex(registerName);
		return registerIndexIsValid(index);
	}
	
	private boolean registerIndexIsValid(int index)
	{
		return index >= 0 && index < registers.length;
	}
	
	@Override
	public IntegerProperty getRegisterValueProperty(String registerName)
	{
		int index = convertNameToIndex(registerName);
		if (registerIndexIsValid(index))
			return registers[index];
		else
			return null;
	}
	
	@Override
	public String getRegisterID(String registerName)
	{
		int index = convertNameToIndex(registerName);
		if (registerIndexIsValid(index))
			return "$" + index;
		else
			return null;
	}
	
	@Override
	public IntegerProperty getMemoryValueProperty(int address)
	{
		validateAddress(address);
		IntegerProperty memoryValueProperty = memoryLocations.get(address);
		if (memoryValueProperty == null)
		{
			memoryValueProperty = new SimpleIntegerProperty(DEFAULT_MEMORY_VALUE);
			memoryLocations.put(address, memoryValueProperty);
		}
		
		return memoryValueProperty;
	}
	
	/**
	 * Throws an {@link IllegalArgumentException} if the given address is not word-aligned
	 * (32-bit words).
	 * <p>
	 * That is, the given address must be divisible by 4.
	 * <p>
	 * Note that PLP memory addresses are not signed, so the address cannot be out of
	 * range.
	 * 
	 * @see MemoryModule32Bit#validateAddress(int)
	 */
	@Override
	public void validateAddress(int address)
	{
		if ((address % 4) != 0)
			throw new IllegalArgumentException("Address must be word-aligned");
	}
	
	private Map<String, Integer> buildNamedRegistersMap()
	{
		// TODO: This is sloppy. Consider reading from JSON?
		Map<String, Integer> map = new HashMap<>();
		map.put("zero", 0);
		map.put("at", 1);
		map.put("v0", 2);
		map.put("v1", 3);
		map.put("a0", 4);
		map.put("a1", 5);
		map.put("a2", 6);
		map.put("a3", 7);
		map.put("t0", 8);
		map.put("t1", 9);
		map.put("t2", 10);
		map.put("t3", 11);
		map.put("t4", 12);
		map.put("t5", 13);
		map.put("t6", 14);
		map.put("t7", 15);
		map.put("t8", 16);
		map.put("t9", 17);
		map.put("s0", 18);
		map.put("s1", 19);
		map.put("s2", 20);
		map.put("s3", 21);
		map.put("s4", 22);
		map.put("s5", 23);
		map.put("s6", 24);
		map.put("s7", 25);
		map.put("i0", 26);
		map.put("i1", 27);
		map.put("iv", 28);
		map.put("sp", 29);
		map.put("ir", 30);
		map.put("ra", 31);
		return map;
	}
}
