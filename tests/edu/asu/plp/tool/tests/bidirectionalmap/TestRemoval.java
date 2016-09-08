package edu.asu.plp.tool.tests.bidirectionalmap;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import edu.asu.plp.tool.backend.BiDirectionalOneToManyMap;
import edu.asu.plp.tool.backend.OrderedBiDirectionalOneToManyHashMap;

public class TestRemoval
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
	
	@Test
	public void testRemoveNonExistentKey()
	{
		
	}
	
	@Test
	public void testRemoveNonExistentPair()
	{
		
	}
	
	@Test
	public void testRemoveExistingKeyNonExistentValue()
	{
		
	}
	
	@Test
	public void testRemoveNonExistingKeyExistentValue()
	{
		
	}
	
	@Test
	public void testRemoveUniqueKeySingleValue()
	{
		
	}
	
	@Test
	public void testRemoveUniqueKeyMultiValue()
	{
		
	}
	
	@Test
	public void testRemoveValue()
	{
		
	}
	
}
