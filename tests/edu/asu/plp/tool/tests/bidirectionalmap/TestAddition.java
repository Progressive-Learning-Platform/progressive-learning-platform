package edu.asu.plp.tool.tests.bidirectionalmap;

import static org.junit.Assert.*;

import java.util.List;
import java.util.Set;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import edu.asu.plp.tool.backend.BiDirectionalOneToManyMap;
import edu.asu.plp.tool.backend.OrderedBiDirectionalOneToManyHashMap;

public class TestAddition
{
	private BiDirectionalOneToManyMap<String, String> map;
	
	@Before
	public void setUp()
	{
		map = new OrderedBiDirectionalOneToManyHashMap<>();
	}
	
	@After
	public void tearDown()
	{
		map = null;
	}
	
	/*
	 * Put Tests
	 */
	@Test
	public void testNonNullKeyToNullValue()
	{
		String key = "key";
		map.put(key, null);
		assertTrue(map.contains(key, null));
		assertTrue(map.containsKey(key));
		assertTrue(map.containsValue(null));
		
		List<String> mappedValues = map.get(key);
		assertTrue(mappedValues.contains(null));
	}
	
	@Test
	public void testNullKeyToNonNullValue()
	{
		String key = null;
		String value = "value";
		
		map.put(key, value);
		assertTrue(map.contains(key, value));
		assertTrue(map.containsKey(key));
		assertTrue(map.containsValue(value));
		
		List<String> mappedValues = map.get(key);
		assertTrue(mappedValues.contains(value));
	}
	
	@Test
	public void testNullKeyToNullValue()
	{
		String key = null;
		String value = null;
		
		map.put(key, value);
		assertTrue(map.contains(key, value));
		assertTrue(map.containsKey(key));
		assertTrue(map.containsValue(value));
		
		List<String> mappedValues = map.get(key);
		assertTrue(mappedValues.contains(value));
	}
	
	@Test
	public void testNonNullKeyToNonNullValue()
	{
		String key = "key";
		String value = "value";
		
		map.put(key, value);
		assertTrue(map.contains(key, value));
		assertTrue(map.containsKey(key));
		assertTrue(map.containsValue(value));
		
		List<String> mappedValues = map.get(key);
		assertTrue(mappedValues.contains(value));
	}
	
	@Test
	public void testInsertSingleKeyMultiValues()
	{
		String key = "firstKey";
		String[] values = new String[] { "first value", "secondValue", "value3" };
		
		for (String value : values)
			map.put(key, value);
		
		for (String value : values)
			assertTrue(map.contains(key, value));
		
		for (String value : values)
			assertTrue(map.containsValue(value));
		
		List<String> mappedValues = map.get(key);
		assertEquals(values.length, mappedValues.size());
		for (String value : values)
			assertTrue(mappedValues.contains(value));
	}
	
	/**
	 * Values must be unique. As such, the expected behaviour is that the first key to be
	 * associated with the value will be overwritten by the second.
	 */
	@Test
	public void testUniqueKeysSameValue()
	{
		String key1 = "firstKey";
		String key2 = "secondKey";
		String value = "value";
		
		map.put(key1, value);
		map.put(key2, value);
		assertTrue(map.contains(key2, value));
		assertFalse(map.contains(key1, value));
	}
	
	@Test
	public void testUniqueKeysSameValue_ReturnsOldKey()
	{
		String key1 = "firstKey";
		String key2 = "secondKey";
		String value = "value";
		
		String ret1 = map.put(key1, value);
		String ret2 = map.put(key2, value);
		
		assertEquals(null, ret1);
		assertEquals(key1, ret2);
	}
	
	@Test
	public void testUniqueKeysSameValue_EmptyKeyIsRemoved()
	{
		String key1 = "firstKey";
		String key2 = "secondKey";
		String value = "value";
		
		map.put(key1, value);
		map.put(key2, value);
		
		assertTrue(map.containsValue(value));
		assertTrue(map.containsKey(key2));
		assertFalse(map.containsKey(key1));
	}
	
	@Test
	public void testUniqueKeysSameValue_MultiValuedKeyIsNotRemoved()
	{
		String key1 = "firstKey";
		String key2 = "secondKey";
		String value = "value";
		String value2 = "value2";
		
		map.put(key1, value);
		map.put(key1, value2);
		map.put(key2, value);
		
		assertTrue(map.containsValue(value));
		assertTrue(map.containsKey(key2));
		assertTrue(map.containsKey(key1));
		assertTrue(map.contains(key2, value));
		assertTrue(map.contains(key1, value2));
		assertFalse(map.contains(key1, value));
	}
	
	/*
	 * Size Tests
	 */
	@Test
	public void testSingleKeyMultiValues_ValuesSize()
	{
		map.put("key1", "value1-1");
		map.put("key1", "value1-2");
		map.put("key1", "value1-3");
		
		assertEquals(3, map.valueSize());
	}
	
	@Test
	public void testSingleKeyMultiValues_KeySize()
	{
		map.put("key1", "value1-1");
		map.put("key1", "value1-2");
		map.put("key1", "value1-3");
		map.put("key1", "value1-4");
		
		assertEquals(1, map.keySize());
	}
	
	@Test
	public void testSingleKeyMultiValues_MapSize()
	{
		map.put("key1", "value1-1");
		map.put("key1", "value1-2");
		map.put("key1", "value1-3");
		map.put("key1", "value1-4");
		
		assertEquals(4, map.size());
	}
	
	@Test
	public void testMultiKeyMultiValuesSymetric_ValueSize()
	{
		map.put("key1", "value1-1");
		map.put("key1", "value1-2");
		map.put("key1", "value1-3");
		
		map.put("key2", "value2-1");
		map.put("key2", "value2-2");
		map.put("key2", "value2-3");
		
		assertEquals(6, map.valueSize());
	}
	
	@Test
	public void testMultiKeyMultiValuesAsymetricLeft_ValueSize()
	{
		map.put("key1", "value1-1");
		map.put("key1", "value1-2");
		map.put("key1", "value1-3");
		map.put("key1", "value1-4");
		
		map.put("key2", "value2-1");
		map.put("key2", "value2-2");
		map.put("key2", "value2-3");
		
		assertEquals(7, map.valueSize());
	}
	
	@Test
	public void testMultiKeyMultiValuesAsymetricRight_ValueSize()
	{
		map.put("key1", "value1-1");
		map.put("key1", "value1-2");
		map.put("key1", "value1-3");
		
		map.put("key2", "value2-1");
		map.put("key2", "value2-2");
		map.put("key2", "value2-3");
		map.put("key2", "value1-4");
		
		assertEquals(7, map.valueSize());
	}
	
	@Test
	public void testMultiKeyMultiValues_KeySize()
	{
		map.put("key1", "value1-1");
		map.put("key1", "value1-2");
		map.put("key1", "value1-3");
		map.put("key1", "value1-4");
		
		map.put("key2", "value2-1");
		map.put("key2", "value2-2");
		map.put("key2", "value2-3");
		
		assertEquals(2, map.keySize());
	}
	
	@Test
	public void testMultiKeyMultiValues_MapSize()
	{
		map.put("key1", "value1-1");
		map.put("key1", "value1-2");
		map.put("key1", "value1-3");
		map.put("key1", "value1-4");
		
		map.put("key2", "value2-1");
		map.put("key2", "value2-2");
		map.put("key2", "value2-3");
		
		assertEquals(7, map.size());
	}
	
	@Test
	public void testSingleKeySingleValue_ValueSize()
	{
		map.put("key", "value");
		
		assertEquals(1, map.valueSize());
	}
	
	@Test
	public void testSingleKeySingleValue_MapSize()
	{
		map.put("key", "value");
		
		assertEquals(1, map.size());
	}
	
	@Test
	public void testSingleKeySingleValue_KeySize()
	{
		map.put("key", "value");
		
		assertEquals(1, map.keySize());
	}
	
	@Test
	public void testEmptyMap_Sizes()
	{
		assertEquals(0, map.keySize());
		assertEquals(0, map.valueSize());
		assertEquals(0, map.size());
	}
	
	@Test
	public void testEmptyMap_SetSizes()
	{
		Set<String> keySet = map.keySet();
		Set<String> valueSet = map.valueSet();
		
		assertTrue(keySet.isEmpty());
		assertTrue(valueSet.isEmpty());
		assertEquals(0, keySet.size());
		assertEquals(0, valueSet.size());
	}
	
	/*
	 * Contains Tests
	 */
	
	/*
	 * Get Tests
	 */
	
	/*
	 * Key/Value Set Tests
	 */
	
}
