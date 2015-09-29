package plptool.gui.develop.filefilters;

import java.io.File;

import javax.swing.filechooser.FileFilter;

public class AsmFilter extends FileFilter
{
	@Override
	public boolean accept(File file)
	{
		if (file.isDirectory())
			return true;
		
		if (file.getAbsolutePath().endsWith(".asm"))
			return true;
		
		return false;
	}

	@Override
	public String getDescription()
	{
		return ".ASM files";
	}
}
