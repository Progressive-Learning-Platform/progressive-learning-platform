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
    private ServerSocket socket;
    private ServerControl control;

    public ServerService(ServerControl control, ProjectDriver plp,
            int port, int maxClients) {
        this.control = control;
        this.plp = plp;
        clientServices = new HashMap<Integer, ClientService>();
        clientIndex = 0;

        try {
            socket = new ServerSocket(port);
        } catch(IOException e) {
            Msg.E("Unable to open port " + port, Constants.PLP_GENERAL_IO_ERROR,
                    null);
            if(Constants.debugLevel >= 2) e.printStackTrace();
        }
    }

    public synchronized void deliverData(String text) {
        StyledDocument doc = plp.g_dev.getEditor().getStyledDocument();
        int offset = plp.g_dev.getEditor().getCaretPosition();
        try {
            doc.insertString(offset, text, null);
            plp.g_dev.getEditor().setCaretPosition(offset+text.length());
        } catch(BadLocationException ble) {
            Msg.E("Collab: Text insert error", Constants.PLP_GENERIC_ERROR,
                    null);
        }
    }

    public synchronized void stopListening() {
        try {
            Object[][] clients = PLPToolbox.mapToArray(clientServices);
            for(int i = 0; i < clients.length; i++) {
                ClientService cs = (ClientService) clients[i][1];
                cs.forceDisconnect();
            }
            socket.close();
        } catch(IOException e) {
            Msg.E("Unable to close port", Constants.PLP_GENERAL_IO_ERROR, this);
            if(Constants.debugLevel >= 2) e.printStackTrace();
        }
    }

    public ServerControl getControls() {
        return control;
    }

    @Override
    public void run() {
        Msg.I("Listening on port " + socket.getLocalPort(), this);
        while(control.getServerState() == ServerControl.State.LISTENING) {
            try {
                Socket client = socket.accept();
                ClientService c = new ClientService(clientIndex, this, client, null);
                clientServices.put(clientIndex, c);
                Msg.I("Client[" + clientIndex + "] connected", this);
                clientIndex++;
                c.start();
                control.update();
            } catch(IOException e) {
                if(control.getServerState() == ServerControl.State.LISTENING)
                    Msg.E("Unable to open connection",
                            Constants.PLP_GENERIC_ERROR, null);
            }
        }
        Msg.D("Collab server service exiting.", 2, null);
    }

    public synchronized void removeClient(int ID) {
        ClientService cs = clientServices.remove(ID);
        if(cs != null) cs.forceDisconnect();
        control.update();
    }

    public ClientService getClient(int ID) {
        return clientServices.get(ID);
    }

    public HashMap<Integer, ClientService> getClients() {
        return clientServices;
    }

    public void updateControl() {
        control.update();
    }

    public synchronized boolean arbitrate() {
        if(control.getServerState() != ServerControl.State.SOLICITING)
            return false;
        else
            return true;
    }

    public synchronized boolean checkNickname(String nick) {
        Object[][] clients = PLPToolbox.mapToArray(clientServices);
        String tempNick;
        for(int i = 0; i < clients.length; i++) {
            tempNick = ((ClientService)clients[i][1]).getClientName();
            if(nick.equals(tempNick))
                return false;
        }

        return true;
    }

    public synchronized void broadcastCommand(String str) {
        Object[][] clients = PLPToolbox.mapToArray(clientServices);
        for(int i = 0; i < clients.length; i++) {
            ((ClientService)clients[i][1]).cmd(str);
        }
    }

    public String toString() {
        return "collab.ServerService";
    }
}

