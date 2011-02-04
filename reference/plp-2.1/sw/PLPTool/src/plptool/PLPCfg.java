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
 * PLPTool configuration 
 *
 * @author wira
 */
public class PLPCfg {
    public static boolean cfgInstrPretty                   = true;

    public static String  cfgArch                          = "plpmips";
    public static boolean cfgSimForwardingUnit             = true;
    public static boolean cfgSimDynamicMemoryAllocation    = false;
    public static boolean cfgRefreshGUIDuringSimRun        = true;
    public static int     cfgSimDelay                      = 100;
    public static boolean cfgVerbose                       = true;

    public static double  relativeDefaultWindowWidth       = 0.8;
    public static double  relativeDefaultWindowHeight      = 0.8;

    public static int     threadedModRefreshRate           = 100;
    public static boolean threadedModEnabled               = true;

    public static boolean cfgSyntaxHighlighting            = false;

    public static long    cfgDefaultRAMSize                = 0x1000000;
}
