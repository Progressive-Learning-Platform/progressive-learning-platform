/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool;

/**
 *
 * @author wira
 */
public class PLPMsg {

    static int debugLevel = 0;

    // Assembler 1st Pass Errors
    static int PLP_ASM_ERROR_INVALID_TOKEN          = 16;
    static int PLP_ASM_ERROR_INVALID_REFSOURCE      = 17;
    static int PLP_ASM_ERROR_CYCLIC_REFERENCE       = 18;
    static int PLP_ASM_ERROR_SOURCE_NOT_FOUND       = 19;
    static int PLP_ASM_ERROR_DIRECTIVE_SYNTAX       = 20;

    // Assembler 2nd Pass Errors
    static int PLP_ASM_ERROR_GENERAL_SYNTAX         = 64;
    static int PLP_ASM_ERROR_INVALID_IDENTIFIER     = 65;
    static int PLP_ASM_ERROR_INVALID_OPCODE         = 66;
    static int PLP_ASM_ERROR_WRONG_INSTR_TYPE       = 67;
    static int PLP_ASM_ERROR_INVALID_REGISTER       = 68;
    static int PLP_ASM_ERROR_INVALID_OPERAND        = 69;
    static int PLP_ASM_ERROR_NUMBER_OF_OPERANDS     = 70;
    static int PLP_ASM_ERROR_INVALID_BRANCH_TARGET  = 71;
    static int PLP_ASM_ERROR_INVALID_JUMP_TARGET    = 72;

    static int PLP_ERROR_GENERIC                    = 1;
    static int PLP_OOPS                             = 2;

    static int PLP_NUMBER_ERROR                     = 3;

    static String[] errorStr;
    static int[]    errorCodes;

    static int lastError = 0;
    static int markCounter = 0;

    // Error message
    public static void E(String infoStr, int errorCode, Object objIdentifier) {
        System.out.println("[E] #" + errorCode + " " + objIdentifier.toString() + ": " + infoStr);
        lastError = errorCode;
    }

    // Information message
    public static void I(String infoStr, Object objIdentifier) {
        if(objIdentifier != null)
            System.out.println("[I] " + objIdentifier.toString() + ": " + infoStr);
        else
            System.out.println("[I] " + infoStr);
    }

    // Debug message
    public static void D(String debugStr, int requestedDebugLevel, Object objIdentifier) {
        if(requestedDebugLevel <= debugLevel)
            System.out.println("[D] " + objIdentifier.toString() + ": " + debugStr);
    }

    // Mark
    public static void mark() {
        System.out.println("[D] " + markCounter + " We're here!");
        markCounter++;
    }
}
