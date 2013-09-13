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

import plptool.*;
import plptool.gui.ProjectDriver;
import java.util.ArrayList;

/**
 *
 * @author Wira
 */
public class Log {
    public static int mode;
    public static int cycle;

    public static final int EDITING = 0;
    public static final int SIMULATION_RESET = 1;
    public static final int SIMULATION_IDLE = 2;
    public static final int SIMULATION_STEP = 3;

    public static ArrayList<Engine> head = new ArrayList<Engine>();
    public static ProjectDriver plp;
    public static ArrayList<DefaultCacheFrame> frames = new ArrayList<DefaultCacheFrame>();

    public static void reset() {       
        for(Engine e : head) {
            e.logReset();
        }
        cycle = 0;
    }

    public static void read(long addr, long val) {
        for(Engine e : head) {
            e.read(addr, val);
        }
    }

    public static void write(long addr, long val) {
        for(Engine e : head) {
            e.write(addr, val);
        }
    }
    
    public static void spawnCacheFrame() {
        DefaultCacheFrame f = new DefaultCacheFrame();
        f.setIconImage(new javax.swing.ImageIcon(PLPToolbox.getTmpDir() + "/plptool.extras.cachesim.icon.png").getImage());
        frames.add(f);
        f.setVisible(true);
        Msg.M("CacheSim Log: Creating a cache simulator");
    }
    
    public static void destroyCacheFrame(DefaultCacheFrame f) {        
        DefaultCacheFrame frame;
        for(int i = 0; i < frames.size(); i++) {
            frame = frames.get(i);
            if(frame.equals(f)) {
                frames.remove(f);
                Msg.M("CacheSim Log: Destroying a cache simulator");
            }
        }
        
        if(f.e != null) {
            for(int i = 0; i < Log.head.size(); i++) {
                if(Log.head.get(i).equals(f.e)) {
                    Log.head.remove(i);
                    Msg.M("CacheSim: Removing cache engine from the list...");
                }
            }
        }
        
        f.dispose();
    }
    
    public static void updateFrames() {
        for(DefaultCacheFrame frame : frames) {
            frame.update();
        }
    }
}
