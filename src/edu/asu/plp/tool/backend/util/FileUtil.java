package edu.asu.plp.tool.backend.util;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;
import org.apache.commons.io.FileUtils;

import edu.asu.plp.tool.backend.isa.ASMFile;
import edu.asu.plp.tool.prototype.model.SimpleASMFile;

public class FileUtil
{
	public static String readAllLines(String filePath) throws IOException
	{
		return readAllLines(new File(filePath));
	}
	
	public static String readAllLines(File file) throws IOException
	{
		return FileUtils.readFileToString(file);
	}
	
	/**
	 * @deprecated replaced by {@link PLP5ProjectParser#parse(File)}
	 */
	public static List<ASMFile> openProject(File projectFile)
	{
		try (TarArchiveInputStream plpInputStream = new TarArchiveInputStream(
				new FileInputStream(projectFile)))
		{
			List<ASMFile> projectFiles = new ArrayList<>();
			
			TarArchiveEntry entry = plpInputStream.getNextTarEntry();
			while ((entry = plpInputStream.getNextTarEntry()) != null)
			{
				if (!entry.isDirectory())
				{
					addFile(plpInputStream, entry, projectFile, projectFiles);
				}
				else
				{
					addDirectory(plpInputStream, entry, projectFile, projectFiles);
				}
			}
			
			return projectFiles;
		}
		catch (IOException e)
		{
			e.printStackTrace();
			System.exit(-1);
		}
		return null;
	}
	
	/**
	 * @deprecated see {@link PLP5ProjectParser#parse(File)}
	 */
	private static void addFile(TarArchiveInputStream plpInputStream,
			TarArchiveEntry entry, File assembleFile, List<ASMFile> projectFiles)
			throws IOException
	{
		byte[] content = new byte[(int) entry.getSize()];
		int currentIndex = 0;
		while (currentIndex < entry.getSize())
		{
			plpInputStream.read(content, currentIndex, content.length - currentIndex);
			currentIndex++;
		}
		if (entry.getName().endsWith(".asm"))
		{
			ASMFile asmFile = new SimpleASMFile(null, entry.getName());
			asmFile.setContent(new String(content));
			projectFiles.add(asmFile);
		}
	}
	
	/**
	 * @deprecated see {@link PLP5ProjectParser#parse(File)}
	 */
	private static void addDirectory(TarArchiveInputStream plpInputStream,
			TarArchiveEntry entry, File assembleFile, List<ASMFile> projectFiles)
			throws IOException
	{
		for (TarArchiveEntry subEntry : entry.getDirectoryEntries())
		{
			if (!subEntry.isDirectory())
			{
				addFile(plpInputStream, subEntry, assembleFile, projectFiles);
			}
			else
			{
				addDirectory(plpInputStream, subEntry, assembleFile, projectFiles);
			}
		}
	}
	
	// TODO replace with correct external method
	public static boolean isValidProject(File projectFile)
	{
		if (!projectFile.isFile())
			return false;
		if (!projectFile.getPath().endsWith(".plp"))
			return false;
		else
			return true;
	}
	
	// TODO replace with correct external method
	public static boolean isValidFile(File file)
	{
		if (!file.isFile())
			return false;
		if (!file.getPath().endsWith(".asm"))
			return false;
		else
			return true;
	}
	
	public static boolean isValidFile(String filePath)
	{
		return isValidFile(new File(filePath));
	}
}
