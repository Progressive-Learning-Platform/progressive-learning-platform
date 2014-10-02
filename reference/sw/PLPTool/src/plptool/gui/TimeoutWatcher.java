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

import plptool.Msg;
import plptool.Constants;

/**
 *
 * @author wira
 */
public class TimeoutWatcher extends Thread {

    private ProjectDriver plp;

    public TimeoutWatcher(ProjectDriver plp) {
        this.plp = plp;
    }

    @Override
    public void run() {
        int timeoutCounter = 0;
        int oldProgress = 0;

        try {

        while(plp.prg.isAlive() && timeoutCounter != 20) {
            if(plp.prg.progress == oldProgress)
                timeoutCounter++;
            else
                timeoutCounter = 0;

            oldProgress = plp.prg.progress;

            Thread.sleep(50);
        }

        if(timeoutCounter == 20) {
            Msg.E("Programming timed out, killing programmer thread.",
                     Constants.PLP_PRG_TIMEOUT, this);
            //plp.p_port.getInputStream().close();
            //plp.prg.interrupt();
            //plp.p_port.close();
            plp.prg.cancelProgramming();
            plp.prg.stop();
            Msg.I("Watchdog quitting.", this);
        }

        } catch(Exception e) {
            Msg.E("Uh oh I crashed.", Constants.PLP_OOPS, this);
        }
    }

    @Override
    public String toString() {
        return "TimeoutWatcher";
    }
}
