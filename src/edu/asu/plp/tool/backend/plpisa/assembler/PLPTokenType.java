package edu.asu.plp.tool.backend.plpisa.assembler;

import com.faeysoft.preceptor.lexer.TokenType;
import com.faeysoft.preceptor.lexer.TokenTypeSet;

public enum PLPTokenType
{
	//Foundation
	//TODO make label colon have nothing after it
	LABEL_COLON("\\b([a-zA-Z]([a-zA-Z]|_|[0-9])*([a-zA-Z]|[0-9])+):\\B"),
	INSTRUCTION("\\b[a-zA-Z]+\\b"),
	LABEL_PLAIN("\\b([a-zA-Z]([a-zA-Z]|_|[0-9])*([a-zA-Z]|[0-9])+)\\b"),
	ADDRESS("\\$([a-zA-Z]|[0-9])+"),
	COMMENT("#.*$"),
	COMMA(","),
	PARENTHESIS_ADDRESS("\\(\\$([a-zA-Z]|[0-9])+\\)"),
	DIRECTIVE("\\B\\.(.+?)\\b"),
	STRING("\"(.*?)\""),
	NUMERIC("\\b((\\d)+|0x([0-9a-fA-F]+))\\b"),
	NEW_LINE("^\\s*$");
	//TODO include .include, .text, .data
	
	private String regex;
	
	private PLPTokenType(String regex)
	{
		this.regex = regex;
	}
	
	public String regex()
	{
		return regex;
	}
	
	public static TokenTypeSet createSet()
	{
		TokenTypeSet set = new TokenTypeSet();
		
		for (PLPTokenType type : PLPTokenType.values())
		{
			TokenType tokenType = new TokenType(type.regex, type.name());
			set.add(tokenType);
		}
		
		return set;
	}
	
	@Override
	public String toString()
	{
		return super.toString().toLowerCase();
	}
}
