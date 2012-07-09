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

package plptool.extras.classroom;

import plptool.*;
import java.io.*;
import java.net.*;

/**
 *
 * @author wira
 */
public class ClientService extends Thread {
    private ServerService server;
    private Socket socket;
    private int ID;
    private State state;
    private boolean disconnect = false;
    private String name;
    BufferedReader in;
    PrintWriter out;

    public enum State {
        DISCONNECT,
        NEED_NICK,
        WAITING_ON_NICK,
        MUTED,
        PARTICIPATING,
        LIVE,
        WAITING_ON_TEXT
    }

    public ClientService(int ID, ServerService server, Socket socket,
            String name) {
        this.ID = ID;
        this.server = server;
        this.socket = socket;
        this.name = name;
        state = State.NEED_NICK;
    }

    public int getClientID() {
        return ID;
    }

    public String getClientName() {
        return name;
    }

    public void setClientName(String name) {
        this.name = name;
    }

    public String getClientIP() {
        return socket.getInetAddress().toString();
    }

    public synchronized void disconnect() {
        disconnect = true;
    }

    public void forceDisconnect() {
        cmd("DISCONNECT");
    }

    @Override
    public void run() {
        String line;
        try {
            in = new BufferedReader(new InputStreamReader(
                    socket.getInputStream()));
            out = new PrintWriter(socket.getOutputStream(), true);
            while(state != State.DISCONNECT) {
                line = in.readLine();
                if(line == null) {
                    cmd("DISCONNECT");
                    state = State.DISCONNECT;
                    break;
                }
                
                if(line.startsWith("%%") && !(state != State.WAITING_ON_TEXT))
                    Msg.D("Received command: " + line, 3, this);
                
                if(line.equals("%%ENDTEXT")) {
                    state = State.MUTED;
                    cmd("MUTE");
                } else if (line.equals("%%TEXT")) {
                    state = State.WAITING_ON_TEXT;
                } else if(line.equals("%%QUIT")) {
                    state = State.DISCONNECT;
                } else if(line.equals("%%REQ")) {
                    cmd(server.arbitrate() ? "ACK" : "MUTE");
                } else if(line.equals("%%NICK")) {
                    state = State.WAITING_ON_NICK;
                } else {
                    switch(state) {
                        case WAITING_ON_TEXT:
                            server.deliverData(line + "\n");
                            break;
                        case WAITING_ON_NICK:
                            Msg.D("Nickname request: " + line, 2, this);
                            if(!server.checkNickname(line)) {
                                Msg.I("Nickname '" + line + "' is already taken", this);
                                state = State.DISCONNECT;
                            } else {
                                cmd("NICKNAME_OK");
                                this.setClientName(line);
                                server.updateControl();
                            }
                            state = State.MUTED;
                            cmd("MUTE");
                            break;
                        
                        // this is where the action is
                        case LIVE:
                            break;
                    }
                }
            }
        } catch(IOException e) {
            Msg.E("I/O error.",
                    Constants.PLP_GENERAL_IO_ERROR, this);
        }
        try {
            cmd("DISCONNECT");
            in.close();
            socket.close();
        } catch(IOException e) {
            Msg.E("Unable to close port", Constants.PLP_GENERAL_IO_ERROR, this);
        }
        server.removeClient(ID);
        Msg.D("service exiting.", 2, this);
    }

    public void cmd(String cmd) {
        out.write("%%" + cmd + "\n");
        out.flush();
    }

    public String toString() {
        return "collab.ClientService[" + ID + "]";
    }
}
