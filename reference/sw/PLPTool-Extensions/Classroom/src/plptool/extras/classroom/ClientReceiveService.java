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
 * @author Wira
 */
public class ClientReceiveService extends Thread {
    private Socket s;
    private BufferedReader in;
    private Client c;
    private ClientService.State state;

    public ClientReceiveService(Socket s, Client c) {
        this.s = s;
        this.c = c;
        state = ClientService.State.DISCONNECT;
    }

    @Override
    public void run() {
        String line;
        boolean disconnect = false;
        boolean online = false;
        try {
            in = new BufferedReader(new InputStreamReader(s.getInputStream()));
            while(!disconnect) {
                line = in.readLine();
                Msg.D("Received: " + line, 3, this);
                if(!online && line.equals("%%NICKNAME_OK"))
                    online = true;
                else if (line.equals("%%DISCONNECT")) {
                    if(!online)
                        Msg.W("Server refused connection. Nickname is probably" +
                                " already taken", this);
                    Msg.I("Closing connection", this);
                    online = false;
                } else if(line.equals("%%MUTE"))
                    c.setState(true, false);
                else if(line.equals("%%ACK"))
                    c.setState(true, true);

                if(!online) {
                    disconnect = true;
                    in.close();
                    c.cleanup();
                }
            }
        } catch(IOException e) {
            Msg.E("I/O error.", Constants.PLP_GENERAL_IO_ERROR, this);
        }
    }

    @Override
    public String toString() {
        return "collab.ClientReceiveService";
    }
}
