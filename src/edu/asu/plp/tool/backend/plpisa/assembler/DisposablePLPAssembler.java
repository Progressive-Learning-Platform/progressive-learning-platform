package edu.asu.plp.tool.backend.plpisa.assembler;

import static org.junit.Assert.assertTrue;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;
import java.util.logging.Logger;

import javafx.util.Pair;

import org.apache.commons.io.FileUtils;

import com.faeysoft.preceptor.lexer.LexException;
import com.faeysoft.preceptor.lexer.Lexer;
import com.faeysoft.preceptor.lexer.Token;

import edu.asu.plp.tool.backend.BiDirectionalOneToManyMap;
import edu.asu.plp.tool.backend.isa.ASMDisassembly;
import edu.asu.plp.tool.backend.isa.ASMFile;
import edu.asu.plp.tool.backend.isa.ASMImage;
import edu.asu.plp.tool.backend.isa.ASMInstruction;
import edu.asu.plp.tool.backend.isa.exceptions.AssemblerException;
import edu.asu.plp.tool.backend.isa.exceptions.AssemblyException;
import edu.asu.plp.tool.backend.plpisa.PLPASMImage;
import edu.asu.plp.tool.backend.plpisa.PLPInstruction;
import edu.asu.plp.tool.backend.util.ISAUtil;
import edu.asu.plp.tool.prototype.model.SimpleASMFile;

/**
 * Port (Relatively Cleaner) of old PLP-Tool Assembler.
 *
 * @author Morgan Nesbitt, Zach Moore
 */
public class DisposablePLPAssembler
{
	private static final Logger LOG = Logger.getLogger(PLPAssembler.class.getName());
	private List<ASMFile> asmFiles;
	
	private BiDirectionalOneToManyMap<ASMInstruction, ASMDisassembly> assemblyToDisassemblyMap;
	
	private HashMap<String, Pair<AssemblerStep, Integer>> instructionMap;
	private HashMap<String, AssemblerStep> directiveMap;
	private HashMap<String, AssemblerStep> pseudoOperationMap;
	
	private HashMap<String, Long> symbolTable;
	private HashMap<Integer, Pair<Integer, Integer>> lineNumAndAsmFileMap;
	private List<String> instructionList;
	private HashMap<String, Integer> instructionOpcodeMap;
	protected HashMap<String, Byte> registerMap;
	
	private int mapperIndex;
	private int asmIndex;
	
	private long currentAddress;
	private long currentTextAddress;
	private long currentDataAddress;
	private long byteSpace;
	private long entryPoint;
	
	private int directiveOffset;
	private int currentRegion;
	private int lineNumber;
	
	private String currentActiveFile;
	private String topLevelFile;
	
	private StringBuilder firstPassString;
	
	private static final String ASM__WORD__ = "ASM__WORD__";
	private static final String ASM__ORG__ = "ASM__ORG__";
	private static final String ASM__SKIP__ = "ASM__SKIP__";
	
	private static final String ASM__HIGH__ = "$_hi:";
	private static final String ASM__LOW__ = "$_lo:";
	
	private Lexer lexer;
	// Map position in asmFile list to that files tokens
	private HashMap<ASMFile, List<Token>> asmToTokensMap;
	private ListIterator<Token> tokenIterator;
	private Token currentToken;
	
	public DisposablePLPAssembler(String asmFilePath) throws IOException
	{
		this(Arrays.asList(readASM(asmFilePath)));
	}
	
	private static ASMFile readASM(String asmFilePath) throws IOException
	{
		File file = new File(asmFilePath);
		String fileContent = FileUtils.readFileToString(file);
		ASMFile asmFile = new SimpleASMFile(null, file.getName());
		asmFile.setContent(fileContent);
		return asmFile;
	}
	
	public DisposablePLPAssembler(List<ASMFile> asmFiles)
	{
		this.asmFiles = asmFiles;
		validateArgument(asmFiles);
		initialize();
	}
	
	private void validateArgument(List<ASMFile> asmFiles2)
	{
		if (asmFiles == null)
		{
			String message = "ASM File list must be non-null";
			throw new IllegalArgumentException(message);
		}
		else if (asmFiles.isEmpty())
		{
			String message = "ASM File list is empty";
			throw new IllegalArgumentException(message);
		}
	}
	
	public ASMImage assemble() throws AssemblerException
	{
		assemblyToDisassemblyMap = null;
		asmToTokensMap = new HashMap<>();
		
		if (asmFiles.isEmpty())
			throw new AssemblerException("Can not assemble an image with no files.");
		
		System.out.println("Entered PLPAssembler");
		
		for (ASMFile asmFile : asmFiles)
		{
			try
			{
				System.out.println("Starting lexing of " + asmFile.getName());
				String fileContent = asmFile.getContent();
				String[] lines = fileContent.split("\\r?\\n");
				asmToTokensMap.put(asmFile, lexer.lex(Arrays.asList(lines)));
			}
			catch (LexException exception)
			{
				throw new AssemblerException(exception);
			}
		}
		
		if (asmToTokensMap == null)
			throw new AssemblerException("File was not lexed correctly.");
		
		currentActiveFile = topLevelFile;
		
		System.out.println("\nStarting Preprocess");
		preprocess();
		
		System.out.println("\nStarting Image Assembling");
		
		ASMImage image = null;
		try
		{
			image = assembleImage();
		}
		catch (AssemblyException e)
		{
			e.printStackTrace();
		}
		
		return image;
	}
	
	private void attemptInstructionsMapping(String[] asmLines)
	{
		List<String> lines = new LinkedList<>();
		Collections.addAll(lines, asmLines);
		
		for (Iterator<String> iterator = lines.iterator(); iterator.hasNext();)
		{
			String line = iterator.next();
			
			if (line.startsWith(ASM__SKIP__) || line.startsWith(ASM__WORD__)
					|| line.startsWith(ASM__ORG__))
				iterator.remove();
			else if (line.equals("sll $0, $0, 0"))
				lines.set(lines.indexOf(line), "nop");
		}
		instructionList.clear();
		instructionList.addAll(lines);
	}
	
	private ASMImage assembleImage() throws AssemblerException, AssemblyException
	{
		long assemblerPCAddress = 0;
		// Assembler directive line offsets (skips)
		int assemblerDirectiveSkips = 0;
		currentRegion = 0;
		
		// FIXME: Should the delimiter match multiple commas?
		String delimiters = "[ ,\t]+|[()]";
		
		String currentPreprocessedAsm = firstPassString.toString();
		// System.out.println(currentPreprocessedAsm);
		String[] asmLines = currentPreprocessedAsm.split("\\r?\\n");
		String[] asmTokens;
		
		attemptInstructionsMapping(asmLines);
		
		long[] objectCode = new long[asmLines.length - directiveOffset];
		long[] addressTable = new long[asmLines.length - directiveOffset];
		int[] entryType = new int[asmLines.length - directiveOffset];
		currentActiveFile = topLevelFile;
		System.out.println("Starting assembling of: " + currentActiveFile);
		
		int asmLineIndex = 0;
		while (asmLineIndex < asmLines.length)
		{
			// TODO set file index
			// TODO set current file path
			// TODO set line number
			boolean isSkippable = false;
			asmTokens = asmLines[asmLineIndex].split(delimiters);
			
			// Resolving ASM__HIGH__ and ASM__LOW__ insertions
			String symbolResolver;
			int symbolResolverValue = 0;
			
			for (int tokenIndex = 0; tokenIndex < asmTokens.length; tokenIndex++)
			{
				if (asmTokens[tokenIndex].startsWith(ASM__HIGH__))
				{
					symbolResolver = asmTokens[tokenIndex + 1];
					if (symbolTable.containsKey(symbolResolver))
					{
						symbolResolverValue = (int) (symbolTable.get(symbolResolver) >> 16);
					}
					else
					{
						symbolResolverValue = (int) (ISAUtil
								.sanitize32bits(symbolResolver) >> 16);
					}
					asmTokens[tokenIndex] = String.valueOf(symbolResolverValue);
				}
				else if (asmTokens[tokenIndex].startsWith(ASM__LOW__))
				{
					symbolResolver = asmTokens[tokenIndex + 1];
					if (symbolTable.containsKey(symbolResolver))
						symbolResolverValue = (int) (symbolTable.get(symbolResolver) & 0xFFFF);
					else
						symbolResolverValue = (int) (ISAUtil
								.sanitize32bits(symbolResolver) & 0xFFFF);
					asmTokens[tokenIndex] = String.valueOf(symbolResolverValue);
				}
			}
			
			int instructionType = instructionMap.get(asmTokens[0]).getValue();
			
			if (instructionType < 10)
			{
				objectCode[asmLineIndex - assemblerDirectiveSkips] = 0;
				entryType[asmLineIndex - assemblerDirectiveSkips] = 0;
			}
			
			int objectCodeIndex = asmLineIndex - assemblerDirectiveSkips;
			switch (instructionType)
			{
				// Three register Operation
				// R-type (includes multiply)
				case 0:
				case 8:
					objectCode[objectCodeIndex] |= ((Byte) registerMap.get(asmTokens[2])) << 21;
					objectCode[objectCodeIndex] |= ((Byte) registerMap.get(asmTokens[3])) << 16;
					objectCode[objectCodeIndex] |= ((Byte) registerMap.get(asmTokens[1])) << 11;
					objectCode[objectCodeIndex] |= (Byte) instructionOpcodeMap.get(
							asmTokens[0]).byteValue();
					break;
				// Two Register Immediate Operation
				// Shift R-Type
				case 1:
					objectCode[objectCodeIndex] |= ((Byte) registerMap.get(asmTokens[2])) << 16;
					objectCode[objectCodeIndex] |= ((Byte) registerMap.get(asmTokens[1])) << 11;
					objectCode[objectCodeIndex] |= ((byte) (ISAUtil
							.sanitize16bits(asmTokens[3]) & 0x1F)) << 6;
					objectCode[objectCodeIndex] |= (Byte) instructionOpcodeMap.get(
							asmTokens[0]).byteValue();
					break;
				// Single Register Operation
				// Jump R-Type
				case 2:
					objectCode[objectCodeIndex] |= ((Byte) registerMap.get(asmTokens[1])) << 21;
					objectCode[objectCodeIndex] |= (Byte) instructionOpcodeMap.get(
							asmTokens[0]).byteValue();
					break;
				// Two Register Label Operation
				// Branch I-Type
				case 3:
					long branchTarget = symbolTable.get(asmTokens[3])
							- (assemblerPCAddress + 4);
					branchTarget /= 4;
					
					objectCode[objectCodeIndex] |= branchTarget & 0xFFFF;
					objectCode[objectCodeIndex] |= ((Byte) registerMap.get(asmTokens[1])) << 21;
					objectCode[objectCodeIndex] |= ((Byte) registerMap.get(asmTokens[2])) << 16;
					objectCode[objectCodeIndex] |= (long) instructionOpcodeMap
							.get(asmTokens[0]) << 26;
					break;
				// Two Register Immediate Operation
				// Arithmetic and Logic I-Type
				case 4:
					objectCode[objectCodeIndex] |= ISAUtil.sanitize16bits(asmTokens[3]);
					objectCode[objectCodeIndex] |= ((Byte) registerMap.get(asmTokens[1])) << 16;
					objectCode[objectCodeIndex] |= ((Byte) registerMap.get(asmTokens[2])) << 21;
					objectCode[objectCodeIndex] |= (long) instructionOpcodeMap
							.get(asmTokens[0]) << 26;
					break;
				// Register Immediate Operation
				// Load Upper Immediate I-Type
				case 5:
					objectCode[objectCodeIndex] |= ISAUtil.sanitize16bits(asmTokens[2]);
					objectCode[objectCodeIndex] |= ((Byte) registerMap.get(asmTokens[1])) << 16;
					objectCode[objectCodeIndex] |= (long) instructionOpcodeMap
							.get(asmTokens[0]) << 26;
					break;
				// Register Offset Register Operation
				// Load/Store Word I-Type
				case 6:
					objectCode[objectCodeIndex] |= ISAUtil.sanitize16bits(asmTokens[2]);
					objectCode[objectCodeIndex] |= ((Byte) registerMap.get(asmTokens[1])) << 16;
					objectCode[objectCodeIndex] |= ((Byte) registerMap.get(asmTokens[3])) << 21;
					objectCode[objectCodeIndex] |= (long) instructionOpcodeMap
							.get(asmTokens[0]) << 26;
					break;
				// Single Label Operation
				// J-Type
				case 7:
					objectCode[objectCodeIndex] |= (long) (symbolTable.get(asmTokens[1]) >> 2) & 0x3FFFFFF;
					objectCode[objectCodeIndex] |= (long) instructionOpcodeMap
							.get(asmTokens[0]) << 26;
					break;
				// Two Register Operation
				// Jalr Instruction
				case 9:
					objectCode[objectCodeIndex] |= ((Byte) registerMap.get(asmTokens[2])) << 21;
					objectCode[objectCodeIndex] |= ((Byte) registerMap.get(asmTokens[1])) << 11;
					objectCode[objectCodeIndex] |= (Byte) instructionOpcodeMap.get(
							asmTokens[0]).byteValue();
					break;
				// 1st pass Directives
				case 10:
					
					if (asmTokens[0].equals(ASM__WORD__))
					{
						entryType[objectCodeIndex] = 1;
						// System.out.println(asmTokens[1]);
						objectCode[objectCodeIndex] = ISAUtil
								.sanitize32bits(asmTokens[1]);
					}
					else if (asmTokens[0].equals(ASM__ORG__))
					{
						assemblerPCAddress = ISAUtil.sanitize32bits(asmTokens[1]);
						assemblerDirectiveSkips++;
						isSkippable = true;
					}
					else
					{
						assemblerDirectiveSkips++;
						isSkippable = true;
					}
					break;
				
				// Reserved for second pass pseudo operations (so says the old code base)
				case 11:
					break;
				default:
					throw new AssemblerException(
							"Unknown instruction made through first pass and into assembling. CATASTROPHIC FAILURE: "
									+ asmTokens[0]);
			}
			
			// Update address table and assembler PC if this line is a valid instruction /
			// .word directive
			if (!isSkippable)
			{
				addressTable[asmLineIndex - assemblerDirectiveSkips] = assemblerPCAddress;
				assemblerPCAddress += 4;
				
				// TODO update mappers
			}
			
			asmLineIndex++;
		}
		
		System.out.println("\nFinished assembling process.");
		System.out.println("Total statically allocated memory: "
				+ (objectCode.length + byteSpace / 4) + " words.");
		System.out.println("Object code and initialized variables: " + objectCode.length
				+ " words");
		
		String outName = "Symbol Table";
		int totalLength = 60;
		int sideLength = (totalLength - outName.length()) / 2;
		System.out.println(String.format(
				"%n%-" + sideLength + "c%s%-" + sideLength + "c", ' ', outName, ' '));
		symbolTable.forEach((key, value) -> {
			System.out.println(String.format("%-40s | 0x%05x", key, value));
		});
		
		assertTrue("Address Table length does not match object code length",
				addressTable.length == objectCode.length);
		
		outName = "Address Table (Hex) | Instruction (Object Code Hex)";
		sideLength = Math.abs(totalLength - outName.length()) / 2;
		System.out.println(String.format(
				"%n%-" + sideLength + "c%s%-" + sideLength + "c", ' ', outName, ' '));
		for (int index = 0; index < objectCode.length; index++)
		{
			System.out.println(String.format("0x%05X | 0x%05X", addressTable[index],
					objectCode[index]));
		}
		
		return new PLPASMImage(assemblyToDisassemblyMap);
	}
	
	/*
	 * 1st past on Assembly map. Resolves assembler directives, pseudo-ops, and populate
	 * the symbol table.
	 */
	private void preprocess() throws AssemblerException
	{
		for (ASMFile asmFile : asmFiles)
		{
			currentToken = null;
			currentActiveFile = asmFile.getName();
			tokenIterator = asmToTokensMap.get(asmFile).listIterator();
			
			if (!nextToken())
				return;
			
			System.out.println("Starting preprocessing of: " + currentActiveFile);
			
			while (currentToken != null)
			{
				// System.out.println(lineNumber + ": " + currentToken);
				// Loop directives
				if (currentToken.getTypeName() == PLPTokenType.DIRECTIVE.name())
				{
					if (directiveMap.containsKey(currentToken.getValue()))
						directiveMap.get(currentToken.getValue()).perform();
					else
						throw new AssemblerException("Line number: "+ Integer.toString(lineNumber)+ ":Unknown directive. Found: "
								+ currentToken.getValue());
				}
				// Loop PseudoOps
				else if (pseudoOperationMap.containsKey(currentToken.getValue()))
				{
					pseudoOperationMap.get(currentToken.getValue()).perform();
				}
				// Instructions
				else if (isInstruction())
				{
					preprocessNormalInstruction();
				}
				// Comments
				else if (currentToken.getTypeName().equals(PLPTokenType.COMMENT.name()))
				{
					appendPreprocessedInstruction(ASM__SKIP__, lineNumber, true);
					directiveOffset++;
				}
				// Labels
				else if (currentToken.getTypeName().equals(
						PLPTokenType.LABEL_COLON.name()))
				{
					preprocessLabels();
				}
				else if (currentToken.getTypeName().equals(PLPTokenType.NEW_LINE.name()))
				{
					appendPreprocessedInstruction(ASM__SKIP__, lineNumber, true);
					directiveOffset++;
				}
				else
				{
					
					System.out.println("Failed on: " + lineNumber);
					throw new AssemblerException(
							"Line number: " + Integer.toString(lineNumber) + ":Unknown token in preprocessing, found: "
									+ currentToken.getValue());
				}
				
				if (!nextToken())
					break;
				
				this.lineNumber++;
			}
		}
	}
	
	/*
	 * 
	 * ======================= Pseudo Operations =========================
	 */
	
	/**
	 * No-operation. Can be used for branch delay slots
	 * 
	 * nop
	 * 
	 * equivalent to: sll $0, $0, 0
	 * 
	 * @throws AssemblerException
	 */
	private void nopOperation() throws AssemblerException
	{
		appendPreprocessedInstruction("sll $0, $0, 0", lineNumber, true);
		addRegionAndIncrementAddress();
	}
	
	/**
	 * Branch always to label
	 * 
	 * b label
	 * 
	 * equivalent to: beq $0, $0, label
	 * 
	 * @throws AssemblerException
	 */
	private void branchOperation() throws AssemblerException
	{
		expectedNextToken("pseudo move operation");
		
		ensureTokenEquality("Line Number: " + Integer.toString(lineNumber) + "(b) Expected a label to branch to, found: ",
				PLPTokenType.LABEL_PLAIN);
		
		appendPreprocessedInstruction("beq $0, $0, " + currentToken.getValue(),
				lineNumber, true);
		
		addRegionAndIncrementAddress();
	}
	
	/**
	 * Copy Register. Copy $rs to $rd
	 * 
	 * move $rd, $rs
	 * 
	 * equivalent to: add $rd, $0, $rs
	 * 
	 * @throws AssemblerException
	 */
	private void moveOperation() throws AssemblerException
	{
		expectedNextToken("pseudo move operation");
		String destinationRegister = currentToken.getValue();
		ensureTokenEquality("(move) Expected a register, found: ", PLPTokenType.ADDRESS);
		
		expectedNextToken("move pseudo instruction");
		ensureTokenEquality("(move) Expected a comma after " + destinationRegister
				+ " found: ", PLPTokenType.COMMA);
		
		expectedNextToken("pseudo move operation");
		String startingRegister = currentToken.getValue();
		ensureTokenEquality("(move) Expected a register, found: ", PLPTokenType.ADDRESS);
		
		// TODO (Look into) Google Code PLP says it's equivalent instruction is Add, src
		// code uses or
		appendPreprocessedInstruction("or " + destinationRegister + ", $0,"
				+ startingRegister, lineNumber, true);
		addRegionAndIncrementAddress();
	}
	
	/**
	 * Push register onto stack-- we modify the stack pointer first so if the CPU is
	 * interrupted between the two instructions, the data written wont get clobbered
	 * 
	 * Push $rt into the stack
	 * 
	 * push $rt
	 * 
	 * equivalent to: addiu $sp, $sp, -4; sw $rt, 0($sp)
	 * 
	 * @throws AssemblerException
	 */
	private void pushOperation() throws AssemblerException
	{
		expectedNextToken("push pseudo operation");
		
		ensureTokenEquality("(push) Expected a register, found: ", PLPTokenType.ADDRESS);
		
		appendPreprocessedInstruction("addiu $sp, $sp, -4", lineNumber, true);
		appendPreprocessedInstruction("sw " + currentToken.getValue() + ", 4($sp)",
				lineNumber, true);
		
		addRegionAndIncrementAddress(2, 8);
	}
	
	/**
	 * Pop data from stack onto a register-- in the pop case, we want to load first so if
	 * the CPU is interrupted we have the data copied already
	 * 
	 * Pop data from the top of the stack to $rt
	 * 
	 * pop $rt
	 * 
	 * equivalent to: lw $rt, 0($sp); addiu $sp, $sp, 4
	 * 
	 * @throws AssemblerException
	 */
	private void popOperation() throws AssemblerException
	{
		expectedNextToken("pop pseudo operation");
		
		ensureTokenEquality("(push) Expected a register, found: ", PLPTokenType.ADDRESS);
		
		appendPreprocessedInstruction("lw " + currentToken.getValue() + ", 4($sp)",
				lineNumber, true);
		appendPreprocessedInstruction("addiu $sp, $sp, 4", lineNumber, true);
		
		addRegionAndIncrementAddress(2, 8);
	}
	
	/**
	 * Load Immediate
	 * 
	 * Load a 32-bit number to $rd Load the address of a label to a register to be used as
	 * a pointer.
	 * 
	 * <p>
	 * li $rd, imm
	 * </p>
	 * <p>
	 * li $rd, label
	 * </p>
	 * 
	 * <p>
	 * equivalent to: lui $rd, (imm & 0xff00) >> 16; ori $rd, imm & 0x00ff
	 * </p>
	 * <p>
	 * equivalent to: lui $rd, (imm & 0xff00) >> 16; ori $rd, imm & 0x00ff
	 * </p>
	 * 
	 * @throws AssemblerException
	 */
	private void liOperation() throws AssemblerException
	{
		expectedNextToken("load immediate pseudo operation");
		String targetRegister = currentToken.getValue();
		ensureTokenEquality("(li) Expected a register, found: ", PLPTokenType.ADDRESS);
		
		expectedNextToken("load immediate pseudo instruction");
		ensureTokenEquality("(li) Expected a comma after " + targetRegister + " found: ",
				PLPTokenType.COMMA);
		
		expectedNextToken("load immediate pseudo operation");
		String immediateOrLabel = currentToken.getValue();
		ensureTokenEquality("(li) Expected a immediate value or label, found: ",
				PLPTokenType.NUMERIC, PLPTokenType.LABEL_PLAIN);
		
		appendPreprocessedInstruction(String.format("lui %s, %s %s", targetRegister,
				ASM__HIGH__, immediateOrLabel), lineNumber, true);
		appendPreprocessedInstruction(String.format("ori %s, %s, %s %s", targetRegister,
				targetRegister, ASM__LOW__, immediateOrLabel), lineNumber, true);
		
		addRegionAndIncrementAddress(2, 8);
	}
	
	/**
	 * Store the value in $rt to a memory location
	 * 
	 * lwm $rt, imm32/label
	 * 
	 * @throws AssemblerException
	 */
	private void lvmOperation() throws AssemblerException
	{
		expectedNextToken("lvm psuedo operation");
		String targetRegister = currentToken.getValue();
		ensureTokenEquality("(lvm) Expected a register, found: ", PLPTokenType.ADDRESS);
		
		expectedNextToken("two register immediate normal instruction");
		ensureTokenEquality(
				"(lvm) Expected a comma after " + targetRegister + " found: ",
				PLPTokenType.COMMA);
		
		expectedNextToken("lvm psuedo operation");
		String immediateOrLabel = currentToken.getValue();
		ensureTokenEquality("Expected a immediate value or label, found: ",
				PLPTokenType.NUMERIC, PLPTokenType.LABEL_PLAIN);
		
		appendPreprocessedInstruction(
				String.format("lui $at, %s %s", ASM__HIGH__, immediateOrLabel),
				lineNumber, true);
		appendPreprocessedInstruction(
				String.format("ori $at, $at, %s %s", ASM__LOW__, immediateOrLabel),
				lineNumber, true);
		appendPreprocessedInstruction("lw " + targetRegister + ", 0($at)", lineNumber,
				true);
		
		addRegionAndIncrementAddress(3, 12);
	}
	
	/**
	 * Store to memory
	 * 
	 * swm $rt, imm32/label
	 * 
	 * @throws AssemblerException
	 */
	private void svmOperation() throws AssemblerException
	{
		expectedNextToken("svm psuedo operation");
		String targetRegister = currentToken.getValue();
		ensureTokenEquality("(svm) Expected a register, found: ", PLPTokenType.ADDRESS);
		
		expectedNextToken("svm pseudo instruction");
		ensureTokenEquality(
				"(svm) Expected a comma after " + targetRegister + " found: ",
				PLPTokenType.COMMA);
		
		expectedNextToken("svm psuedo operation");
		String immediateOrLabel = currentToken.getValue();
		ensureTokenEquality("Expected a immediate value or label, found:",
				PLPTokenType.NUMERIC, PLPTokenType.LABEL_PLAIN);
		
		appendPreprocessedInstruction(
				String.format("lui $at, %s %s", ASM__HIGH__, immediateOrLabel),
				lineNumber, true);
		appendPreprocessedInstruction(
				String.format("ori $at, $at, %s %s", ASM__LOW__, immediateOrLabel),
				lineNumber, true);
		appendPreprocessedInstruction("sw " + targetRegister + ", 0($at)", lineNumber,
				true);
		
		addRegionAndIncrementAddress(3, 12);
	}
	
	/**
	 * Save registers and call a function
	 * 
	 * Save $aX, $tX, $sX, and $ra to stack and call function
	 * 
	 * call label
	 * 
	 * @throws AssemblerException
	 */
	private void callOperation() throws AssemblerException
	{
		expectedNextToken("call psuedo operation");
		String label = currentToken.getValue();
		ensureTokenEquality("(call) Expected a label, found: ", PLPTokenType.LABEL_PLAIN);
		
		String[] registers = { "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3",
				"$t4", "$t5", "$t6", "$t7", "$t8", "$t9", "$s0", "$s1", "$s2", "$s3",
				"$s4", "$s5", "$s6", "$s7", "$ra" };
		
		appendPreprocessedInstruction("addiu $sp, $sp, " + (registers.length * 4),
				lineNumber, true);
		
		for (int registerIndex = 0; registerIndex < registers.length; registerIndex++)
		{
			appendPreprocessedInstruction("sw " + registers[registerIndex] + ", "
					+ (registerIndex + 1) * 4 + "($sp)", lineNumber, true);
		}
		
		appendPreprocessedInstruction("jal " + label, lineNumber, true);
		appendPreprocessedInstruction("sll $0, $0, $0", lineNumber, true);
		
		addRegionAndIncrementAddress(26, 104);
	}
	
	/**
	 * Restore registers and return from callee. NOT INTERRUPT SAFE
	 * 
	 * Restore $aX, $tX, $sX, and $ra from stack and return
	 * 
	 * return
	 * 
	 * @throws AssemblerException
	 */
	private void returnOperation() throws AssemblerException
	{
		String[] registers = { "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2t", "$t3",
				"$t4", "$t5", "$t6", "$t7", "$t8", "$t9", "$s0", "$s1", "$s2", "$s3",
				"$s4", "$s5", "$s6", "$s7" };
		
		for (int registerIndex = 0; registerIndex < registers.length; registerIndex++)
		{
			appendPreprocessedInstruction("lw " + registers[registerIndex] + ", "
					+ (registerIndex + 1) * 4 + "($sp)", lineNumber, true);
		}
		
		appendPreprocessedInstruction("addu $at, $zero, $ra", lineNumber, true);
		appendPreprocessedInstruction(
				"lw $ra, " + ((registers.length + 1) * 4) + "($sp)", lineNumber, true);
		appendPreprocessedInstruction("addiu $sp, $sp, " + ((registers.length + 1) * 4),
				lineNumber, true);
		appendPreprocessedInstruction("sll $0, $0, $0", lineNumber, true);
		
		addRegionAndIncrementAddress(27, 108);
	}
	
	/**
	 * Save all registers except for $zero to stack
	 * 
	 * save
	 * 
	 * @throws AssemblerException
	 */
	private void saveOperation() throws AssemblerException
	{
		// Start at four instead of zero and exclude $zero register, and normal register
		// names ((registerMap.size() / 2) - 2) * 4;
		appendPreprocessedInstruction("addiu $sp, $sp, " + ((registerMap.size() / 2) - 2)
				* 4, lineNumber, true);
		
		int registerCount = (registerMap.size() / 2) - 1;
		for (int registerIndex = 1; registerIndex <= registerCount; registerIndex++)
		{
			appendPreprocessedInstruction("sw $" + registerIndex + ", " + registerIndex
					* 4 + "($sp)", lineNumber, true);
		}
		
		addRegionAndIncrementAddress(registerCount, registerCount * 4);
	}
	
	/**
	 * Restore all none-zero registers from the stack
	 * 
	 * Restore all registers saved by 'save' in reverse order
	 * 
	 * restore
	 * 
	 * @throws AssemblerException
	 */
	private void restoreOperation() throws AssemblerException
	{
		int registerCount = (registerMap.size() / 2) - 1;
		for (int registerIndex = 1; registerIndex <= registerCount; registerIndex++)
		{
			appendPreprocessedInstruction("lw $" + registerIndex + ", " + registerIndex
					* 4 + "($sp)", lineNumber, true);
		}
		appendPreprocessedInstruction("addiu $sp, $sp, " + ((registerMap.size() / 2) - 2)
				* 4, lineNumber, true);
		
		addRegionAndIncrementAddress(registerCount, registerCount * 4);
	}
	
	/*
	 * 
	 * ======================= Preprocess Operations =========================
	 */
	
	private void preprocessNormalInstruction() throws AssemblerException
	{
		String instruction = currentToken.getValue();
		
		instructionMap.get(instruction).getKey().perform();
		
		addRegionAndIncrementAddress();
	}
	
	/**
	 * xxx $rt, imm
	 * 
	 * @throws AssemblerException
	 */
	private void registerImmediateOperation() throws AssemblerException
	{
		String instruction = currentToken.getValue();
		
		expectedNextToken("register immediate normal instruction");
		String targetRegister = currentToken.getValue();
		ensureTokenEquality("Line number: "+ Integer.toString(lineNumber)+"(" + instruction + ") Expected a target register, found: ",
				PLPTokenType.ADDRESS);
		
		expectedNextToken("register immediate normal instruction");
		ensureTokenEquality("Line number: "+ Integer.toString(lineNumber)+"(" + instruction + ") Expected a comma after "
				+ targetRegister + " found: ", PLPTokenType.COMMA);
		
		expectedNextToken("register immediate normal instruction");
		String immediate = currentToken.getValue();
		ensureTokenEquality("Line number: "+ Integer.toString(lineNumber)+"(" + instruction
				+ ") Expected an immediate value (16-bit), found: ", PLPTokenType.NUMERIC);
		
		appendPreprocessedInstruction(instruction + " " + targetRegister + ", "
				+ immediate, lineNumber, true);
	}
	
	/**
	 * xxx label
	 * 
	 * @throws AssemblerException
	 */
	private void singleLabelOperation() throws AssemblerException
	{
		String instruction = currentToken.getValue();
		
		expectedNextToken("single label normal instruction");
		String label = currentToken.getValue();
		ensureTokenEquality("(" + instruction + ") Expected a label, found: ",
				PLPTokenType.LABEL_PLAIN);
		
		// System.out.println(lineNumber + ": " + instruction + " " + label);
		appendPreprocessedInstruction(instruction + " " + label, lineNumber, true);
	}
	
	/**
	 * xxx $rs
	 * 
	 * @throws AssemblerException
	 */
	private void singleRegisterOperation() throws AssemblerException
	{
		String instruction = currentToken.getValue();
		
		expectedNextToken("single register normal instruction");
		String sourceRegister = currentToken.getValue();
		ensureTokenEquality("(" + instruction + ") Expected a register, found: ",
				PLPTokenType.ADDRESS);
		
		appendPreprocessedInstruction(instruction + " " + sourceRegister, lineNumber,
				true);
	}
	
	/**
	 * xxx $rt, imm($rs)
	 * 
	 * @throws AssemblerException
	 */
	private void registerOffsetRegisterOperation() throws AssemblerException
	{
		String instruction = currentToken.getValue();
		
		expectedNextToken("register offset register normal instruction");
		String targetRegister = currentToken.getValue();
		ensureTokenEquality("(" + instruction + ") Expected a target register, found: ",
				PLPTokenType.ADDRESS);
		
		expectedNextToken("register offset register normal instruction");
		ensureTokenEquality("(" + instruction + ") Expected a comma after "
				+ targetRegister + " found: ", PLPTokenType.COMMA);
		
		expectedNextToken("register offset register normal instruction");
		String offset = currentToken.getValue();
		ensureTokenEquality("(" + instruction
				+ ") Expected an offset value (immediate) in bytes, found: ",
				PLPTokenType.NUMERIC);
		
		expectedNextToken("register offset register normal instruction");
		String sourceRegister = currentToken.getValue();
		ensureTokenEquality("(" + instruction
				+ ") Expected an (source register), found: ",
				PLPTokenType.PARENTHESIS_ADDRESS);
		
		appendPreprocessedInstruction(instruction + " " + targetRegister + ", " + offset
				+ sourceRegister, lineNumber, true);
	}
	
	/**
	 * xxx $rd, $rs, imm
	 * 
	 * @throws AssemblerException
	 */
	private void twoRegisterImmediateOperation() throws AssemblerException
	{
		String instruction = currentToken.getValue();
		
		expectedNextToken("two register immediate normal instruction");
		String destinationRegister = currentToken.getValue();
		ensureTokenEquality("Line number: "+ Integer.toString(lineNumber)+"(" + instruction
				+ ") Expected a destination register, found: ", PLPTokenType.ADDRESS);
		
		expectedNextToken("two register immediate normal instruction");
		ensureTokenEquality("(" + instruction + ") Expected a comma after "
				+ destinationRegister + " found: ", PLPTokenType.COMMA);
		
		expectedNextToken("two register immediate normal instruction");
		String sourceRegister = currentToken.getValue();
		ensureTokenEquality("Line number: "+ Integer.toString(lineNumber)+"(" + instruction + ") Expected an source register, found: ",
				PLPTokenType.ADDRESS);
		
		expectedNextToken("two register immediate normal instruction");
		ensureTokenEquality("Line number: "+ Integer.toString(lineNumber)+"(" + instruction + ") Expected a comma after "
				+ sourceRegister + " found: ", PLPTokenType.COMMA);
		
		expectedNextToken("two register immediate normal instruction");
		String immediate = currentToken.getValue();
		ensureTokenEquality("Line number: "+ Integer.toString(lineNumber)+"(" + instruction + ") Expected an immediate value, found: ",
				PLPTokenType.NUMERIC);
		
		appendPreprocessedInstruction(instruction + " " + destinationRegister + ", "
				+ sourceRegister + ", " + immediate, lineNumber, true);
	}
	
	/**
	 * xxx $rt, $rs, label
	 * 
	 * @throws AssemblerException
	 */
	private void twoRegisterLabelOperation() throws AssemblerException
	{
		String instruction = currentToken.getValue();
		
		expectedNextToken("two register label normal instruction");
		String targetRegister = currentToken.getValue();
		ensureTokenEquality("(" + instruction + ") Expected a target register, found: ",
				PLPTokenType.ADDRESS);
		
		expectedNextToken("two register label normal instruction");
		ensureTokenEquality("(" + instruction + ") Expected a comma after "
				+ targetRegister + " found: ", PLPTokenType.COMMA);
		
		expectedNextToken("two register label normal instruction");
		String sourceRegister = currentToken.getValue();
		ensureTokenEquality("(" + instruction + ") Expected an source register, found: ",
				PLPTokenType.ADDRESS);
		
		expectedNextToken("two register label normal instruction");
		ensureTokenEquality("(" + instruction + ") Expected a comma after "
				+ sourceRegister + " found: ", PLPTokenType.COMMA);
		
		expectedNextToken("two register label normal instruction");
		String label = currentToken.getValue();
		ensureTokenEquality("(" + instruction + ") Expected a label, found: ",
				PLPTokenType.LABEL_PLAIN);
		
		appendPreprocessedInstruction(instruction + " " + targetRegister + ", "
				+ sourceRegister + ", " + label, lineNumber, true);
	}
	
	/**
	 * xxx $rd, $rs
	 * 
	 * @throws AssemblerException
	 */
	private void twoRegisterOperation() throws AssemblerException
	{
		String instruction = currentToken.getValue();
		
		expectedNextToken("two register normal instruction");
		String destinationRegister = currentToken.getValue();
		ensureTokenEquality("(" + instruction
				+ ") Expected a destination register, found: ", PLPTokenType.ADDRESS);
		
		expectedNextToken("two register normal instruction");
		ensureTokenEquality("(" + instruction + ") Expected a comma after "
				+ destinationRegister + " found: ", PLPTokenType.COMMA);
		
		expectedNextToken("two register normal instruction");
		String sourceRegister = currentToken.getValue();
		ensureTokenEquality("(" + instruction + ") Expected an source register, found: ",
				PLPTokenType.ADDRESS);
		
		appendPreprocessedInstruction(instruction + " " + destinationRegister + ", "
				+ sourceRegister, lineNumber, true);
	}
	
	/**
	 * xxx $rd, $rs, $rt
	 * 
	 * @throws AssemblerException
	 */
	private void threeRegisterOperation() throws AssemblerException
	{
		String instruction = currentToken.getValue();
		
		expectedNextToken("three register normal instruction");
		String destinationRegister = currentToken.getValue();
		ensureTokenEquality("(" + instruction
				+ ") Expected a destination register, found: ", PLPTokenType.ADDRESS);
		
		expectedNextToken("three register normal instruction");
		ensureTokenEquality("(" + instruction + ") Expected a comma after "
				+ destinationRegister + " found: ", PLPTokenType.COMMA);
		
		expectedNextToken("three register normal instruction");
		String sourceRegister = currentToken.getValue();
		ensureTokenEquality("(" + instruction + ") Expected an source register, found: ",
				PLPTokenType.ADDRESS);
		
		expectedNextToken("three register normal instruction");
		ensureTokenEquality("(" + instruction + ") Expected a comma after "
				+ sourceRegister + " found: ", PLPTokenType.COMMA);
		
		expectedNextToken("three register normal instruction");
		String targetRegister = currentToken.getValue();
		ensureTokenEquality("(" + instruction + ") Expected an target register, found: ",
				PLPTokenType.ADDRESS);
		
		appendPreprocessedInstruction(instruction + " " + destinationRegister + ", "
				+ sourceRegister + ", " + targetRegister, lineNumber, true);
	}
	
	private void preprocessLabels() throws AssemblerException
	{
		Token directiveToken = currentToken;
		String labelValue = directiveToken.getValue();
		// Remove colon from label
		labelValue = labelValue.substring(0, labelValue.length() - 1);
		
		if (symbolTable.containsKey(labelValue))
		{
			throw new AssemblerException("(" + directiveToken.getTypeName()
					+ ") preprocessing label failure. Symbol already defined, found: "
					+ directiveToken.getValue());
		}
		else
		{
			symbolTable.put(labelValue, currentAddress);
			appendPreprocessedInstruction(ASM__SKIP__, lineNumber, true);
			directiveOffset++;
		}
	}
	
	/*
	 * 
	 * ======================= Preprocess Directives =========================
	 */
	
	private void orgDirective() throws AssemblerException
	{
		expectedNextToken(".org directive");
		
		ensureTokenEquality("(.org) Expected an address, found: ", PLPTokenType.NUMERIC);
		
		appendPreprocessedInstruction(ASM__ORG__ + " " + currentToken.getValue(),
				lineNumber, true);
		directiveOffset++;
		try
		{
			currentAddress = ISAUtil.sanitize32bits(currentToken.getValue());
		}
		catch (AssemblyException e)
		{
			e.printStackTrace();
		}
		
		entryPoint = (entryPoint < 0) ? currentAddress : entryPoint;
	}
	
	private void wordDirective() throws AssemblerException
	{
		expectedNextToken(".word directive");
		
		ensureTokenEquality(
				"(.word) Expected number to initialize current memory address to, found: ",
				PLPTokenType.NUMERIC);
		
		appendPreprocessedInstruction(ASM__WORD__ + " " + currentToken.getValue(),
				lineNumber, true);
		addRegionAndIncrementAddress();
	}
	
	private void spaceDirective() throws AssemblerException
	{
		expectedNextToken(".space directive");
		
		ensureTokenEquality("(.space) Expected a number, found: ", PLPTokenType.NUMERIC);
		
		try
		{
			long size = ISAUtil.sanitize32bits(currentToken.getValue());
			currentAddress += 4 * size;
			byteSpace += 4 * size;
			
			appendPreprocessedInstruction(ASM__ORG__ + " " + currentAddress, lineNumber,
					true);
			directiveOffset++;
		}
		catch (AssemblyException e)
		{
			e.printStackTrace();
		}
		
	}
	
	private void asciiDirective() throws AssemblerException
	{
		Token directiveToken = currentToken;
		boolean wordAligned = directiveToken.getValue().equals(".asciiw");
		
		expectedNextToken(currentToken.getValue() + " directive");
		
		ensureTokenEquality("(" + directiveToken.getValue()
				+ ") Expected a string to store, found: ", PLPTokenType.STRING);
		
		// Strip quotes
		String currentValue = null;
		if (currentToken.getValue().charAt(0) == '\"')
			currentValue = currentToken.getValue().substring(1,
					currentToken.getValue().length() - 1);
		
		// Check for escaped characters
		// Only loop through indices that contain \\
		StringBuffer stringBuffer = new StringBuffer(currentValue);
		List<Character> specialEscapedCharacters = Arrays.asList('n', 'r', 't', '0');
		
		for (int index = -1; (index = currentValue.indexOf("\\", index + 1)) != -1;)
		{
			if (index != currentValue.length() - 1)
			{
				if (specialEscapedCharacters.contains(currentValue.charAt(index + 1)))
				{
					stringBuffer = stringBuffer.replace(
							index,
							index + 2,
							"\\"
									+ specialEscapedCharacters.indexOf(currentValue
											.charAt(index + 1)));
				}
				else if (currentValue.charAt(index + 1) == '\\')
				{
					stringBuffer = stringBuffer.replace(index, index + 2, "\\");
				}
				else
				{
					System.out
							.println("("
									+ directiveToken.getValue()
									+ ") Preprocessing could not identify escaped character, found: \\"
									+ currentValue.charAt(index + 1) + ".\n\tIn "
									+ currentToken.getValue() + "\n");
				}
			}
		}
		currentValue = stringBuffer.toString();
		
		// if directive is asciiz, we need to append a null character
		if (directiveToken.getValue().equals(".asciiz"))
			currentValue += '\0';
		
		// if string is not word-aligned, pad with zeroes
		if (currentValue.length() % 4 != 0 && !wordAligned)
		{
			int neededPadding = 4 - (currentValue.length() % 4);
			for (int index = 0; index < neededPadding; index++)
			{
				currentValue += '\0';
			}
		}
		
		// add ASM__WORD__ 2nd pass directives and were done
		for (int index = 0; index < currentValue.length(); index++)
		{
			if (index % (wordAligned ? 1 : 4) == 0)
				appendPreprocessedInstruction(ASM__WORD__ + " 0x", lineNumber, false);
			
			if (!wordAligned)
			{
				appendPreprocessedInstruction(
						String.format("%02x", (int) currentValue.charAt(index)),
						lineNumber, false);
			}
			else
			{
				appendPreprocessedInstruction(
						String.format("%08x", (int) currentValue.charAt(index)),
						lineNumber, true);
				addRegionAndIncrementAddress();
			}
			
			if (!wordAligned && (index + 1) % 4 == 0 && index > 0)
			{
				addRegionAndIncrementAddress();
				appendPreprocessedInstruction("", lineNumber, true);
			}
		}
	}
	
	private void textDirective() throws AssemblerException
	{
		expectedNextToken(".text directive");
		
		if (currentRegion != 1)
		{
			ensureTokenEquality("(.text) Expected a string, found: ", PLPTokenType.STRING);
			
			directiveOffset++;
			
			if (currentRegion == 2)
				currentDataAddress = currentAddress;
			
			currentRegion = 1;
			currentAddress = currentTextAddress;
			
			// TODO ASM .text has a line that should never be reached. Look into it.
			appendPreprocessedInstruction(ASM__ORG__ + " " + currentToken.getValue(),
					lineNumber, true);
			try
			{
				currentAddress = ISAUtil.sanitize32bits(currentToken.getValue());
			}
			catch (AssemblyException e)
			{
				e.printStackTrace();
			}
			entryPoint = currentAddress;
			currentTextAddress = entryPoint;
			
			if (currentAddress < 0)
				throw new AssemblerException("Starting address for .text is not defined.");
		}
	}
	
	private void dataDirective() throws AssemblerException
	{
		expectedNextToken(".data directive");
		
		ensureTokenEquality("(.data) Expected a string, found: ", PLPTokenType.STRING);
		
		if (currentRegion != 2)
		{
			directiveOffset++;
			if (currentRegion == 1)
				currentTextAddress = currentAddress;
			
			currentRegion = 2;
			currentAddress = currentDataAddress;
			
			// TODO Asm .data has a line that should never be reached. Look into it.
			appendPreprocessedInstruction(ASM__ORG__ + " " + currentToken.getValue(),
					lineNumber, true);
			try
			{
				currentAddress = ISAUtil.sanitize32bits(currentToken.getValue());
			}
			catch (AssemblyException e)
			{
				e.printStackTrace();
			}
			currentDataAddress = currentAddress;
			
			if (currentAddress < 0)
				throw new AssemblerException("Starting address for .data is not defined.");
		}
	}
	
	private void equDirective() throws AssemblerException
	{
		expectedNextToken(".equ directive");
		
		ensureTokenEquality("(.equ) Expected a string, found: ", PLPTokenType.STRING);
		
		String symbol = currentToken.getValue();
		if (symbolTable.containsKey(symbol))
		{
			throw new AssemblerException("(.equ) Symbol table already contains: "
					+ currentToken.getValue());
		}
		
		expectedNextToken(".equ directive");
		
		ensureTokenEquality("(.equ) Expected an address after symbol, found: ",
				PLPTokenType.NUMERIC);
		
		long value = Long.MIN_VALUE;
		try
		{
			value = ISAUtil.sanitize32bits(currentToken.getValue());
		}
		catch (AssemblyException e)
		{
			e.printStackTrace();
		}
		
		if (value < 0)
		{
			throw new AssemblerException(
					"(.equ) Could not process address after symbol, found: "
							+ currentToken.getValue());
		}
		
		symbolTable.put(symbol, value);
		
	}
	
	private void includeDirective() throws AssemblerException
	{
		expectedNextToken("include directive");
		
		appendPreprocessedInstruction(ASM__SKIP__, lineNumber, true);
		
		throw new UnsupportedOperationException("Include Directive is not implemented");
	}
	
	/*
	 * 
	 * ======================= Initialization =========================
	 */
	
	private void initialize()
	{
		symbolTable = new HashMap<>();
		instructionMap = new HashMap<>();
		registerMap = new HashMap<>();
		instructionOpcodeMap = new HashMap<>();
		pseudoOperationMap = new HashMap<>();
		directiveMap = new HashMap<>();
		lineNumAndAsmFileMap = new HashMap<>();
		instructionList = new ArrayList<>();
		
		firstPassString = new StringBuilder();
		
		asmIndex = 0;
		mapperIndex = 0;
		currentAddress = 0;
		currentTextAddress = -1;
		currentDataAddress = -1;
		entryPoint = -1;
		directiveOffset = 0;
		byteSpace = 0;
		lineNumber = 1;
		topLevelFile = asmFiles.get(0).getName();
		
		setInstructionMapValues();
		setRegisterMapValues();
		setPseudoMapValues();
		
		lexer = new Lexer(PLPTokenType.createSet());
	}
	
	private void setInstructionMapValues()
	{
		PLPInstruction currentInstruction;
		
		currentInstruction = PLPInstruction.ADD_UNSIGNED;
		instructionMap.put(currentInstruction.getMnemonic(), new Pair<>(
				this::threeRegisterOperation, currentInstruction.getInstructionType()));
		
		currentInstruction = PLPInstruction.SUBTRACT_UNSIGNED;
		instructionMap.put(currentInstruction.getMnemonic(), new Pair<>(
				this::threeRegisterOperation, currentInstruction.getInstructionType()));
		
		currentInstruction = PLPInstruction.AND;
		instructionMap.put(currentInstruction.getMnemonic(), new Pair<>(
				this::threeRegisterOperation, currentInstruction.getInstructionType()));
		
		currentInstruction = PLPInstruction.OR;
		instructionMap.put(currentInstruction.getMnemonic(), new Pair<>(
				this::threeRegisterOperation, currentInstruction.getInstructionType()));
		
		currentInstruction = PLPInstruction.NOR;
		instructionMap.put(currentInstruction.getMnemonic(), new Pair<>(
				this::threeRegisterOperation, currentInstruction.getInstructionType()));
		
		currentInstruction = PLPInstruction.SET_ON_LESS;
		instructionMap.put(currentInstruction.getMnemonic(), new Pair<>(
				this::threeRegisterOperation, currentInstruction.getInstructionType()));
		
		currentInstruction = PLPInstruction.SET_ON_LESS_UNSIGNED;
		instructionMap.put(currentInstruction.getMnemonic(), new Pair<>(
				this::threeRegisterOperation, currentInstruction.getInstructionType()));
		
		currentInstruction = PLPInstruction.SHIFT_LEFT_LOGICAL_VARIABLE;
		instructionMap.put(currentInstruction.getMnemonic(), new Pair<>(
				this::threeRegisterOperation, currentInstruction.getInstructionType()));
		
		currentInstruction = PLPInstruction.SHIFT_RIGHT_LOGICAL_VARIABLE;
		instructionMap.put(currentInstruction.getMnemonic(), new Pair<>(
				this::threeRegisterOperation, currentInstruction.getInstructionType()));
		
		currentInstruction = PLPInstruction.SHIFT_LEFT_LOGICAL;
		instructionMap.put(
				currentInstruction.getMnemonic(),
				new Pair<>(this::twoRegisterImmediateOperation, currentInstruction
						.getInstructionType()));
		
		currentInstruction = PLPInstruction.SHIFT_RIGHT_LOGICAL;
		instructionMap.put(
				currentInstruction.getMnemonic(),
				new Pair<>(this::twoRegisterImmediateOperation, currentInstruction
						.getInstructionType()));
		
		currentInstruction = PLPInstruction.JUMP_RETURN;
		instructionMap.put(currentInstruction.getMnemonic(), new Pair<>(
				this::singleRegisterOperation, currentInstruction.getInstructionType()));
		
		currentInstruction = PLPInstruction.BRANCH_EQUALS;
		instructionMap
				.put(currentInstruction.getMnemonic(),
						new Pair<>(this::twoRegisterLabelOperation, currentInstruction
								.getInstructionType()));
		
		currentInstruction = PLPInstruction.BRANCH_NOT_EQUALS;
		instructionMap
				.put(currentInstruction.getMnemonic(),
						new Pair<>(this::twoRegisterLabelOperation, currentInstruction
								.getInstructionType()));
		
		currentInstruction = PLPInstruction.ADD_IMMEDIATE_UNSIGNED;
		instructionMap.put(
				currentInstruction.getMnemonic(),
				new Pair<>(this::twoRegisterImmediateOperation, currentInstruction
						.getInstructionType()));
		
		currentInstruction = PLPInstruction.AND_IMMEDIATE;
		instructionMap.put(
				currentInstruction.getMnemonic(),
				new Pair<>(this::twoRegisterImmediateOperation, currentInstruction
						.getInstructionType()));
		
		currentInstruction = PLPInstruction.OR_IMMEDIATE;
		instructionMap.put(
				currentInstruction.getMnemonic(),
				new Pair<>(this::twoRegisterImmediateOperation, currentInstruction
						.getInstructionType()));
		
		currentInstruction = PLPInstruction.SET_ON_LESS_IMMEDIATE;
		instructionMap.put(
				currentInstruction.getMnemonic(),
				new Pair<>(this::twoRegisterImmediateOperation, currentInstruction
						.getInstructionType()));
		
		currentInstruction = PLPInstruction.SET_ON_LESS_IMMEDIATE_UNSIGNED;
		instructionMap.put(
				currentInstruction.getMnemonic(),
				new Pair<>(this::twoRegisterImmediateOperation, currentInstruction
						.getInstructionType()));
		
		currentInstruction = PLPInstruction.LOAD_UPPER_IMMEDIATE;
		instructionMap.put(
				currentInstruction.getMnemonic(),
				new Pair<>(this::registerImmediateOperation, currentInstruction
						.getInstructionType()));
		
		currentInstruction = PLPInstruction.LOAD_WORD;
		instructionMap.put(
				currentInstruction.getMnemonic(),
				new Pair<>(this::registerOffsetRegisterOperation, currentInstruction
						.getInstructionType()));
		
		currentInstruction = PLPInstruction.STORE_WORD;
		instructionMap.put(
				currentInstruction.getMnemonic(),
				new Pair<>(this::registerOffsetRegisterOperation, currentInstruction
						.getInstructionType()));
		
		currentInstruction = PLPInstruction.JUMP;
		instructionMap.put(currentInstruction.getMnemonic(), new Pair<>(
				this::singleLabelOperation, currentInstruction.getInstructionType()));
		
		currentInstruction = PLPInstruction.JUMP_AND_LINK;
		instructionMap.put(currentInstruction.getMnemonic(), new Pair<>(
				this::singleLabelOperation, currentInstruction.getInstructionType()));
		
		currentInstruction = PLPInstruction.MULTIPLY_HIGH;
		instructionMap.put(currentInstruction.getMnemonic(), new Pair<>(
				this::threeRegisterOperation, currentInstruction.getInstructionType()));
		
		currentInstruction = PLPInstruction.MULTIPLY_LOW;
		instructionMap.put(currentInstruction.getMnemonic(), new Pair<>(
				this::threeRegisterOperation, currentInstruction.getInstructionType()));
		
		currentInstruction = PLPInstruction.JUMP_AND_LINK_RETURN;
		instructionMap.put(currentInstruction.getMnemonic(), new Pair<>(
				this::twoRegisterOperation, currentInstruction.getInstructionType()));
		
		instructionMap.put("ASM__WORD__", new Pair<>(() -> {
		}, 10));
		instructionMap.put("ASM__ORG__", new Pair<>(() -> {
		}, 10));
		instructionMap.put("ASM__SKIP__", new Pair<>(() -> {
		}, 10));
		instructionMap.put("ASM__LINE_OFFSET__", new Pair<>(() -> {
		}, 10));
		instructionMap.put("ASM__POINTER__", new Pair<>(() -> {
		}, 10));
		
		for (PLPInstruction instruction : PLPInstruction.values())
		{
			instructionOpcodeMap
					.put(instruction.getMnemonic(), instruction.getByteCode());
			
		}
	}
	
	private void setPseudoMapValues()
	{
		pseudoOperationMap.put("nop", this::nopOperation);
		pseudoOperationMap.put("b", this::branchOperation);
		pseudoOperationMap.put("move", this::moveOperation);
		pseudoOperationMap.put("push", this::pushOperation);
		pseudoOperationMap.put("pop", this::popOperation);
		pseudoOperationMap.put("li", this::liOperation);
		pseudoOperationMap.put("call", this::callOperation);
		pseudoOperationMap.put("return", this::returnOperation);
		pseudoOperationMap.put("save", this::saveOperation);
		pseudoOperationMap.put("restore", this::restoreOperation);
		pseudoOperationMap.put("lwm", this::lvmOperation);
		pseudoOperationMap.put("swm", this::svmOperation);
		
		directiveMap.put(".org", this::orgDirective);
		directiveMap.put(".word", this::wordDirective);
		directiveMap.put(".space", this::spaceDirective);
		directiveMap.put(".ascii", this::asciiDirective);
		directiveMap.put(".asciiz", this::asciiDirective);
		directiveMap.put(".asciiw", this::asciiDirective);
		directiveMap.put(".include", this::includeDirective);
		directiveMap.put(".text", this::textDirective);
		directiveMap.put(".data", this::dataDirective);
		directiveMap.put(".equ", this::equDirective);
	}
	
	private void setRegisterMapValues()
	{
		String[] registers = { "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
				"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9",
				"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$i0", "$i1",
				"$iv", "$sp", "$ir", "$ra" };
		
		for (int index = 0; index < registers.length; index++)
		{
			registerMap.put("$" + index, (byte) index);
			registerMap.put(registers[index], (byte) index);
		}
	}
	
	/*
	 * 
	 * ======================= Helper Functions =========================
	 */
	
	private void appendPreprocessedInstruction(String instruction, int lineNumber,
			boolean newLine)
	{
		lineNumAndAsmFileMap.put(mapperIndex, new Pair<>(lineNumber, asmIndex));
		
		firstPassString.append(instruction);
		if (newLine)
		{
			mapperIndex++;
			firstPassString.append("\n");
		}
		// System.out.println(lineNumber + ": " + instruction);
		
		// System.out.println(lineNumber + ": " + instruction);
	}
	
	private boolean nextToken()
	{
		return nextToken(1);
	}
	
	private boolean nextToken(int count)
	{
		for (int index = 0; index < count; index++)
		{
			if (!tokenIterator.hasNext())
				return false;
			currentToken = tokenIterator.next();
			
		}
		
		return true;
	}
	
	private void expectedNextToken(String location) throws AssemblerException
	{
		String previousToken = currentToken.getValue();
		if (!nextToken())
			throw new AssemblerException("Previous token->(" + previousToken
					+ ") Unexpected end of token stream. In " + location);
	}
	
	private void addRegionAndIncrementAddress()
	{
		addRegionAndIncrementAddress(1, 4);
	}
	
	private void addRegionAndIncrementAddress(int timesToAddCurrentRegion,
			int currentAddressIncrementSize)
	{
		currentAddress += currentAddressIncrementSize;
	}
	
	private void ensureTokenEquality(String assemblerExceptionMessage,
			PLPTokenType compareTo) throws AssemblerException
	{
		String lineNumberString = "Failed at Line Number: " + Integer.toString(lineNumber)+ " :";
		assemblerExceptionMessage = lineNumberString + assemblerExceptionMessage;
		
		if (compareTo.equals(PLPTokenType.INSTRUCTION))
		{
			willThrowAssemblerMessage(!isInstruction(), assemblerExceptionMessage
					+ currentToken.getValue());
			return;
		}
		else if (compareTo.equals(PLPTokenType.LABEL_PLAIN))
		{
			willThrowAssemblerMessage(!isLabel(), assemblerExceptionMessage
					+ currentToken.getValue());
			return;
		}
		else if (compareTo.equals(PLPTokenType.ADDRESS)
				|| compareTo.equals(PLPTokenType.PARENTHESIS_ADDRESS))
		{
			willThrowAssemblerMessage(!isValidRegister(), assemblerExceptionMessage
					+ currentToken.getValue());
			return;
		}
		
		if (!currentToken.getTypeName().equals(compareTo.name()))
			willThrowAssemblerMessage(true,
					assemblerExceptionMessage + currentToken.getValue());
	}
	
	private void willThrowAssemblerMessage(boolean isThrown, String message)
			throws AssemblerException
	{
		if (isThrown)
		{
			throw new AssemblerException(message);
		}
		
	}
	
	private void ensureTokenEquality(String assemblerExceptionMessage,
			PLPTokenType... compareTo) throws AssemblerException
	{
		for (PLPTokenType comparison : compareTo)
		{
			if (compareTo.equals(PLPTokenType.INSTRUCTION))
			{
				willThrowAssemblerMessage(!isInstruction(), assemblerExceptionMessage
						+ currentToken.getValue());
				return;
			}
			else if (compareTo.equals(PLPTokenType.LABEL_PLAIN))
			{
				willThrowAssemblerMessage(!isLabel(), assemblerExceptionMessage
						+ currentToken.getValue());
				return;
			}
			else if (compareTo.equals(PLPTokenType.ADDRESS)
					|| compareTo.equals(PLPTokenType.PARENTHESIS_ADDRESS))
			{
				willThrowAssemblerMessage(!isValidRegister(), assemblerExceptionMessage
						+ currentToken.getValue());
				return;
			}
			
			else if (currentToken.getTypeName().equals(comparison.name()))
				return;
		}
		
		throw new AssemblerException(assemblerExceptionMessage + currentToken.getValue());
	}
	
	private boolean isLabel()
	{
		return isLabel(currentToken);
	}
	
	private boolean isLabel(Token aToken)
	{
		return isLabel(aToken.getTypeName(), aToken.getValue());
	}
	
	private boolean isLabel(String tokenType, String value)
	{
		if (!isTypeInstructionOrLabel(tokenType, value))
			return false;
		
		return !instructionMap.containsKey(value);
	}
	
	private boolean isInstruction()
	{
		return isInstruction(currentToken);
	}
	
	private boolean isInstruction(Token aToken)
	{
		return isInstruction(aToken.getTypeName(), aToken.getValue());
	}
	
	private boolean isInstruction(String tokenType, String value)
	{
		return isTypeInstructionOrLabel(tokenType, value)
				&& instructionMap.containsKey(value);
	}
	
	private boolean isTypeInstructionOrLabel(String tokenType, String value)
	{
		return (tokenType.equals(PLPTokenType.INSTRUCTION.name()) || tokenType
				.equals(PLPTokenType.LABEL_PLAIN.name()));
	}
	
	private boolean isValidRegister()
	{
		return isValidRegister(currentToken);
	}
	
	private boolean isValidRegister(Token aToken)
	{
		return isValidRegister(aToken.getTypeName(), aToken.getValue());
	}
	
	private boolean isValidRegister(String tokenType, String value)
	{
		if (tokenType.equals(PLPTokenType.ADDRESS.name()))
		{
			return registerMap.containsKey(value);
		}
		else if (tokenType.equals(PLPTokenType.PARENTHESIS_ADDRESS.name()))
		{
			return registerMap.containsKey(value.replaceAll("\\(|\\)", ""));
		}
		
		return false;
	}
}
