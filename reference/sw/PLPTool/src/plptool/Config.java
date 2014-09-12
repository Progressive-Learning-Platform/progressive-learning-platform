/*
    Copyright 2010-2014 David Fritz, Brian Gordon, Wira Mulia

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

import java.awt.Color;

/**
 * PLPTool configuration 
 *
 * @author wira
 */
public class Config {
    public static boolean cfgInstrPretty                   = true;

    public static String  cfgArch                          = "plpmips";

    public static boolean hostIsLinux                      = false;
    public static boolean cfgAskBeforeAutoloadingModules   = true;
    public static int     cfgOverrideISA                   = -1;
    public static boolean cfgAskForISAForNewProjects       = false;
    public static boolean cfgErrorDialog                   = false;

    public static boolean simForwardingUnit                = true;
    public static boolean simDynamicMemoryAllocation       = false;
    public static boolean simRefreshGUIDuringSimRun        = true;
    public static boolean simRefreshDevDuringSimRun        = false;
    public static int     simRunnerDelay                   = 100;
    public static boolean cfgVerbose                       = true;
    public static boolean simBusReturnsZeroForUninitRegs   = true;
    public static boolean simAllowExecutionOfArbitraryMem  = true;
    public static boolean simDumpTraceOnFailedEvaluation   = false;
    public static long    simDefaultRAMSize                = 0x1000000;
    public static long    simRAMStartingAddress            = 0x10000000;
    public static boolean simIgnoreSavedSimState           = false;
    public static int     simCyclesPerStep                 = 1;
    public static boolean simHighlightLine                 = false;
    public static boolean simFunctional                    = true;

    public static double  relativeDefaultWindowWidth       = 0.8;
    public static double  relativeDefaultWindowHeight      = 0.8;

    public static int     threadedModRefreshRate           = 100;
    public static boolean threadedModEnabled               = true;

    public static boolean prgProgramInChunks               = true;
    public static int     prgMaxChunkSize                  = 2048; //16384;
    public static int     prgReadTimeout                   = 500;
    public static boolean prgNexys3ProgramWorkaround       = true;
    public static boolean prgAutoDetectPorts               = true;

    public static boolean devNewSyntaxHighlightStrategy    = true;
    public static long    devHighlighterThreadRefreshMsecs = 1000;
    public static boolean devSyntaxHighlighting            = true;
    public static boolean devSyntaxHighlightOnAssemble     = false;
    public static boolean highlighting                     = false;
    public static int     filetoolarge                     = 10000;

    public static int     maxUndoLength                    = 20;

    public static String  devFont                          = "Monospaced";
    public static int     devFontSize                      = 12;
    public static Color   devBackground                    = Color.WHITE;
    public static Color   devForeground                    = Color.BLACK;

    public static int     devMaxUndoEntries                = 64000;

    public static boolean devWindowSettingSet              = false;
    public static int     devWindowPositionX               = -1;
    public static int     devWindowPositionY               = -1;
    public static int     devWindowWidth                   = -1;
    public static int     devWindowHeight                  = -1;

    public static int     serialTerminalReadDelayMs        = 10;
    public static int     serialTerminalBufferSize         = 64;
    public static boolean serialTerminalAutoDetectPorts    = true;
   
    public static void resetConfig() {
        cfgInstrPretty                   = true;

        cfgArch                          = "plpmips";

        hostIsLinux                      = false;
        cfgAskBeforeAutoloadingModules   = true;

        simForwardingUnit                = true;
        simDynamicMemoryAllocation       = false;
        simRefreshGUIDuringSimRun        = true;
        simRefreshDevDuringSimRun        = false;
        simRunnerDelay                   = 100;
        cfgVerbose                       = true;
        simBusReturnsZeroForUninitRegs   = true;
        simAllowExecutionOfArbitraryMem  = true;
        simDumpTraceOnFailedEvaluation   = false;
        simDefaultRAMSize                = 0x1000000;
        simRAMStartingAddress            = 0x10000000;
        simIgnoreSavedSimState           = false;
        simCyclesPerStep                 = 1;
        simHighlightLine                 = false;
        simFunctional                    = true;

        relativeDefaultWindowWidth       = 0.8;
        relativeDefaultWindowHeight      = 0.8;

        threadedModRefreshRate           = 100;
        threadedModEnabled               = true;

        prgProgramInChunks               = true;
        prgMaxChunkSize                  = 2048; //16384;
        prgReadTimeout                   = 500;
        prgNexys3ProgramWorkaround       = true;
        prgAutoDetectPorts               = true;

        devNewSyntaxHighlightStrategy    = true;
        devHighlighterThreadRefreshMsecs = 1000;
        devSyntaxHighlighting            = true;
        devSyntaxHighlightOnAssemble     = false;
        highlighting                     = false;
        filetoolarge                     = 10000;

        maxUndoLength                    = 20;

        devFont                          = "Monospaced";
        devFontSize                      = 12;
        devBackground                    = Color.WHITE;
        devForeground                    = Color.BLACK;

        devMaxUndoEntries                = 64000;

        serialTerminalReadDelayMs        = 10;
        serialTerminalBufferSize         = 64;
        serialTerminalAutoDetectPorts    = true;
    }
}
