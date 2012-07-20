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

import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.UnsupportedEncodingException;
import java.io.IOException;
import java.net.Socket;

/**
 * Client connection handler class
 *
 * @author Wira
 */
public class ClientHandle {
    public static final int READ_BUFFER_SIZE = 4096;

    public static enum State {CONNECTING, SLEEP, PONDERING, LIVE, DISCONNECT};
    public static enum Role  {LOBBY, CONTROLLER, PARTICIPANT};

    private State st;
    private Role r;
    private boolean admin;

    private BufferedReader in;
    private BufferedWriter out;
    private Socket s;
    private int numID;
    private String ID;

    private Server srv;
    private Session session;
    private Server.Query last;

    private int dataCounter;

    public ClientHandle(Socket s, int numID, Server srv) {
        this.s = s;
        this.numID = numID;
        this.srv = srv;
        this.session = null;
        ID = "__ORPHANkljlkjciou343njknlkdj3"; /* the only reserved client ID */
        last = Server.Query.NONE;
        admin = false;
        dataCounter = 0;

        try {
            in = new BufferedReader(new InputStreamReader(s.getInputStream(), "UTF-8"));
        } catch(IOException e) {
            /* failed to open input stream */
        }
        try {
            out = new BufferedWriter(new OutputStreamWriter(s.getOutputStream(), "UTF-8"));
        } catch(IOException e) {
            /* failed to open output stream */
        }

        st = State.CONNECTING;
    }

    public void start() {
        (new Listener()).start();
    }

    public void close() {
        try {
            out.close();
            in.close();
        } catch(IOException e) {

        }
    }

    public void setID(String str) {
        this.ID = str;
    }

    public String getID() {
        return ID;
    }

    public void setState(State st) {
        this.st = st;
    }

    public void setRole(Role r) {
        this.r = r;
    }

    public State getState() {
        return st;
    }

    public Role getRole() {
        return r;
    }

    public Socket getSocket() {
        return s;
    }

    public int getNumID() {
        return numID;
    }

    public void setSession(Session session) {
        this.session = session;
    }

    public Session getSession() {
        return session;
    }

    public void send(String d) {
        try {
            out.write(d + "\n");
            out.flush();
        } catch(UnsupportedEncodingException e) {

        } catch(IOException e) {
            if(s.isOutputShutdown()) {
                /* output stream is closed */
            } else if(s.isClosed()) {
                /* socket is closed */
            } else {
                /* unknown I/O error */
                e.printStackTrace();
            }
        }
    }

    public void discardLastQuery() {
        last = Server.Query.NONE;
    }

    public void query(Server.Query q, String...args) {
        if(last != Server.Query.NONE) {
            /* nope, we have a query that has not being resolved or discarded */
            return;
        }

        last = q;
        switch(q) {
            case SERVER_PASSWORD:
                send("?server_password");
                break;
            case ID:
                send("?id");
                break;
            case PASSWORD:
                send("?password");
                break;
            case SLEEP:
                send("%%sleep");
                st = State.SLEEP;
                break;
            case PARTICIPATE:
                send("??participate");
                st = State.PONDERING;
                break;
            case SESSION_PASSWORD_PROTECT:
                send("?session_password_protect " + args[0]);
                break;
            case SESSION_BANNER:
                send("?banner " + args[0]);
                break;
            case PICK:
                send("??pick " + args[0]);
                break;
            case TEXT:
                send("??text");
                break;
            case LIVETEXT:
                send("??livetext");
                break;
            case NONE:
                /* do nothing */
                break;
        }
    }

    private synchronized void handleData(String d) {
        dataCounter++;
        System.out.println("Client #" + numID + " " + dataCounter + " recv=" + d);
        Server.Query next = Server.Query.NONE;
        String[] t = d.split("\\s+");

        if(d == null)
            return;

        if(d.startsWith("%_") || d.startsWith("?_"))
            handleAdminQuery(d);
        
        else if(d.startsWith("?") || d.startsWith("%")) {
            /* client sent a query or a command */
            if(d.equals("%disconnect")) {
                send("# bye");
                if(!srv.removeClient(numID))
                    srv.disconnect(this);
            } else if(d.equals("%lobby")) {
                session = null;
                last = Server.Query.NONE;
                r = Role.LOBBY;
                if(r == Role.CONTROLLER) {
                    session.stopActivity();
                    if(!srv.getRegisteredClientMap().containsKey(ID)) {
                        srv.destroySession(numID);
                    } else
                        send("# you are a registered client, the session persists");
                    send("ok");
                } else
                    session.getClientList().remove(numID);
            } else if(d.equals("?list")) {

            } else {
                switch(r) {
                    case LOBBY:
                        if(d.equals("%controller")) {
                            if(!srv.createSession(this)) {
                                send("error=no more session slots");
                            } else {
                                next = Server.Query.SESSION_PASSWORD_PROTECT;
                            }
                        }
                        break;
                    case CONTROLLER:
                        break;
                    case PARTICIPANT:
                        break;
                }
            }
        } else if(d.startsWith("#")) {
            /* information message */
        } else {
            /* we have a reply from our query or command */
            switch(last) {
                case SERVER_PASSWORD:
                    /* handle server password */
                    if(srv.opts.get("password_required") != null &&
                            d.equals(srv.opts.get("password_required")))
                        next = Server.Query.ID;
                    else
                        try {
                            /* wrong password, wait 3 seconds and disconnect */
                            Thread.sleep(3000);
                            send("error=wrong password");
                            s.close();
                        } catch(IOException e) {
                            /* failed to close socket */
                        } catch(InterruptedException e) {
                            
                        }
                    break;
                case ID:
                    /* handle client ID */
                    ID = d;
                    if(d.equals("__ORPHANkljlkjciou343njknlkdj3"))
                        next = Server.Query.ID;
                    /* synchronized across all clients to prevent race */
                    else if(!srv.getClientList().add(this)) { 
                        send("# ID is already taken, pick something else");
                        next = Server.Query.ID;
                    } else {
                        /* check if admin */
                        if(d.equals("admin") && srv.opts.containsKey("admin_password")) {
                            send("# enter administrator's password");
                            next = Server.Query.PASSWORD;
                        /* check if registered */
                        } else if(srv.getRegisteredClientMap().containsKey(ID)) {
                            send("# this ID is registered");
                            next = Server.Query.PASSWORD;
                        } else {
                            r = Role.LOBBY;
                            srv.welcome(this);
                        }
                    }
                    break;
                case PASSWORD:
                    if(ID.equals("admin") && d.equals(srv.opts.get("admin_password"))) {                        
                        r = Role.LOBBY;
                        admin = true;
                        srv.welcome(this);
                        send("# You're logged in as an administrator!");
                    /* handle registered client password */
                    } else if(d.equals(srv.getRegisteredClientMap().get(ID)))  {
                        r = Role.LOBBY;
                        srv.welcome(this);
                        send("# You're logged in a registered user.");
                    } else {
                        send("# wrong password");
                        next = Server.Query.ID;
                        srv.removeClient(numID);
                    }
                    break;
                case SESSION_PASSWORD_PROTECT:
                    if(!d.equals(""))
                        session.setPassword(d);
                    next = Server.Query.SESSION_BANNER;
                    break;
                case SESSION_BANNER:
                    session.setBanner(d);
                    break;
                case SLEEP:
                    /* handle sleep reply */
                    break;
                case PARTICIPATE:
                    /* handle participation request reply */
                    if(d.equals("ok")) {
                        if(session.joinActivity(this)) {
                            /* we're in */
                            send("ok");
                            st = State.LIVE;
                        } else {
                            send("# no free slots");
                            query(Server.Query.SLEEP);
                            st = State.SLEEP;
                        }
                    } else {
                        send("# invalid response");
                        next = Server.Query.PARTICIPATE;
                    }
                    break;
                case PICK:
                    /* handle multiple-choice answer */
                    break;
                case TEXT:
                    /* handle text input */
                    break;
                case LIVETEXT:
                    /* handle live text input */
                    break;
                case NONE:
                    /* client sent garbage */
                    break;
            }
            /* we are satisfied */
            last = Server.Query.NONE;
            if(next != Server.Query.NONE)
                query(next);
        }
    }

    private void handleAdminQuery(String d) {
        if(!admin) {
            send("error=not an admin");
            return;
        }
        srv.stdin(d, this);
    }

    class Listener extends Thread {
        @Override
        public void run() {
            System.out.println("ClientHandle for #" + numID +": Listening.");
            String d;

            while(st != ClientHandle.State.DISCONNECT) {
                try {
                    /* block on socket read */
                    d = in.readLine();
                    if(d != null)
                        handleData(d);
                } catch(IOException e) {
                    if(s.isClosed()) {
                        /* socket has been closed */
                    } else {
                        /* unexpected I/O error */                        
                    }
                    System.out.println("We just lost #" + numID);
                    st = ClientHandle.State.DISCONNECT;
                }
            }
            /* if we lost the connection, this client is gone */
            srv.removeClient(numID);
        }
    }
}
