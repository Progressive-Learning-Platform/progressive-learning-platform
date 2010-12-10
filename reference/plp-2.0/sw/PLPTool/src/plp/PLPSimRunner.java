/*
    Copyright 2010 David Fritz, Brian Gordon, Wira Mulia

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

package plp;

import plptool.PLPSimCore;
import plptool.PLPMsg;
import plptool.PLPCfg;

/**
 *
 * @author wira
 */
public class PLPSimRunner extends Thread {

    private PLPSimCore sim;
    private PLPToolView mainWindow;
    public int stepCount;
    private int startInstr;
    private long startTime;

    public PLPSimRunner(PLPSimCore sim, PLPToolView mainWindow) {
        this.sim = sim;
        this.mainWindow = mainWindow;
        stepCount = 1;
    }

    @Override
    public void run() {
        PLPMsg.lastError = 0;
        mainWindow.getErrFrame().clearError();
        startInstr = sim.getinstrcount();
        startTime = System.currentTimeMillis();

        while(stepCount > 0) {
            sim.step();
            if(PLPCfg.cfgRefreshGUIDuringSimRun)
                mainWindow.updateComponents();
            if(PLPMsg.lastError != 0) {
                mainWindow.getErrFrame().setError(PLPMsg.lastError);
                break;
            }
            try {
                this.sleep(PLPCfg.cfgSimDelay);
            } catch(Exception e) {}
        }

        long time = System.currentTimeMillis() - startTime;
        PLPMsg.m("SimRunner: " + (sim.getinstrcount() - startInstr) + " instructions issued ");
        PLPMsg.M("in " + time + " milliseconds of real time.");

        mainWindow.unselectTglRun();
    }
}
