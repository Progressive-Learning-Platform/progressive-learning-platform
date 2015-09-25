/*
    Copyright 2014 PLP Contributors

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

import java.awt.Robot;
import java.util.HashMap;

import plptool.PLPToolbox;
import plptool.gui.ProjectDriver;
import plptool.testsuite.AutoTest;
import plptool.testsuite.Tester;

/**
 *
 * @author wira
 */
public class ConfigTester implements Tester
{
	public void configure(Robot r)
	{
	}
	
	public void run(ProjectDriver plp)
	{
		HashMap<String, String> config = new HashMap<String, String>();
		
		config.put("config1", "7000");
		config.put("config2", "true");
		config.put("config3", "somestring");
		
		PLPToolbox.writeConfig(config, PLPToolbox.getConfDir() + "/configtest");
		
		HashMap<String, String> configRead = PLPToolbox.parseConfig(PLPToolbox
				.getConfDir() + "/configtest");
		
		AutoTest.p("SUCCESS");
		System.exit(0);
	}
}
