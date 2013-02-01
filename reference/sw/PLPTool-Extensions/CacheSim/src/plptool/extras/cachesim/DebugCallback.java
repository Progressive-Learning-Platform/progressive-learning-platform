/*
    Copyright 2013 Wira Mulia

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

package plptool.extras.cachesim;

import plptool.dmf.*;

import plptool.*;

/**
 *
 * @author Wira
 */
public class DebugCallback implements Callback {
    public boolean callback(int num, Object param) {
        String cmd = (String) param;
        if(cmd.equals("cachesim_init_default")) {
            Msg.M("CacheSim: Initializing default test cache");
            Log.head = new TestCache(null);
        } else if(cmd.equals("cachesim_reset")) {
            Msg.M("CacheSim: Resetting memory hierarchy statistics");
            Log.reset();
        } else if(cmd.equals("cachesim_dump_head_stats")) {
            Msg.M("CacheSim: Dump first level statistics");
            Log.head.stats.print();
            Msg.P(Log.head.dumpContents());
        } else if(cmd.equals("cachesim_configure")) {
            Msg.M("CacheSim: Creating configuration window");
            Log.frame = new DefaultCacheFrame();
            Log.frame.setVisible(true);
        }

        return true;
    }
}
