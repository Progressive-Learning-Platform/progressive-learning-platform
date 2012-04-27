/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.extras.collab;

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
    private boolean disconnect = false;
    private String name;
    BufferedReader in;
    PrintWriter out;

    public ClientService(int ID, ServerService server, Socket socket,
            String name) {
        this.ID = ID;
        this.server = server;
        this.socket = socket;
        this.name = name;
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
        boolean text = false;
        boolean check_nick = false;
        try {
            in = new BufferedReader(new InputStreamReader(
                    socket.getInputStream()));
            out = new PrintWriter(socket.getOutputStream(), true);
            while(!disconnect) {
                line = in.readLine();
                if(line == null) {
                    cmd("DISCONNECT");
                    break;
                }
                
                if(line.startsWith("%%") && !text)
                    Msg.D("Received command: " + line, 3, this);
                
                if(line.equals("%%ENDTEXT")) {
                    text = false;
                    cmd("MUTE");
                } else if (line.equals("%%TEXT")) {
                    text = true;
                } else if(line.equals("%%QUIT")) {
                    disconnect = true;
                } else if(line.equals("%%REQ")) {
                    cmd(server.arbitrate() ? "ACK" : "MUTE");
                } else if(line.equals("%%NICK")) {
                    check_nick = true;
                } else if(text) {
                    server.deliverData(line + "\n");
                } else if(check_nick) {
                    Msg.D("Nickname request: " + line, 2, this);
                    if(!server.checkNickname(line)) {
                        Msg.I("Nickname '" + line + "' is already taken", this);
                        disconnect = true;
                    } else {
                        cmd("NICKNAME_OK");
                        this.setClientName(line);
                        server.updateControl();
                    }
                    check_nick = false;
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
