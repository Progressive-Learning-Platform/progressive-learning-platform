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
import java.io.FileReader;
import java.io.File;
import plptool.Constants;

/**
 *
 * @author wira
 */
public class PLPCC {

    static native String nativeCompile(int logLevel, String input);
    static native String getVersion();

    static {
        System.loadLibrary("plpcclib");
    }

    public static void main(String args[]) {
        System.out.println("PLPCC Java Interface");
		String str = "";
        String version = PLPCC.getVersion();

        if(args.length != 1) return;

	try {
		System.out.println("Compiling " + args[0]);
		File f = new File(args[0]);

		if(!f.exists()) return;

		FileReader in = new FileReader(f);
		char[] buf = new char[(int) f.length()];
		in.read(buf, 0, buf.length);
		in.close();
		str = new String(buf);

	} catch(Exception e) {

	}

        System.out.println("PLPCC-JNI Version: " + version);
        String assembly = PLPCC.nativeCompile(2, str);

        System.out.println(assembly);
    }
}
