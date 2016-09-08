package edu.asu.plp.tool.prototype.model;

import com.google.common.base.Joiner;

import java.util.*;

/**
 * Default implementation of
 * {@link OptionSection}. Used in {@link edu.asu.plp.tool.prototype.view.menu.options.OptionsPane}.
 * <p>
 * Implements basic list functionality so every element acts as a parent which can have children (and those children
 * can be parents and have their own children etc). Implements entire List interface without extending List.
 * <p>
 * Paths are period separated.
 * <p>
 * Created by Nesbitt, Morgan on 2/28/2016.
 */
public class PLPOptions implements OptionSection
{
	List<OptionSection> children;
	private String name;
	private String fullPath;

	public PLPOptions( String name )
	{
		if ( name == null )
			throw new IllegalArgumentException("PLPOption name cannot be null.");

		this.name = name;
		this.fullPath = name;
		this.children = new ArrayList<>();
	}

	@Override
	public String getName()
	{
		return name;
	}

	@Override
	public String getFullPath()
	{
		return fullPath;
	}

	@Override
	public void setParent( String parentPath )
	{
		if ( parentPath != null && !parentPath.isEmpty() )
			this.fullPath = Joiner.on(".").join(parentPath, name);
		else
			this.fullPath = name;

		if ( children.size() > 0 )
			children.forEach(child -> child.setParent(getFullPath()));
	}

	@Override
	public int size()
	{
		return children.size();
	}

	@Override
	public boolean isEmpty()
	{
		return children.isEmpty();
	}

	@Override
	public boolean contains( Object o )
	{
		return children.contains(o);
	}

	@Override
	public Iterator<OptionSection> iterator()
	{
		return children.iterator();
	}

	@Override
	public Object[] toArray()
	{
		return children.toArray();
	}

	@Override
	public <T> T[] toArray( T[] a )
	{
		return children.toArray(a);
	}

	@Override
	public boolean add( OptionSection optionSection )
	{
		optionSection.setParent(getFullPath());
		return children.add(optionSection);
	}

	@Override
	public boolean remove( Object o )
	{
		return children.remove(o);
	}

	@Override
	public boolean containsAll( Collection<?> c )
	{
		return children.containsAll(c);
	}

	@Override
	public boolean addAll( Collection<? extends OptionSection> c )
	{
		c.forEach(item -> item.setParent(getFullPath()));
		return children.addAll(c);
	}

	@Override
	public boolean addAll( int index, Collection<? extends OptionSection> c )
	{
		c.forEach(item -> item.setParent(getFullPath()));
		return children.addAll(index, c);
	}

	@Override
	public boolean removeAll( Collection<?> c )
	{
		return children.removeAll(c);
	}

	@Override
	public boolean retainAll( Collection<?> c )
	{
		return children.retainAll(c);
	}

	@Override
	public void clear()
	{
		children.clear();
	}

	@Override
	public OptionSection get( int index )
	{
		return children.get(index);
	}

	@Override
	public OptionSection set( int index, OptionSection element )
	{
		return children.set(index, element);
	}

	@Override
	public void add( int index, OptionSection element )
	{
		element.setParent(getFullPath());
		children.add(index, element);
	}

	@Override
	public OptionSection remove( int index )
	{
		return children.remove(index);
	}

	@Override
	public int indexOf( Object o )
	{
		return children.indexOf(o);
	}

	@Override
	public int lastIndexOf( Object o )
	{
		return children.indexOf(o);
	}

	@Override
	public ListIterator<OptionSection> listIterator()
	{
		return children.listIterator();
	}

	@Override
	public ListIterator<OptionSection> listIterator( int index )
	{
		return children.listIterator(index);
	}

	@Override
	public List<OptionSection> subList( int fromIndex, int toIndex )
	{
		return children.subList(fromIndex, toIndex);
	}

	@Override
	public String toString()
	{
		return fullPath;
	}
}
