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
 *
 * @author wira
 */
public class Constants {
public static final String versionString = "Beta 2";
    public static final int debugLevel = 0;
    public static final int PLP_ASM_MAX_NUMBER_OF_ASMS           = 256;

    // Assembler 1st Pass Errors
    public static final int PLP_ASM_INVALID_TOKEN                = 16;
    public static final int PLP_ASM_INVALID_REFSOURCE            = 17;
    public static final int PLP_ASM_CYCLIC_REFERENCE             = 18;
    public static final int PLP_ASM_SOURCE_NOT_FOUND             = 19;
    public static final int PLP_ASM_DIRECTIVE_SYNTAX_ERROR       = 20;
    public static final int PLP_ASM_DUPLICATE_LABEL              = 21;
    public static final int PLP_ASM_INVALID_STRING               = 22;
    public static final int PLP_ASM_INVALID_LINENUM              = 23;

    // Assembler 2nd Pass Errors
    public static final int PLP_ASM_GENERAL_SYNTAX_ERROR         = 64;
    public static final int PLP_ASM_INVALID_IDENTIFIER           = 65;
    public static final int PLP_ASM_INVALID_OPCODE               = 66;
    public static final int PLP_ASM_WRONG_INSTR_TYPE             = 67;
    public static final int PLP_ASM_INVALID_REGISTER             = 68;
    public static final int PLP_ASM_INVALID_OPERAND              = 69;
    public static final int PLP_ASM_INVALID_NUMBER_OF_OPERANDS   = 70;
    public static final int PLP_ASM_INVALID_BRANCH_TARGET        = 71;
    public static final int PLP_ASM_INVALID_JUMP_TARGET          = 72;
    public static final int PLP_ASM_INVALID_POINTER              = 73;

    // Formatter Errors
    public static final int PLP_OUT_UNHANDLED_ERROR              = 128;
    public static final int PLP_OUT_CAN_NOT_WRITE_TO_FILE        = 129;
    public static final int PLP_OUT_FILE_EXISTS                  = 130;

    // Simulator errors
    public static final int PLP_SIM_INSTRMEM_OUT_OF_BOUNDS       = 256;
    public static final int PLP_SIM_UNINITIALIZED_MEMORY         = 257;
    public static final int PLP_SIM_UNHANDLED_INSTRUCTION_TYPE   = 258;
    public static final int PLP_SIM_EVALUATION_FAILED            = 259;
    public static final int PLP_SIM_STALE                        = 260;
    public static final int PLP_SIM_OUT_ADDRESS_OUT_OF_RANGE     = 261;
    public static final int PLP_SIM_OUT_UNALIGNED_MEMORY         = 262;
    public static final int PLP_SIM_MODULE_DISABLED              = 263;
    public static final int PLP_SIM_BUS_ERROR                    = 264;
    public static final int PLP_SIM_BUS_MAPPING_ERROR            = 265;
    public static final int PLP_SIM_INVALID_MODULE               = 266;
    public static final int PLP_SIM_WRITE_TO_READONLY_MODULE     = 267;

    // Programmer errors
    public static final int PLP_PRG_SOURCES_NOT_ASSEMBLED        = 512;
    public static final int PLP_PRG_NOT_A_SERIAL_PORT            = 513;
    public static final int PLP_PRG_PORT_IN_USE                  = 514;
    public static final int PLP_PRG_SERIAL_TRANSMISSION_ERROR    = 515;
    public static final int PLP_PRG_PLP_FILE_NOT_FOUND           = 516;
    public static final int PLP_PRG_INVALID_PLP_FILE             = 517;
    public static final int PLP_PRG_INVALID_IMAGE_FILE           = 518;
    public static final int PLP_PRG_IMAGE_OUT_OF_DATE            = 519;
    public static final int PLP_PRG_UNABLE_TO_CLOSE_PORT         = 520;

    // Simulator flags
    public static final long PLP_SIM_FWD_NO_EVENTS               = 0xFFFFFFE0;
    public static final long PLP_SIM_FWD_EX_EX_RTYPE             = 0x00000001;
    public static final long PLP_SIM_FWD_EX_EX_ITYPE             = 0x00000002;
    public static final long PLP_SIM_FWD_EX_RF                   = 0x00000004;
    public static final long PLP_SIM_FWD_MEM_MEM                 = 0x00000008;
    public static final long PLP_SIM_FWD_MEM_EX                  = 0x00000010;
    public static final long PLP_SIM_FWD_MEM_EX_LW               = 0x00000020;
    public static final long PLP_SIM_IF_STALL_SET                = 0x00000100;
    public static final long PLP_SIM_ID_STALL_SET                = 0x00000200;
    public static final long PLP_SIM_EX_STALL_SET                = 0x00000400;
    public static final long PLP_SIM_MEM_STALL_SET                = 0x00000800;

    // General
    public static final int PLP_NUMBER_ERROR                     = -1;
    public static final int PLP_OK                               = 0;
    public static final int PLP_GENERIC_ERROR                    = 1;
    public static final int PLP_TOOLAPP_ERROR                    = 2;

    public static final int PLP_ERROR_RETURN                     = -1;
    public static final int PLP_OOPS                             = 9001;

    public static final int PLP_LONG_SIM                         = 100;

    // Flags
    public static final boolean FLAGS_ALIGNED_MEMORY             = true;
}
