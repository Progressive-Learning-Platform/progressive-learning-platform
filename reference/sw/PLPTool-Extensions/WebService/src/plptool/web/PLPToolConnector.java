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

import plptool.*;
import plptool.dmf.*;
import plptool.gui.*;

/**
 *
 * @author Wira
 */
public class PLPToolConnector implements ModuleInterface5 {

    public String getName() {
        return "PLP WebService";
    }

    public String getDescription() {
        return "This module provides web services for PLP web based functions.";
    }

    public int[] getVersion() {
        int[] ver = {1, 0};
        return ver;
    }

    public int[] getMinimumPLPToolVersion() {
        int[] ver = {5, 0};
        return ver;
    }

    public int initialize(ProjectDriver plp) {
        if(PLPToolApp.getAttributes().containsKey("Web_Help")) {
            Msg.P("");
            Msg.P("PLPTool WebService Usage:");
            Msg.P("   -PWeb_HTTP_Test        Run test web service");
            Msg.P("   -PWeb_Frontend         Run HTML5 PLPTool frontend ONLY");
            Msg.P("   -PWeb_Classroom        Run PLP Classroom Web Backend");
            Msg.P("   -PWeb_Db::<address>:<port>");
            Msg.P("                          PostgreSQL backend server to use for persistent store");
            Msg.P("   -PWeb_Db_Cfg::<file>   Load database configuration from <file>");
            Msg.P("   -PWeb_Db_User::<user>  Use <user> to connect to the database");
            Msg.P("   -PWeb_Db_Password::<password>");
            Msg.P("                          Use <password> to connect to the database");
            Msg.P("   -PWeb_Db_Init          (Re-)initialize the backend database");
            Msg.P("   -PWeb_Db_Dump::<file>  Dump database contents to a zip <file>");
            Msg.P("   -PWeb_Db_Import::<file>");
            Msg.P("                          Import data from <file> to the database");
            Msg.P("   -PWeb_Port::<port>     Listen to <port> instead of the default 8080");
            Msg.P("");
        } else {
            CallbackRegistry.register(new HeadlessCallback(), CallbackRegistry.EVENT_HEADLESS_START);
        }
        return Constants.PLP_OK;
    }
}
