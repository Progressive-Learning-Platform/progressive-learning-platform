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
    static int PLP_ASM_ERROR_INVALID_TOKEN          = 1;
    static int PLP_ASM_ERROR_INVALID_REFSOURCE      = 2;
    static int PLP_ASM_ERROR_CYCLIC_REFERENCE       = 3;
    static int PLP_ASM_ERROR_SOURCE_NOT_FOUND       = 4;
    static int PLP_ASM_ERROR_DIRECTIVE_SYNTAX       = 5;

    // Assembler 2nd Pass Errors
    static int PLP_ASM_ERROR_GENERAL_SYNTAX         = 6;
    static int PLP_ASM_ERROR_INVALID_IDENTIFIER     = 7;
    static int PLP_ASM_ERROR_INVALID_OPCODE         = 8;
    static int PLP_ASM_ERROR_WRONG_INSTR_TYPE       = 9;
    static int PLP_ASM_ERROR_INVALID_REGISTER       = 10;
    static int PLP_ASM_ERROR_INVALID_OPERAND        = 11;
    static int PLP_ASM_ERROR_NUMBER_OF_OPERANDS     = 12;
    static int PLP_ASM_ERROR_INVALID_BRANCH_TARGET  = 13;

    static int PLP_ERROR_GENERIC = -1;

    static String[] errorStr;
    static int[]    errorCodes;

    public static int PLPError(int errorCode) {
        //PLPToolView.appendStdOut("Error " + errorCode);
        return errorCode;
    }

    public static void PLPInfo(String infoStr) {
        System.out.println(infoStr);
    }

    public static void PLPDebug(String debugStr, int requestedDebugLevel) {
        if(requestedDebugLevel == debugLevel)
            System.out.println(debugStr);
    }
}
