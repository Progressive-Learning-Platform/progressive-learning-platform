package edu.asu.plp.tool.backend.plpisa;

import edu.asu.plp.tool.backend.PLP;
import edu.asu.plp.tool.backend.isa.LanguageSpecification;

public class PLPLanguageSpecification extends LanguageSpecification
{

	public PLPLanguageSpecification()
	{
		super(PLP.NAME, PLP.MAJOR_VERSION, PLP.MINOR_VERSION);
	}
}
