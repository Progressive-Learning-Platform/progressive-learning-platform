/*
    Copyright 2013 PLP Contributors

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

package plptool;

/**
 *
 * @author Wira
 */
public class ThreadSync {
    private boolean stop;
    private String text;
    private Object param;

    public ThreadSync() {
        stop = false;
        text = "";
        param = null;
    }

    public synchronized void stop() {
        stop = true;
    }

    public synchronized void start() {
        stop = false;
        text = "";
        param = null;
    }

    public synchronized boolean isStopped() {
        return stop;
    }

    public synchronized void setString(String str) {
        text = str;
    }

    public synchronized String getString() {
        return text;
    }

    public synchronized void setParam(Object p) {
        param = p;
    }

    public synchronized Object getParam() {
        return param;
    }
}
