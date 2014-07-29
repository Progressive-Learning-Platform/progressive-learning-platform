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

package org.plp.web;

import java.io.*;
import com.sun.net.httpserver.*;
import plptool.PLPToolbox;

/**
 *
 * @author Wira
 */
public class Utils {
    public static String getQueryFromExchange(HttpExchange t) throws IOException {
        String qry = null;

        // http://stackoverflow.com/questions/3409348/read-post-request-values-httphandler
        String encoding = "ISO-8859-1";

        // read the query string from the request body
        InputStream in = t.getRequestBody();
        try {
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            byte buf[] = new byte[4096];
            for (int n = in.read(buf); n > 0; n = in.read(buf)) {
                out.write(buf, 0, n);
            }
            qry = new String(out.toByteArray(), encoding);
        } finally {
            in.close();
        }

        return qry;
    }

    public static String readFile(String path) throws IOException {
        String str = null;
        str = PLPToolbox.readFileAsString(path);
        if(str == null)
            throw new IOException();

        return str;
    }
}
