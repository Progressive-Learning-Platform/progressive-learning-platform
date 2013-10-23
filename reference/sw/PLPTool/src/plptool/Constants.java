/*
    Copyright 2010-2013 David Fritz, Brian Gordon, Wira Mulia

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
    public static String launchPath = ".";

    public static final int minimumJREMajorVersion               = 1;
    public static final int minimumJREMinorVersion               = 5;
    public static final int DEFAULT_IO_BUFFER_SIZE               = 1024;

    public static int debugLevel = 0;
    public static final int PLP_ASM_MAX_NUMBER_OF_ASMS           = 256;
    public static final int PLP_BAUDRATE                         = 57600;
    public static final int PLP_MAX_STEPS                        = 1000000;

    // Assembler 1st Pass Errors
    public static final int PLP_ASM_INVALID_TOKEN                = 16;
    public static final int PLP_ASM_INVALID_REFSOURCE            = 17;
    public static final int PLP_ASM_CYCLIC_REFERENCE             = 18;
    public static final int PLP_ASM_SOURCE_NOT_FOUND             = 19;
    public static final int PLP_ASM_DIRECTIVE_SYNTAX_ERROR       = 20;
    public static final int PLP_ASM_DUPLICATE_LABEL              = 21;
    public static final int PLP_ASM_INVALID_STRING               = 22;
    public static final int PLP_ASM_INVALID_LINENUM              = 23;
    public static final int PLP_ASM_PREPROCESS_FAILED            = 24;

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
    public static final int PLP_ASM_ASSEMBLE_FAILED              = 74;
    public static final int PLP_ASM_MEM_REGION_START_NOT_INIT    = 75;

    // Formatter Errors
    public static final int PLP_OUT_UNHANDLED_ERROR              = 128;
    public static final int PLP_OUT_CAN_NOT_WRITE_TO_FILE        = 129;
    public static final int PLP_OUT_FILE_EXISTS                  = 130;

    // Simulator errors
    public static final int PLP_SIM_INSTRUCTION_FETCH_FAILED     = 256;
    public static final int PLP_SIM_UNINITIALIZED_MEMORY         = 257;
    public static final int PLP_SIM_UNHANDLED_INSTRUCTION_TYPE   = 258;
    public static final int PLP_SIM_EVALUATION_FAILED            = 259;
    public static final int PLP_SIM_STALE                        = 260;
    public static final int PLP_SIM_OUT_ADDRESS_OUT_OF_RANGE     = 261;
    public static final int PLP_SIM_OUT_UNALIGNED_MEMORY         = 262;
    public static final int PLP_SIM_MODULE_DISABLED              = 263;
    public static final int PLP_SIM_BUS_ERROR                    = 264;
    public static final int PLP_SIM_UNMAPPED_MEMORY_ACCESS       = 265;
    public static final int PLP_SIM_INVALID_MODULE               = 266;
    public static final int PLP_SIM_WRITE_TO_READONLY_MODULE     = 267;
    public static final int PLP_SIM_EVAL_ON_THREADED_MODULE      = 268;
    public static final int PLP_SIM_UNABLE_TO_LOAD_PROGRAM       = 269;
    public static final int PLP_SIM_NO_EXECUTE_VIOLATION         = 270;
    public static final int PLP_SIM_MODULE_NO_DATA_ON_READ       = 271;
    public static final int PLP_SIM_UNSUPPORTED_ARCHITECTURE     = 272;
    public static final int PLP_SIM_WRONG_MODULE_TYPE            = 273;
    public static final int PLP_SIM_MODHOOK_ERROR                = 274;
    public static final int PLP_SIM_EMPTY_PROGRAM                = 275;

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
    public static final int PLP_PRG_TIMEOUT                      = 521;
    public static final int PLP_PRG_EMPTY_PROGRAM                = 522;

    // Error codes >= 1024 will also always be printed to the terminal

    // Backend errors
    public static final int PLP_BACKEND_PLP_OPEN_ERROR           = 1024;
    public static final int PLP_BACKEND_PLP_SAVE_ERROR           = 1025;
    public static final int PLP_BACKEND_INVALID_PLP_FILE         = 1026;
    public static final int PLP_BACKEND_BOUND_CHECK_FAILED       = 1027;
    public static final int PLP_BACKEND_NO_ASSEMBLED_OBJECT      = 1028;
    public static final int PLP_BACKEND_EMPTY_ASM_LIST           = 1029;
    public static final int PLP_BACKEND_ASM_IMPORT_ERROR         = 1030;
    public static final int PLP_BACKEND_DELETING_LAST_ASM_ERROR  = 1031;
    public static final int PLP_BACKEND_IMPORT_CONFLICT          = 1032;
    public static final int PLP_BACKEND_EMPTY_PROGRAM            = 1033;
    public static final int PLP_BACKEND_THREAD_EXCEPTION         = 1034;
    public static final int PLP_BACKEND_NO_NATIVE_SERIAL_LIBS    = 1035;
    public static final int PLP_BACKEND_ASM_DOES_NOT_EXIST       = 1036;
    public static final int PLP_BACKEND_SAVE_CONFIG_FAILED       = 1037;
    public static final int PLP_BACKEND_LOAD_CONFIG_FAILED       = 1038;
    public static final int PLP_BACKEND_GUI_ON_HEADLESS_ENV      = 1039;

    public static final int PLP_DMOD_INSTANTIATION_ERROR         = 1040;
    public static final int PLP_DMOD_PATH_ERROR                  = 1041;
    public static final int PLP_DMOD_CLASS_NOT_FOUND_ERROR       = 1042;
    public static final int PLP_DMOD_ILLEGAL_ACCESS              = 1043;
    public static final int PLP_DMOD_FILE_IO_ERROR               = 1044;
    public static final int PLP_DMOD_GENERAL_ERROR               = 1045;
    public static final int PLP_DMOD_FAILED_TO_LOAD_ALL_JAR      = 1046;
    public static final int PLP_DMOD_NO_MANIFEST_FOUND           = 1047;
    public static final int PLP_DMOD_INVALID_MANIFEST_ENTRY      = 1048;
    public static final int PLP_DMOD_HOOK_EXCEPTION              = 1049;
    public static final int PLP_DMOD_UNABLE_TO_DOWNLOAD_JAR      = 1050;
    public static final int PLP_DMOD_INVALID_CLASS_INDEX         = 1051;
    public static final int PLP_DMOD_INVALID_MODULE_INDEX        = 1052;
    public static final int PLP_DMOD_MODULE_IS_ALREADY_LOADED    = 1053;

    // Framework errors
    public static final int PLP_ISA_INVALID_ARCHITECTURE_ID      = 2048;
    public static final int PLP_ISA_NO_SIM_CLI                   = 2049;
    public static final int PLP_ISA_NO_ASSEMBLER                 = 2050;
    public static final int PLP_ISA_NO_CC                        = 2051;
    public static final int PLP_ISA_NO_SIMCORE                   = 2052;
    public static final int PLP_ISA_NO_SIMCOREGUI                = 2053;
    public static final int PLP_ISA_NO_PROGRAMMER                = 2054;
    public static final int PLP_ISA_ALREADY_DEFINED              = 2055;
    public static final int PLP_ISA_INVALID_METACLASS            = 2056;

    // Develop errors
    public static final int PLP_DEV_INVALID_FILE_INDEX           = 4096;
    public static final int PLP_DEV_INVALID_GOTO_LOCATION        = 4097;

    // ProjectDriver modes
    public static final int PLP_DEFAULT                          = 0;
    public static final int PLP_GUI_START_IDE                    = 1;
    public static final int PLP_GUI_START_SIM                    = 2;
    public static final int PLP_GUI_APPLET                       = 4;
    public static final int PLP_NON_INTERACTIVE                  = 8;

    // General codes
    public static final int PLP_NUMBER_ERROR                     = -1;
    public static final int PLP_OK                               = 0;
    public static final int PLP_GENERIC_ERROR                    = 1;
    public static final int PLP_TOOLAPP_ERROR                    = 2;
    public static final int PLP_FILE_OPEN_ERROR                  = 3;
    public static final int PLP_FILE_SAVE_ERROR                  = 4;
    public static final int PLP_FILE_USE_SAVE_AS                 = 5;
    public static final int PLP_PRINT_ERROR                      = 6;
    public static final int PLP_FATAL_ERROR                      = 7;
    public static final int PLP_IO_PERMISSION_ERROR              = 8;
    public static final int PLP_GENERAL_IO_ERROR                 = 9;

    // Specific I/O errors
    public static final int PLP_IO_PERMISSION_EXCEPTION          = 9192;
    public static final int PLP_IO_FILE_DOES_NOT_EXIST           = 9193;
    public static final int PLP_IO_IS_NOT_A_DIRECTORY            = 9194;
    public static final int PLP_IO_READ_ERROR                    = 9195;
    public static final int PLP_IO_WRITE_ERROR                   = 9196;

    public static final int PLP_ERROR_RETURN                     = -1;
    public static final int PLP_OOPS                             = 9001;

    public static final int PLP_LONG_SIM                         = 100;

    // OS Constants
    public static final int PLP_OS_UNKNOWN                       = 0;
    public static final int PLP_OS_LINUX_32                      = 1;
    public static final int PLP_OS_LINUX_64                      = 2;
    public static final int PLP_OS_WIN_32                        = 4;
    public static final int PLP_OS_WIN_64                        = 8;
    public static final int PLP_OS_MACOSX                        = 16;

    // PLP Simulation Tool Frames IDs
    public static final int PLP_TOOLFRAME_IOREGISTRY             = 1;
    public static final int PLP_TOOLFRAME_ASMVIEW                = 2;
    public static final int PLP_TOOLFRAME_WATCHER                = 3;
    public static final int PLP_TOOLFRAME_SIMRUN                 = 4;
    public static final int PLP_TOOLFRAME_SIMCPU                 = 5;
    public static final int PLP_TOOLFRAME_SIMCTRL                = 6;

    public static final int PLP_TOOLFRAME_SIMLEDS                = 7;
    public static final int PLP_TOOLFRAME_SIMSWITCHES            = 8;
    public static final int PLP_TOOLFRAME_SIMUART                = 9;
    public static final int PLP_TOOLFRAME_SIMPLPID               = 10;
    public static final int PLP_TOOLFRAME_SIMSEVENSEGMENTS       = 11;
    public static final int PLP_TOOLFRAME_SIMVGA                 = 12;
    public static final int PLP_TOOLFRAME_SIMGPIO                = 13;

    // ASMView Columns identifiers
    public static final int ASMVIEW_ADDR                         = 1;
    public static final int ASMVIEW_LABEL                        = 0;

    // Programmer constants
    public static final int PLP_PRG_CHUNK_BUFFER_SIZE            = 1048576; // 1MB

    // Flags
    public static final boolean FLAGS_ALIGNED_MEMORY             = true;
}

