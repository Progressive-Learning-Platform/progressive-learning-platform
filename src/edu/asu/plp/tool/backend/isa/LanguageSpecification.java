package edu.asu.plp.tool.backend.isa;

public abstract class LanguageSpecification
{
	private final String languageName;
	private final String languageMajorVersion;
	private final String languageMinorVersion;
	
	public LanguageSpecification(String languageName, int majorVersion, int minorVersion)
	{
		this(languageName, Integer.toString(majorVersion), Integer.toString(minorVersion));
	}
	
	public LanguageSpecification(String languageName, String majorVersion,
			String minorVersion)
	{
		this.languageName = languageName;
		this.languageMajorVersion = majorVersion;
		this.languageMinorVersion = minorVersion;
	}
	
	public String getName()
	{
		return languageName;
	}
	
	public String getMajorVersion()
	{
		return languageMajorVersion;
	}
	
	public String getMinorVersion()
	{
		return languageMinorVersion;
	}
}
