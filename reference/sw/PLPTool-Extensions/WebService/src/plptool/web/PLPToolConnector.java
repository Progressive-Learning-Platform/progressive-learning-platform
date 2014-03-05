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
			Msg.M("");
			Msg.M("PLPTool WebService Usage:");
			Msg.M("   -PWeb_HTTP_Test        Run test web service");
			Msg.M("   -PWeb_Frontend         Run HTML5 PLPTool frontend ONLY");
            Msg.M("   -PWeb_Classroom        Run PLP Classroom Web Backend");
            Msg.M("   -PWeb_Db::[address]:[port]");
            Msg.M("                          PostgreSQL backend server to use for persistent store");
            Msg.M("   -PWeb_Db_Cfg::[file]   Load database configuration from [file]");
            Msg.M("   -PWeb_Db_User::[user]  Use [user] to connect to the database");
            Msg.M("   -PWeb_Db_Password::[password]");
            Msg.M("                          Use [password] to connect to the database");
            Msg.M("   -PWeb_Db_Init          (Re-)initialize the backend database");
            Msg.M("   -PWeb_Db_Dump::[file]  Dump database contents to a zip [file]");
            Msg.M("   -PWeb_Db_Import::[file]");
            Msg.M("                          Import data from [file] to the database");
			Msg.M("   -PWeb_Port::[port]     Listen to [port] instead of the default 8080");
			Msg.M("");
		} else {
	        CallbackRegistry.register(new HeadlessCallback(), CallbackRegistry.EVENT_HEADLESS_START);
		}
        return Constants.PLP_OK;
    }
}
