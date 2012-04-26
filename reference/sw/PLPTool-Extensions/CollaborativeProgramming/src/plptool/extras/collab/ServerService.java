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

package plptool.extras.collab;

import plptool.*;
import plptool.gui.ProjectDriver;
import java.io.*;
import java.net.*;
import java.util.HashMap;
import javax.swing.text.StyledDocument;
import javax.swing.text.BadLocationException;

/**
 *
 * @author Wira
 */
public class ServerService extends Thread {
    private HashMap<Integer, ClientService> clientServices;
    private ProjectDriver plp;
    private int clientIndex;
    private boolean listening;
    private ServerSocket socket;
    private ServerControl control;

    public ServerService(ServerControl control, ProjectDriver plp,
            int port, int maxClients) {
        this.control = control;
        this.plp = plp;
        clientServices = new HashMap<Integer, ClientService>();
        clientIndex = 0;
        listening = false;
        try {
            socket = new ServerSocket(port);
        } catch(IOException e) {
            Msg.E("Unable to open port " + port, Constants.PLP_GENERAL_IO_ERROR,
                    null);
        }
    }

    public synchronized void deliverData(String text) {
        StyledDocument doc = plp.g_dev.getEditor().getStyledDocument();
        int offset = plp.g_dev.getEditor().getCaretPosition();
        try {
            doc.insertString(offset, text, null);
        } catch(BadLocationException ble) {
            Msg.E("Collab: Text insert error", Constants.PLP_GENERIC_ERROR,
                    null);
        }
    }

    public void setListening(boolean b) {
        listening = b;
    }

    public ServerControl getControls() {
        return control;
    }

    @Override
    public void run() {
        while(listening) {
            try {
                Socket client = socket.accept();
                ClientService c = new ClientService(clientIndex, this, client);
                clientServices.put(clientIndex, c);
                clientIndex++;
                c.start();
                control.update();
            } catch(IOException e) {
                Msg.E("Unable to open connection",
                        Constants.PLP_GENERIC_ERROR, null);
            }
        }
        Msg.I("Collab server service exiting.", null);
    }

    public synchronized void removeClient(int ID) {
        
    }
}

class ClientService extends Thread {
    private ServerService server;
    private Socket socket;
    private int ID;
    private boolean disconnect = false;

    public ClientService(int ID, ServerService server, Socket socket) {
        this.ID = ID;
        this.server = server;
        this.socket = socket;
    }

    public void disconnect() {
        disconnect = true;
    }

    @Override
    public void run() {
        try {
            BufferedReader in = new BufferedReader(new InputStreamReader(
                    socket.getInputStream()));
            while(!disconnect) {
                server.deliverData(in.readLine());
            }
        } catch(IOException e) {
            Msg.E("Collab CLIENT[" + ID + "]: I/O error.",
                    Constants.PLP_GENERAL_IO_ERROR, null);
        }
        try {
            socket.close();
        } catch(IOException e) {
            Msg.E("Collab CLIENT[" + ID + "]: unable to close connection.",
                    Constants.PLP_GENERAL_IO_ERROR, null);
        }
        Msg.I("Collab CLIENT[" + ID + "]: service exiting.", null);
    }
}