/*
    Copyright 2011 David Fritz, Brian Gordon, Wira Mulia

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

package plptool.interfaces;

import java.util.*;
import plptool.Constants;

/**
 *
 * @author wira
 */
public class PLPCC {

    native String nativeCompile(String input);
    native String testLink();

    static {
        System.loadLibrary("plpcclib");
    }

    public static void main(String args[]) {
        System.out.println("PLPCC Java Interface");

        if(args.length == 0) return;

        String str =
                "int depth = 10;\n" +
                "int fib(int fn1, int fn2) {\n" +
                "   int fn0 = fn1 + fn2;\n" +
                "   depth--;\n" +
                "   if (depth == 0)\n" +
                "       return fn0;\n" +
                "   else\n" +
                "       return fib(fn0, fn1);\n" +
                "}" +
                "\n\n" +
                "void main(void) {\n" +
                "    fib(1,0);\n" +
                "}\n";


        PLPCC cc = new PLPCC();
        String test = cc.testLink();
        System.out.println(test);
        String assembly = cc.nativeCompile(str);

        System.out.println(assembly);
    }
}
