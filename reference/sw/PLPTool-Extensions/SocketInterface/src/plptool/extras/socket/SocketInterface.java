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

import plptool.*;

import java.io.*;
import java.net.*;
import java.util.ArrayList;

/**
 *
 * @author Wira
 */
public class SocketInterface {
    private static int port;
    private static ServerSocket serverSocket;
    private static ArrayList<ClientHandler> clients;
    private static ServerListenThread serverListenThread;

    public static void setPort(int port) {
        Msg.M("SocketInterface: port to listen to = " + port);
        SocketInterface.port = port;
    }

    public static ServerSocket getServerSocket() {
        return serverSocket;
    }

    public static void init() {
        try {
            serverSocket = new ServerSocket(port);
        } catch(IOException ioe) {
            Msg.E("SocketInterface: failed to open socket for port " + port,
                    Constants.PLP_GENERAL_IO_ERROR, null);
        }

        clients = new ArrayList<ClientHandler>();
        serverListenThread = new ServerListenThread();
        serverListenThread.start();
    }

    public static void addClient(Socket client) {
        clients.add(new ClientHandler(client));

    }
}
