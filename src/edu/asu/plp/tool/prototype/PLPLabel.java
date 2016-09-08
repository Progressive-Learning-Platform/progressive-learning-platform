package edu.asu.plp.tool.prototype;

import java.util.ArrayList;
import java.util.List;

import com.faeysoft.preceptor.lexer.LexException;
import com.faeysoft.preceptor.lexer.Lexer;
import com.faeysoft.preceptor.lexer.Token;
import com.faeysoft.preceptor.lexer.TokenType;
import com.faeysoft.preceptor.lexer.TokenTypeSet;

public class PLPLabel
{
	private String name;
	private int lineNumber;
	
	public PLPLabel(String name, int lineNumber)
	{
		super();
		this.name = name;
		this.lineNumber = lineNumber;
	}
	
	public String getName()
	{
		return name;
	}
	
	public int getLineNumber()
	{
		return lineNumber;
	}
	
	public static List<PLPLabel> scrape(String content)
	{
		TokenTypeSet tokenTypes = new TokenTypeSet();
		// TODO: replace with reference to PLPValueType or call the assembler interface
		tokenTypes.add(new TokenType("[a-zA-Z_\\$][a-zA-Z_\\$0-9]*:", "LABEL"));
		Lexer lexer = new Lexer(tokenTypes);
		
		try
		{
			String[] lines = content.split("\\n\\r?");
			List<PLPLabel> labels = new ArrayList<>();
			int lineNumber = 1;
			
			for (String line : lines)
			{
				List<Token> tokens = lexer.lex(line);
				for (Token token : tokens)
				{
					if ("LABEL".equals(token.getTypeName()))
					{
						String name = token.getValue();
						PLPLabel label = new PLPLabel(name, lineNumber);
						labels.add(label);
					}
				}
				lineNumber++;
			}
			
			return labels;
		}
		catch (LexException e)
		{
			throw new IllegalArgumentException("Invalid file", e);
		}
	}
}
