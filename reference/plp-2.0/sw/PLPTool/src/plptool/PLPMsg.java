/*
    Copyright 2010 David Fritz, Brian Gordon, Wira Mulia

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

package plptool;

/**
 * PLPTool constant definitions and messaging utility class.
 *
 * @author wira
 */
public class PLPMsg {

    static String versionString = "Beta 2";

    static int debugLevel = 0;
    static int PLP_ASM_MAX_NUMBER_OF_ASMS           = 256;

    // Assembler 1st Pass Errors
    static int PLP_ASM_ERROR_INVALID_TOKEN          = 16;
    static int PLP_ASM_ERROR_INVALID_REFSOURCE      = 17;
    static int PLP_ASM_ERROR_CYCLIC_REFERENCE       = 18;
    static int PLP_ASM_ERROR_SOURCE_NOT_FOUND       = 19;
    static int PLP_ASM_ERROR_DIRECTIVE_SYNTAX       = 20;
    static int PLP_ASM_ERROR_DUPLICATE_LABEL        = 21;
    static int PLP_ASM_ERROR_INVALID_STRING         = 22;

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
    static int PLP_ASM_ERROR_INVALID_POINTER        = 73;

    // Formatter Errors
    static int PLP_OUT_UNHANDLED_ERROR              = 128;
    static int PLP_OUT_CAN_NOT_WRITE_TO_FILE        = 129;
    static int PLP_OUT_FILE_EXISTS                  = 130;

    // Emulator errors
    static int PLP_EMU_INSTRMEM_OUT_OF_BOUNDS       = 256;
    static int PLP_EMU_UNINITIALIZED_MEMORY         = 257;

    static int PLP_OK                               = 0;
    static int PLP_ERROR_GENERIC                    = 1;
    static int PLP_ERROR_RETURN                     = -1;
    static int PLP_OOPS                             = 9001;

    static int PLP_NUMBER_ERROR                     = -1;

    static String[] errorStr;
    static int[]    errorCodes;

    static int lastError = 0;
    static int markCounter = 0;

    // Error message
    public static int E(String errStr, int errorCode, Object objIdentifier) {
        if(objIdentifier != null)
            System.out.println("[E] #" + errorCode + " " + objIdentifier.toString() + ": " + errStr);
        else
            System.out.println("[E] #" + errorCode + " " + errStr);
        lastError = errorCode;
        return errorCode;
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
            if(objIdentifier != null)
                System.out.println("[D] " + objIdentifier.toString() + ": " + debugStr);
            else
                System.out.println("[D] " + debugStr);
    }

    // Mark
    public static void mark() {
        System.out.println("[D] " + markCounter + " We're here!");
        markCounter++;
    }
}
