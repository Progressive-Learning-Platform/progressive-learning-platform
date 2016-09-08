package edu.asu.plp.tool.prototype.model;

import java.util.ArrayList;
import java.util.List;

public class CSSStyle
{
	private StringBuilder inlineCSS;
	private List<String> cssClasses;
	
	public CSSStyle()
	{
		this.inlineCSS = new StringBuilder();
		this.cssClasses = new ArrayList<>();
	}
	
	public void addInlineStyle(String cssString)
	{
		cssString = cssString.trim();
		String sameLineStyles = cssString.replace("\n", "");
		
		if (cssString.length() > 0)
		{
			int lastIndex = sameLineStyles.length() - 1;
			char lastCharacter = sameLineStyles.charAt(lastIndex);
			if (lastCharacter != ';')
				cssString += ';';
			
			inlineCSS.append(cssString);
		}
	}
	
	public void addStyleClass(String cssClass)
	{
		String trimmedClass = cssClass.trim();
		if (trimmedClass.length() > 0)
			cssClasses.add(trimmedClass);
	}
	
	public String compileStyleString()
	{
		return inlineCSS.toString();
	}
	
	public String compileClassesString()
	{
		return String.join(" ", cssClasses);
	}
}
