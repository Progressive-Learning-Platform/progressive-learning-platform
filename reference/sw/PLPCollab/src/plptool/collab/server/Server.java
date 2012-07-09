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

import javax.net.ssl.*;
import java.net.*;
import java.io.*;
import java.util.*;
import java.text.*;

/**
 * Main server and socket connection handler class
 *
 * @author Wira
 */
public class Server {
    public final int CLIENT_NUM_ID_MAX_POWER = 20;

    private State st;
    private ServerSocket sv;
    private ClientList clients;
    private HashMap<String, String> registered;
    private HashMap<Integer, Session> sessions;
    private ConnListener l;
    private String banner;
    private int sessionCounter;

    public Properties opts;

    public static enum State {
        SUSPENDED, LISTENING, QUITTING;
    }

    public static enum Query {
        NONE, SLEEP, LIVE,
        SERVER_PASSWORD, ID, PASSWORD,
        PARTICIPATE, PICK, TEXT, LIVETEXT,
        SESSION_PASSWORD_PROTECT, SESSION_BANNER
    }

    /* constructor will throw I/O exception if it can not create the socket */
    public Server(int port, SSLContext ssl) throws IOException {
        clients = new ClientList();
        if(ssl != null)
            sv = ssl.getServerSocketFactory().createServerSocket(port);
        else {
            System.err.println("Warning: Server is not using SSL!");
            sv = new ServerSocket(port);
        }
        st = State.LISTENING;
        clients = new ClientList();
        registered = new HashMap<String, String>();
        sessions = new HashMap<Integer, Session>();
        sessionCounter = 0;
        opts = new Properties();
        /* opts.put("registered_clients_only", true); */
        /* opts.put("registered_controller_only", true); */
        /* opts.put("password_required", "aardvark"); */
        /* opts.put("self_registration_allowed", true); */
        opts.put("maximum_sessions", 16);
        opts.put("maximum_clients", 1024);
        opts.put("maximum_session_clients", 64);
    }

    public boolean accept() {
        if(l != null)
            return false; /* already accepting connections */

        l = new ConnListener(this);
        l.start();
        st = State.LISTENING;
        return true;
    }

    public boolean suspend() {
        if(l == null)
            return false;

        st = State.SUSPENDED;
        l.interrupt();
        l = null;
        return true;
    }

    /* this server object is useless after this call */
    public void close() {
        st = Server.State.QUITTING;
        try {
            sv.close();
        } catch(IOException e) {
            
        }
    }

    public void setBanner(String d) {
        banner = d;
    }

    public synchronized void stdin(String d, ClientHandle c) {
        String[] t = d.split("\\s+");

        if(t.length == 0) {
            stdout("error=empty command", c);
        } else if(d.equals("%_quit"))
            System.exit(0);
        else if(d.equals("%_suspend"))
            suspend();
        else if(d.equals("%_resume"))
            accept();
        else if(t[0].equals("%_reg") && t.length == 3) {
            registered.put(t[1], t[2]);
            stdout("ok", c);
        } else if(t[0].equals("%_unreg") && t.length == 2) {
            if(registered.remove(t[1]) != null)
                stdout("ok", c);
            else
                stdout("error=id is not registered", c);
        } else if(d.equals("?_peons")) {
            ArrayList<ClientHandle> clist = clients.getClients();
            ClientHandle cli;
            for(int i = 0; i < clist.size(); i++) {
                cli = clist.get(i);
                stdout("# " + cli.getNumID() + "\t" + cli.getID() + "\t" + cli.getSocket().getInetAddress(), c);
            }
        } else if(t[0].equals("%_destroy") && t.length == 2) {
            try {
                int n = Integer.parseInt(t[1]);
                Session session = sessions.get(n);
                if(session == null)
                    stdout("error=invalid session ID", c);
                else {
                    session.destroy();
                    stdout("ok", c);
                }
            } catch(NumberFormatException e) {
                stdout("error=unable to parse " + t[1], c);
            }
        } else if(t[0].equals("%_kick") && t.length == 2) {
            try {
                stdout("# kicking " + t[1], c);
                int n = Integer.parseInt(t[1]);
                if(!removeClient(n))
                    stdout("error=invalid client ID", c);
                else
                    stdout("ok", c);
            } catch(NumberFormatException e) {
                stdout("error=unable to parse " + t[1], c);
            }
        }

    }

    public void stdout(String d, ClientHandle c) {
        if(c != null)
            c.send(d);
        else
            System.out.println(d);
    }

    public synchronized boolean removeClient(int clientNumID) {
        ClientHandle c = clients.get(clientNumID);
        if(c == null)
            return false;
        System.out.println("Removing #" + clientNumID + " from the list");
        disconnect(c);
        clients.remove(clientNumID);
        if(c.getSession() != null)
            c.getSession().getClientList().remove(clientNumID);
        return true;
    }

    /* force disconnect */
    public synchronized void disconnect(ClientHandle c) {
        System.out.println("Disconnecting #" + c.getNumID());
        try {
            c.close();
            c.getSocket().shutdownInput();
            c.getSocket().shutdownOutput();
            c.getSocket().close();
        } catch(IOException e) {
            /* failed to gracefully close client connection */
        }
    }

    public synchronized boolean createSession(ClientHandle controller) {
        if(sessions.size() == opts.get("maximum_sessions")) {
            controller.send("# maximum number of sessions have been reached");
            return false;
        }
        if(sessions.containsKey(controller.getNumID())) {
            controller.send("# this client already has a session active");
            return false;
        }

        Session session = new Session(this, controller);
        session.setCurrentActivity(Session.Activity.SUSPENDED);
        sessions.put(controller.getNumID(), session);
        controller.setSession(session);
        controller.setRole(ClientHandle.Role.CONTROLLER);
        return true;
    }

    public synchronized boolean destroySession(int numID) {
        Session session;
        if((session = sessions.get(numID)) == null)
            return false;

        session.destroy();
        session.getController().setSession(null);
        sessions.remove(numID);
        return true;
    }

    public synchronized Session getSession(int numID) {
        return sessions.get(numID);
    }

    class ConnListener extends Thread {
        private Server server;

        public ConnListener(Server server) {
            this.server = server;
        }

        @Override
        public void run() {
            System.out.println("Server: Listening on " + sv.getLocalSocketAddress());
            Socket s;
            ClientHandle c;

            while(st == Server.State.LISTENING) {
                try {
                    s = sv.accept();
                    c = new ClientHandle(s, generateClientNumID(), server);
                    System.out.println("New client - #" + c.getNumID());
                    /* handle new client if we are not suspended */
                    if(st == Server.State.LISTENING) {
                        c.start();
                        if(opts.containsKey("password_required"))
                            c.query(Query.SERVER_PASSWORD);
                        else
                            c.query(Query.ID);                        
                    } else
                        s.close();
                    
                } catch(IOException e) {
                    if(sv.isClosed()) {
                        /* server has closed connection */
                    } else {
                        /* interrupted? */
                    }
                }
            }
            System.out.println("Server: Connection listener exiting.");
        }
    }

    private int generateClientNumID() {
        Random r = new Random();
        int n;
        while(clients.checkNumID(
                (n = r.nextInt((int) Math.pow(2, CLIENT_NUM_ID_MAX_POWER)))));
        return n;
    }

    public ClientList getClientList() {
        return clients;
    }

    public HashMap getRegisteredClientMap() {
        return registered;
    }

    public void welcome(ClientHandle c) {
        c.send("# Welcome aboard!");
        c.send("# Current server time is: " +
                (new SimpleDateFormat("yyyy/MM/dd HH:mm:ss")).format(Calendar.getInstance().getTime()) +
                " " + Calendar.getInstance().getTimeZone().getDisplayName());
    }
}
