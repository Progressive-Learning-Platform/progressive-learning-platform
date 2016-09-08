package edu.asu.plp.tool.prototype;

import edu.asu.plp.tool.backend.isa.ASMFile;
import edu.asu.plp.tool.backend.isa.ASMImage;
import edu.asu.plp.tool.prototype.model.Project;

public class ProjectAssemblyDetails
{
	private boolean isDirty;
	private ASMImage assembledImage;
	
	public ProjectAssemblyDetails(Project project)
	{
		this.isDirty = true;
		this.assembledImage = null;
		
		// When the project source changes, mark this project as dirty
		for (ASMFile file : project)
		{
			file.contentProperty().addListener((value, current, old) -> setDirty());
		}
	}
	
	public boolean isDirty()
	{
		return isDirty;
	}
	
	public void setDirty()
	{
		this.isDirty = true;
	}
	
	public ASMImage getAssembledImage()
	{
		return assembledImage;
	}
	
	public void setAssembledImage(ASMImage assembledImage)
	{
		this.assembledImage = assembledImage;
		this.isDirty = false;
	}
	
}
