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

package plptool.gui;

import plptool.PLPSimCore;
import plptool.Msg;
import plptool.Config;

/**
 *
 * @author wira
 */
public class SimRunner extends Thread {

    private PLPSimCore sim;
    private ProjectDriver plp;
    public int stepCount;
    private int startInstr;
    private long startTime;

    public SimRunner(ProjectDriver plp) {
        this.sim = plp.sim;
        this.plp = plp;
        stepCount = 1;
    }

    @Override
    public void run() {
        Msg.lastError = 0;
        plp.g_err.clearError();
        startInstr = sim.getinstrcount();
        startTime = System.currentTimeMillis();

        while(stepCount > 0) {
            int steps = Integer.parseInt(plp.g_simsh.getTxtSteps().getText());
            if(steps <= plptool.Constants.PLP_MAX_STEPS && steps > 0) {
                for(int i = 0; i < steps; i++)
                    plp.sim.step();
                plp.g_sim.updateComponents();
            } else {
                plp.g_simsh.getTxtSteps().setText("1");
                steps = 1;
            }
            if(Config.cfgRefreshGUIDuringSimRun)
                plp.updateComponents();
            if(Msg.lastError != 0) {
                plp.g_err.setError(Msg.lastError);
                break;
            }
            try {
                this.sleep(Config.cfgSimDelay);
            } catch(Exception e) {}
        }

        long time = System.currentTimeMillis() - startTime;
        Msg.m("SimRunner: " + (sim.getinstrcount() - startInstr) + " instructions issued ");
        Msg.M("in " + time + " milliseconds of real time.");

        plp.g_simsh.unselectTglRun();
    }
}
