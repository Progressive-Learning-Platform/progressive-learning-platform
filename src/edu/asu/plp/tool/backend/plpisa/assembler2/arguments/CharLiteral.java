package edu.asu.plp.tool.backend.plpisa.assembler2.arguments;

import edu.asu.plp.tool.backend.plpisa.assembler2.Argument;

public class CharLiteral implements Argument
{
	private String rawValue;
	
	public CharLiteral(String rawValue)
	{
		this.rawValue = rawValue;
	}

	@Override
	public int encode()
	{
		// exclude the quotes (first and last character) in value
		int lastIndex = rawValue.length() - 1;
		String value = rawValue.substring(1, lastIndex);
		
		// TODO Auto-generated method stub return 0;
		throw new UnsupportedOperationException("The method is not implemented yet.");
	}
	
	@Override
	public String raw()
	{
		// TODO Auto-generated method stub return null;
		throw new UnsupportedOperationException("The method is not implemented yet.");
	}

	@Override
	public ArgumentType getType()
	{
		// TODO Auto-generated method stub return null;
		throw new UnsupportedOperationException("The method is not implemented yet.");
	}
}
