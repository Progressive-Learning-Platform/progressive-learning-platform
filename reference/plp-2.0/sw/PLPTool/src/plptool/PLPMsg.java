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

    // Assembler 1st Pass Errors
    static int PLP_ASM_ERROR_INVALID_REFSOURCE      = 1;
    static int PLP_ASM_ERROR_CYCLIC_REFERENCE       = 2;
    static int PLP_ASM_ERROR_SOURCE_NOT_FOUND       = 3;

    // Assembler 2nd Pass Errors
    static int PLP_ASM_ERROR_GENERAL_SYNTAX_ERROR   = 4;
    static int PLP_ASM_ERROR_INVALID_IDENTIFIER     = 5;
    static int PLP_ASM_ERROR_INVALID_OPCODE         = 6;
    static int PLP_ASM_ERROR_WRONG_INSTR_TYPE       = 7;
    static int PLP_ASM_ERROR_INVALID_REGISTER       = 8;
    static int PLP_ASM_ERROR_INVALID_OPERAND        = 9;
    static int PLP_ASM_ERROR_NUMBER_OF_OPERANDS     = 10;
    static int PLP_ASM_ERROR_INVALID_BRANCH_TARGET  = 11;

    static int PLP_ERROR_GENERIC = -1;

    static String[] errorStr;
    static int[]    errorCodes;

    public static int PLPError(int errorCode) {
        //PLPToolView.appendStdOut("Error " + errorCode);
        return errorCode;
    }
}
