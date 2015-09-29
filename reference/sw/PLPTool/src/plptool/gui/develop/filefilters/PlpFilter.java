package plptool.gui.develop.filefilters;

import java.io.File;

import javax.swing.filechooser.FileFilter;

public class PlpFilter extends FileFilter
{
	@Override
	public boolean accept(File file)
	{
		if (file.isDirectory())
			return true;
		
		if (file.getAbsolutePath().endsWith(".plp"))
			return true;
		
		return false;
	}

	@Override
	public String getDescription()
	{
		return "PLP project files";
	}
}
