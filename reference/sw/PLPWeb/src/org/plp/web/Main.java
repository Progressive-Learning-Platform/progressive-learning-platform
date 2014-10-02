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

import plptool.*;
import org.plp.web.services.*;

/**
 *
 * @author wira
 */
public class Main {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        Msg.P("\n" + org.plp.web.Text.copyrightString + "\n");
        Msg.P(plptool.Text.copyrightString + "\n");

        int activeArgIndex = 0;

        if(args.length == 0) {
            printHelpMessage();
            System.exit(plptool.Constants.PLP_OK);
        }

        for(int i = 0; i < args.length; i++) {
            Msg.D("args[" + i + "] parsing: " + args[i], 4, null);

            if(args[i].equals("--help") || args[i].equals("-h")) {
                printHelpMessage();
                System.exit(plptool.Constants.PLP_OK);
            } else if(args[i].equals("--test")) {
                int ret;
                Msg.P("Loading PostgreSQL JDBC Driver...");
                if((ret = DatabaseConnector.init()) != plptool.Constants.PLP_OK) {
                    System.exit(ret);
                }
                org.plp.web.services.HTTPTest.start();
                activeArgIndex++;
            } else if(args[i].equals("--about")) {
                Msg.M(plptool.Text.licenseBanner + "\n");
                Msg.M(plptool.Text.thirdPartyCopyrightString + "\n");
                Msg.M(plptool.Text.contactString + "\n");
                System.exit(plptool.Constants.PLP_OK);
            } else if(args.length >= activeArgIndex + 2 && args[i].equals("--root")) {
                Handlers.root = args[i+1];
                activeArgIndex += 2;
                i++;
            }
        }
    }

    public static void printHelpMessage() {
        Msg.P("PLP Web Service Usage:");
        Msg.P("   --test                 Run test web service");
        Msg.P("   --ide                  Run HTML5 PLPTool frontend ONLY");
        Msg.P("   --backend              Run PLP Classroom Web Backend");
        Msg.P("   --root <path>          Path to HTML root (default current directory)");
        Msg.P("   --port <port>          Listen to <port> instead of the default 8080");
        Msg.P("   --db <address> <port>  PostgreSQL backend server to use for persistent store");
        Msg.P("   --dbconfig <file>      Load database configuration from <file>");
        Msg.P("   --dbuser <user>        Use <user> to connect to the database");
        Msg.P("   --dbpw <password>      Use <password> to connect to the database");
        Msg.P("   --dbinit               (Re-)initialize the backend database");
        Msg.P("   --dbdump <file>        Dump database contents to a zip <file>");
        Msg.P("   --dbimport <file>      Import data from <file> to the database");        
        Msg.P("");
    }
}
