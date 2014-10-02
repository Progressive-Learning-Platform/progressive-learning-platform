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

package plptool.extras.socket;

import java.net.*;
import java.io.*;
import plptool.*;

/**
 *
 * @author Wira
 */
public class ServerListenThread extends Thread {
    private boolean run = true;

    @Override public void run() {
        while(run) {
            try {
                Msg.M("SocketInterface: ServerListenThread: listening for connections.");
                Socket client = SocketInterface.getServerSocket().accept();
                SocketInterface.addClient(client);
            } catch(IOException ioe) {
                Msg.E("SocketInterface: ServerListenThread: failed to accept connection",
                        Constants.PLP_GENERAL_IO_ERROR, null);
            }
        }
        Msg.M("SocketInterface: ServerListenThread: stopped.");
    }

    public void stopListening() {
        run = false;
    }
}
