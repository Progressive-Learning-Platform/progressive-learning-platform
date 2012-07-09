/*
    Copyright 2012 PLP Contributors

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

package plptool.collab.server;

/**
 *
 * @author Wira
 */
public class Session {
    private Server srv;
    private ClientList clients;
    private ClientHandle controller;
    private int activitySlots;
    private String banner;
    private String password;

    public enum Activity {
        SUSPENDED, IDLE, PICK, TEXT, LIVETEXT
    }

    private Activity a;

    public Session(Server srv, ClientHandle controller) {
        this.srv = srv;
        this.controller = controller;
        clients = new ClientList();
        activitySlots = 0;
        a = Activity.IDLE;
        banner = "";
        password = null;
    }

    public ClientList getClientList() {
        return clients;
    }

    public int getID() {
        return controller.getNumID();
    }

    public synchronized boolean joinActivity(ClientHandle c) {
        if(activitySlots == 0)
            return false;
        activitySlots--;
        return true;
    }

    public synchronized void setActivitySlots(int slots) {
        /* solicit participation */
        activitySlots = slots;
    }

    public synchronized void setBanner(String d) {
        this.banner = d;
    }

    public String getBanner() {
        return banner;
    }

    public synchronized void setPassword(String d) {
        this.password = d;
    }

    public String getPassword() {
        return password;
    }

    public ClientHandle getController() {
        return controller;
    }

    public void destroy() {
        clients.setRoleForAll(ClientHandle.Role.LOBBY);
        clients.setStateForAll(ClientHandle.State.SLEEP);
        controller.setRole(ClientHandle.Role.LOBBY);
        controller.setState(ClientHandle.State.SLEEP);
        clients.sendAll("# Session has been destroyed. You're now in the lobby");
    }

    public synchronized void stopActivity() {
        a = Activity.IDLE;
        clients.setStateForAll(ClientHandle.State.SLEEP);
        clients.queryAll(Server.Query.SLEEP);
        activitySlots = 0;
    }

    public synchronized void stopSoliciation() {
        activitySlots = 0;
    }

    public Activity getCurrentActivity() {
        return a;
    }

    public synchronized void setCurrentActivity(Activity a) {
        this.a = a;
    }

    class ParticipationTimer extends Thread {
        private int time;

        public ParticipationTimer(int time) {
            this.time = time;
        }
    }
}
