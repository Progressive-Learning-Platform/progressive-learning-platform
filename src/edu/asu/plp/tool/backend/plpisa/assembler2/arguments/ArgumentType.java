package edu.asu.plp.tool.backend.plpisa.assembler2.arguments;

import java.util.function.Function;

import com.faeysoft.preceptor.lexer.RegexCommons.*;

import edu.asu.plp.tool.backend.plpisa.assembler2.Argument;

public enum ArgumentType
{
	REGISTER("\\$[a-z0-9]+", (source) -> new RegisterArgument(source)),
	MEMORY_LOCATION("[0-9]+\\(\\$[a-z0-9]+\\)", (source) -> new MemoryArgument(source)),
	STRING_LITERAL(PatternLiterals.LITERAL_STRING, (source) -> new StringLiteral(source)),
	NUMBER_LITERAL("(0[xb])?[0-9]+", (source) -> new Value(source)),
	CHAR_LITERAL("'[a-z A-Z0-9]'", (source) -> new CharLiteral(source));
	
	private String regex;
	private Function<String, Argument> parseFunction;
	
	private ArgumentType(String regex, Function<String, Argument> parseFunction)
	{
		this.regex = regex;
		this.parseFunction = parseFunction;
	}
	
	public static Argument parse(String source)
	{
		source = source.trim();
		for (ArgumentType type : ArgumentType.values())
		{
			if (source.matches(type.regex))
				return type.parseFunction.apply(source);
		}
		
		throw new IllegalArgumentException("No match found for: " + source);
	}
}
