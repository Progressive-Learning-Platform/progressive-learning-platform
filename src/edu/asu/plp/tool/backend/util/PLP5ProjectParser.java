package edu.asu.plp.tool.backend.util;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;
import org.apache.commons.io.FilenameUtils;

import edu.asu.plp.tool.backend.isa.ASMFile;
import edu.asu.plp.tool.prototype.model.PLPProject;
import edu.asu.plp.tool.prototype.model.SimpleASMFile;

public class PLP5ProjectParser
{
	private PLPProject project;
	private TarArchiveInputStream inputStream;
	
	public PLPProject parse(File projectFile) throws IOException
	{
		String name = projectFile.getName();
		name = FilenameUtils.removeExtension(name);
		String type = "plp5";
		
		project = new PLPProject(name, type);
		project.setPath(projectFile.getAbsolutePath());
		
		extract(projectFile);
		return project;
	}

	private void extract(File projectFile) throws IOException
	{
		try (FileInputStream fileStream = new FileInputStream(projectFile);
				TarArchiveInputStream inputStream = new TarArchiveInputStream(fileStream))
		{
			this.inputStream = inputStream;
			
			// TODO: verify the first entry is not relevant
			TarArchiveEntry entry = inputStream.getNextTarEntry();
			while ((entry = inputStream.getNextTarEntry()) != null)
			{
				if (!entry.isDirectory())
				{
					addFile(entry);
				}
				else
				{
					addDirectory(entry);
				}
			}
		}
		catch (IOException exception)
		{
			throw exception;
		}
	}
	
	private void addFile(TarArchiveEntry entry) throws IOException
	{
		byte[] content = new byte[(int) entry.getSize()];
		int currentIndex = 0;
		while (currentIndex < entry.getSize())
		{
			inputStream.read(content, currentIndex, content.length - currentIndex);
			currentIndex++;
		}
		if (entry.getName().endsWith(".asm"))
		{
			ASMFile asmFile = new SimpleASMFile(project, entry.getName());
			asmFile.setContent(new String(content));
			
			project.add(asmFile);
		}
	}
	
	private void addDirectory(TarArchiveEntry entry) throws IOException
	{
		for (TarArchiveEntry subEntry : entry.getDirectoryEntries())
		{
			if (!subEntry.isDirectory())
			{
				addFile(subEntry);
			}
			else
			{
				addDirectory(subEntry);
			}
		}
	}
}
