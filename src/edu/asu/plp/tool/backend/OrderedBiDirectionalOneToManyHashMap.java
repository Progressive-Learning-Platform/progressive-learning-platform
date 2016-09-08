package edu.asu.plp.tool.backend;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * A hash-table implementation of {@link BiDirectionalOneToManyMap}, with predictable
 * iteration order. This map maintains a linked list running through it's keys and values
 * to define the iteration ordering of {@link #keySet()} and {@link #valueSet()}.
 * <p>
 * The iteration order over those sets will be the same as the insertion order for the
 * keys and values respectively. Note that keys or values that are re-entered into the map
 * will <b>not</b> have an effect on the ordering.
 * <p>
 * <b>Note that this implementation is not synchronized.</b> If multiple threads access a
 * linked hash map concurrently, and at least one of the threads modifies the map
 * structurally, it <i>must</i> be synchronized externally.
 * 
 * @author Moore, Zachary
 *
 * @param <K>
 *            key type
 * @param <V>
 *            value type
 */
public class OrderedBiDirectionalOneToManyHashMap<K, V> implements
		BiDirectionalOneToManyMap<K, V>
{
	/**
	 * Map of keys to all associated values. If a key is contained in this map, it is
	 * expected to have at least one value associated with it. Keys mapped to an empty
	 * list are not allowed.
	 */
	private Map<K, List<V>> keys;
	
	/**
	 * Map of all values to their corresponding key. Each value must be unique, and is
	 * required to also appear in a list inside {@link #keys}. If a value is contained in
	 * this map, it must have a corresponding key, and it must appear in one of the
	 * value-lists in {@link #keys}
	 */
	private Map<V, K> values;
	
	/**
	 * Creates an empty Map
	 */
	public OrderedBiDirectionalOneToManyHashMap()
	{
		this.keys = new LinkedHashMap<>();
		this.values = new LinkedHashMap<>();
	}
	
	@Override
	public K put(K key, V value)
	{
		if (key.equals(getKey(value)))
			return key;
		
		K oldKey = removeValue(value);
		List<V> associates = keys.get(key);
		if (associates == null)
		{
			associates = new ArrayList<>();
			keys.put(key, associates);
		}
		
		associates.add(value);
		values.put(value, key);
		return oldKey;
	}
	
	@Override
	public boolean remove(K key, V value)
	{
		// TODO: this does not support null keys
		List<V> mappedValues = keys.get(key);
		if (mappedValues == null)
			return false;
		
		K mappedKey = values.get(value);
		
		// If the given key-value pair is in this map
		if (mappedKey != null && mappedKey.equals(key))
		{
			mappedValues.remove(value);
			if (mappedValues.isEmpty())
				keys.remove(key);
			values.remove(value);
		}
		
		return keys.containsKey(key);
	}
	
	@Override
	public List<V> removeKey(K key)
	{
		List<V> mappedValues = keys.get(key);
		if (mappedValues == null)
			return null;
		
		for (V value : mappedValues)
			values.remove(value);
		
		return keys.remove(key);
	}
	
	@Override
	public K removeValue(V value)
	{
		if (!values.containsKey(value))
			return null;
		
		K mappedKey = values.remove(value);
		List<V> mappedValues = keys.get(mappedKey);
		if (mappedValues == null)
			throw new IllegalStateException(
					"Invalid internal key mapping. Expected list to be non-null");
		
		mappedValues.remove(value);
		if (mappedValues.isEmpty())
			keys.remove(mappedKey);
		
		return mappedKey;
	}
	
	@Override
	public boolean containsKey(K key)
	{
		return keys.containsKey(key);
	}
	
	@Override
	public boolean containsValue(V value)
	{
		// containsKey(value) is not a typo. The "key" of the values map is a Value
		return values.containsKey(value);
	}
	
	@Override
	public boolean contains(K key, V value)
	{
		List<V> mappedValues = keys.get(key);
		if (mappedValues == null)
			return false;
		else
			return mappedValues.contains(value);
	}
	
	@Override
	public K getKey(V value)
	{
		return values.get(value);
	}
	
	@Override
	public List<V> get(K key)
	{
		return keys.get(key);
	}
	
	@Override
	public Set<K> keySet()
	{
		return keys.keySet();
	}
	
	@Override
	public Set<V> valueSet()
	{
		return values.keySet();
	}
	
	@Override
	public int size()
	{
		return valueSize();
	}
	
	@Override
	public int keySize()
	{
		return keys.size();
	}
	
	@Override
	public int valueSize()
	{
		return values.size();
	}
	
	@Override
	public void clear()
	{
		keys.clear();
		values.clear();
	}
	
}
