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

package plptool.collab.client;

import java.net.*;
import javax.net.ssl.*;
import java.io.*;

/**
 *
 * @author Wira
 */
public class Client {
    private Socket s;
    private BufferedReader in;
    private BufferedWriter out;

    public static enum Role {
        LOBBY, CONTROLLER, PARTICIPANT;
    }

    public static enum Query {
        HELP, BANNER, OPTIONS, IP, STATS, ME, LIST, INFO, CONTROLLER,
            JOIN, BCAST, LOBBY, REG, UNREG
    }

    public static enum SessionQuery {
        INFO, LIST, BCAST, VOTEKICK, INTERRUPT
    }

    public static enum ControllerQuery {
        START, SUSPEND, RESUME, KICKANDSUSPEND, RECRUIT, DESTROY, SOLICIT,
            ENOUGH, FINISH, NEXT, PICK, TEXT, LIVETEXT, LISTDATA, VIEWDATA
    }

    public Client(String host, int port, SSLContext ssl) throws IOException {
        if(ssl != null)
            s = ssl.getSocketFactory().createSocket(host, port);
        else {
            System.err.println("Warning: Client is not using SSL.");
            s = new Socket(host, port);
        }

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

    public void stdin(String d) {
        send(d);
    }

    public void handleData(String d) {
        System.out.println(d);
        if(d  == null) {
            System.err.println("Received EOF");
            System.exit(1);
        }
    }
    
    public void query(Query q) {
        
    }
    
    public void sessionQuery(SessionQuery q) {
        
    }
    
    public void controllerQuery(ControllerQuery q) {
        
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

    class Listener extends Thread {
        @Override
        public void run() {
            String d;

            while(true) {
                try {
                    /* block on socket read */
                    d = in.readLine();
                    handleData(d);
                } catch(IOException e) {
                    System.out.println("Connection to server lost.");
                    if(s.isClosed()) {
                        /* socket has been closed */
                        System.exit(0);
                    } else {
                        /* unexpected I/O error */
                        System.exit(1);
                    }
                }
            }
        }
    }
}
