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

package org.plp.web.services;

import plptool.*;
import plptool.gui.*;

import java.io.*;
import java.net.*;
import java.util.*;

import org.plp.web.*;

import com.json.parsers.*;

import com.sun.net.httpserver.*;

/**
 *
 * @author Wira
 */
public class Handlers {

    private static JSONParser parser = new JSONParser();
    public static String root = "./";

    static class PLPAsmHandler implements HttpHandler {
        public void handle(HttpExchange t) {
            try {
                String command;
                Map<String, String> data = parser.parseJson(Utils.getQueryFromExchange(t));
                if(!data.containsKey("command")) {
                    Msg.E("No command field in JSON request", org.plp.web.Constants.PLPWEB_MISSING_COMMAND_FIELD, null);
                }
                command = data.get("command");

            } catch(IOException ioe) {
                Msg.E("No data from exchange", org.plp.web.Constants.PLPWEB_INVALID_POST_REQUEST, null);
            }
        }
    }
}
