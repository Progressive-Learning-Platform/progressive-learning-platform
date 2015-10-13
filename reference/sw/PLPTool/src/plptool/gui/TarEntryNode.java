package plptool.gui;

import org.apache.commons.compress.archivers.tar.TarArchiveEntry;

/**
 * Data structure to hold the entry-data and physical-data of a {@link TarArchiveEntry}
 * 
 * @author Moore, Zachary
 *
 */
public class TarEntryNode
{
	public final TarArchiveEntry entry;
	public final byte[] data;
	
	public TarEntryNode(TarArchiveEntry entry, byte[] data)
	{
		if (entry == null || data == null)
			throw new IllegalArgumentException("entry and data must be non-null");
		
		this.entry = entry;
		this.data = data;
	}
}
