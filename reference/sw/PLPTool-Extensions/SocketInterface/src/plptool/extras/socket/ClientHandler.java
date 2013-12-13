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
import plptool.gui.ProjectDriver;

/**
 *
 * @author Wira
 */
public class ClientHandler {
    private InputStreamReader in;
    private OutputStreamWriter out;

    public ClientHandler(Socket socket) {
        try {
            in = new InputStreamReader(socket.getInputStream());
            out = new OutputStreamWriter(socket.getOutputStream());
        } catch(IOException ioe) {
            Msg.E("SocketInterface: ClientHandler: failed to open streams",
                    Constants.PLP_GENERAL_IO_ERROR, null);
        }
    }
}
