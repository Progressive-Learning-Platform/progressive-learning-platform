/*
    Copyright 2014 Wira Mulia

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

package plptool.web;

import java.util.regex.*;

/**
 *
 * @author wira
 */
public class Utils {
    public static String resolveEscapedChars(String in) {
        String ret = "";
        ret = in.replaceAll(Pattern.quote("%3A"), ":");
        ret = ret.replaceAll(Pattern.quote("%0D%0A"), "\n");
        ret = ret.replaceAll(Pattern.quote("+"), " ");
        ret = ret.replaceAll(Pattern.quote("%22"), "\"");
        ret = ret.replaceAll(Pattern.quote("%25"), "%");
        ret = ret.replaceAll(Pattern.quote("%2C"), Pattern.quote(","));
        ret = ret.replaceAll(Pattern.quote("%24"), Pattern.quote("$"));        

        return ret;
    }
}
