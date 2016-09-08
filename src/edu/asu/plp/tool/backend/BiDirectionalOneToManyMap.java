package edu.asu.plp.tool.backend;

import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * An object that maps keys to values. Unlike {@link Map}, this class represents a
 * one-to-many mapping with unique keys <b>and</b> values.
 * 
 * @author Moore, Zachary
 *
 * @param <K>
 *            key type
 * @param <V>
 *            value type
 */
public interface BiDirectionalOneToManyMap<K, V>
{
	/**
	 * Add a key-value pair to this map. Each key may be associated with multiple values,
	 * but each value may be associated with only one key.
	 * <p>
	 * If the key is not already in this map, it will be added, and associated with the
	 * given value. Otherwise, the value will be associated with the pre-existing key.
	 * <p>
	 * If the given value is already associated with a different key, the existing
	 * key-value pair will be removed as specified by {@link #remove(Object, Object)}, and
	 * the new key-value pair will be associated. In this case, the old key will be
	 * returned. Otherwise, null will be returned.
	 * 
	 * @param key
	 *            Key object
	 * @param value
	 *            Value to be associated with the given key.
	 * @return The key previously associated with the specified value, or null if the
	 *         value was not previously in this map.
	 */
	public K put(K key, V value);
	
	/**
	 * Removes the key-value pair from this map and returns true if the given key is still
	 * a member of this map after the pair was removed.
	 * <p>
	 * If the given value is the only value associated with the key, then the key will
	 * also be removed from this map.
	 * 
	 * @param key
	 *            Key object
	 * @param value
	 *            Value object
	 * @return True if the key is still a member of this map, false otherwise
	 */
	public boolean remove(K key, V value);
	
	/**
	 * Removes the specified key from this map, and returns the values it was associated
	 * with. The associated values will also be removed.
	 * 
	 * @param key
	 *            Key object to remove
	 * @return The values associated with the given key, or null if the key was not
	 *         contained by this map.
	 */
	public List<V> removeKey(K key);
	
	/**
	 * Removes the specified value from this map, and returns the key it was associated
	 * with. If this value is the only value that the key was mapped to, then the key will
	 * also be removed.
	 * 
	 * @param value
	 *            Value object to remove
	 * @return The key associated with the given value, or null if the value was not
	 *         contained by this map.
	 */
	public K removeValue(V value);
	
	/**
	 * Returns true if the given key is contained in this map. Note that for a key to be
	 * contained by a map, it must be associated with at least one value.
	 * 
	 * @param key
	 *            Key object
	 * @return True if the key is contained by this map, and associated with any number of
	 *         values.
	 */
	public boolean containsKey(K key);
	
	/**
	 * @param value
	 *            Value object
	 * @return True if the value is contained by this map, and associated with any key.
	 */
	public boolean containsValue(V value);
	
	/**
	 * Returns true if this map contains the specified key-value pair. Note that both the
	 * key and the value must be contained in this map, and associated with each other.
	 * Returns false otherwise.
	 * 
	 * @param key
	 *            Key object
	 * @param value
	 *            Value object
	 * @return True if the key-value pair is contained in this map. False otherwise
	 */
	public boolean contains(K key, V value);
	
	/**
	 * Returns the key associated with the specified value, or null if the value is not in
	 * this map.
	 * 
	 * @param value
	 *            Value object
	 * @return The key associated with the specified value, or null if the value is not in
	 *         this map
	 */
	public K getKey(V value);
	
	/**
	 * Returns a list of values mapped from the given key.
	 * 
	 * @param key
	 *            Key object
	 * @return A list of values associated with the specified key
	 */
	public List<V> get(K key);
	
	/**
	 * @return A set of all keys contained in this map
	 */
	public Set<K> keySet();
	
	/**
	 * @return A set of all values contained in this map
	 */
	public Set<V> valueSet();
	
	/**
	 * Returns the number of key-value pairs contained in this map.
	 * <p>
	 * Since each key may be mapped to multiple values, each value may have only one key,
	 * and values must be unique, the number of key-value pairs of this map is equal to
	 * the number of values contained in the map.
	 * 
	 * @return the number of key-value pairs contained in this map
	 * @see #valueSize()
	 */
	public int size();
	
	/**
	 * Returns the number of keys contained in this map.
	 * <p>
	 * While this behaviour can be achieved by {@link #keySet()}.size(), this method is
	 * generally preferred.
	 * 
	 * @return the number of keys contained in this map
	 */
	public int keySize();
	
	/**
	 * Returns the number of values contained in this map.
	 * <p>
	 * Note that this method is equivalent to {@link #size()}
	 * 
	 * @return the number of values contained in this map
	 * @see #size()
	 */
	public int valueSize();
	
	/**
	 * @return True if there are no key-value pairs in this map. False otherwise
	 */
	default boolean isEmpty()
	{
		return size() == 0;
	}
	
	/**
	 * Removes all key-value pairs in this map.
	 */
	public void clear();
}
