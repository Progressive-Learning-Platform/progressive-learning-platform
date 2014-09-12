/*
    Copyright 2010-2014 David Fritz, Brian Gordon, Wira Mulia

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
import plptool.dmf.CallbackRegistry;

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
    private boolean done;
    private boolean quit;

    public SimRunner(ProjectDriver plp) {
        this.sim = plp.sim;
        this.plp = plp;
        stepCount = 1;
        setDone(false);
        quit = false;
    }

    private synchronized void setDone(boolean val) {
        done = val;
    }

    public synchronized boolean isDone() {
        return done;
    }

    public synchronized void gracefullyQuit() {
        quit = true;
    }

    @Override
    public void run() {
        Msg.lastError = 0;
        boolean breakEncountered = false;
        int quitCount = 600;
        if(plp.g())
            plp.g_err.clearError();
        startInstr = sim.getInstrCount();
        Msg.M("--- run");
        startTime = System.currentTimeMillis();
        if(plp.g())
            plp.g_dev.clearLineHighlights();

        while(stepCount > 0) {
            int steps = Config.simCyclesPerStep;
            if(steps <= plptool.Constants.PLP_MAX_STEPS && steps > 0) {
                for(int i = 0; i < steps && Msg.lastError == 0 && stepCount > 0; i++) {
                    plp.sim.stepW();
                    if(plp.sim.breakpoints.hasBreakpoint()) {
                        if(plp.sim.breakpoints.isBreakpoint(plp.sim.visibleAddr)) {
                            stepCount = 0;
                            breakEncountered = true;
                            Msg.M("--- breakpoint encountered: " + String.format("0x%02x", plp.sim.visibleAddr));
                        }
                    }
                }
                if(plp.g() && plp.getArch().hasSimCoreGUI())
                    plp.g_sim.updateComponents();
            } else {
                //if(plp.g()) plp.g_simsh.getTxtSteps().setText("1");
                steps = 1;
            }
            if(Config.simRefreshGUIDuringSimRun)
                if(plp.g())
                    plp.updateComponents(Config.simRefreshDevDuringSimRun);
            if(Msg.lastError != 0) {
                if(plp.g()) {
                    plp.g_err.setError(Msg.lastError);
                    stepCount = 0;
                    breakEncountered = true;
                }
                break;
            }
            
            try {
                this.sleep(Config.simRunnerDelay);
            } catch(Exception e) {}

            
        }

        long time = System.currentTimeMillis() - startTime;
        Msg.m("--- stop: " + (sim.getInstrCount() - startInstr) + " instructions fetched ");
        Msg.M("in " + time + " milliseconds of real time.");

        if(breakEncountered) {
            plp.g_dev.stopSimState();
            if(plp.g_simctrl != null)
                plp.g_simctrl.stopSimState();
            plp.updateComponents(true);
        }

        setDone(true);
        while(!quit && !breakEncountered) {
            try {
                Thread.sleep(50);
            } catch(Exception e) {

            }
            quitCount--;
        }
        Msg.D("SimRunner exiting", 4, null);
    }
}
