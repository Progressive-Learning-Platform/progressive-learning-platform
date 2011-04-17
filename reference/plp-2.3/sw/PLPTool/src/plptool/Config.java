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

import java.awt.Color;

/**
 * PLPTool configuration 
 *
 * @author wira
 */
public class Config {
    public static boolean cfgInstrPretty                   = true;

    public static String  cfgArch                          = "plpmips";
    public static boolean simForwardingUnit                = true;
    public static boolean simDynamicMemoryAllocation       = false;
    public static boolean simRefreshGUIDuringSimRun        = true;
    public static int     simRunnerDelay                   = 100;
    public static boolean cfgVerbose                       = true;
    public static boolean simBusReturnsZeroForUninitRegs   = true;
    public static boolean simAllowExecutionOfArbitraryMem  = true;
    public static boolean simDumpTraceOnFailedEvaluation   = false;

    public static double  relativeDefaultWindowWidth       = 0.8;
    public static double  relativeDefaultWindowHeight      = 0.8;

    public static int     threadedModRefreshRate           = 100;
    public static boolean threadedModEnabled               = true;

    public static boolean cfgSyntaxHighlighting            = true;
    public static boolean nothighlighting                  = true;
    public static int     filetoolarge                     = 1000;

    public static int     maxUndoLength                    = 20;

    public static long    cfgDefaultRAMSize                = 0x1000000;

    public static String  devFont                          = "Monospaced";
    public static int     devFontSize                      = 12;
    public static Color   devBackground                    = Color.WHITE;
    public static Color   devForeground                    = Color.BLACK;

    //                                                        RTYPE              ITYPE              BRANCH             JUMP               MEMTYPE            NOP                    REG                IMM                  LABEL            COMMENT              SYS
    public static Color[]   syntaxColors                   = {new Color(0,0,255),new Color(0,0,255),new Color(0,0,255),new Color(0,0,255),new Color(0,0,255),new Color(127,127,127),new Color(255,0,0),new Color(255,153,0),new Color(0,0,0),new Color(0, 153, 0),new Color(204, 0, 102)};
    public static boolean[] syntaxBold                     = {false,             false,             false,             false,             false,             false,                 false,             false,               true,            false,               false};
    public static boolean[] syntaxItalic                   = {false,             false,             false,             false,             false,             false,                 false,             false,               false,           false,               false};
}
