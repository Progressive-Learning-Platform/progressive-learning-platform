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

package plptool.collab;

import plptool.collab.server.*;
import plptool.collab.client.*;
import javax.net.ssl.*;
import java.io.*;
import java.net.*;

/**
 *
 * @author Wira
 */
public class Main {
    private static boolean server = false;
    private static boolean client = false;
    private static boolean stdin = false;
    private static boolean ssl = true;
    private static boolean console = false;
    private static int port;
    private static String stdin_addr;
    private static int stdin_port;
    private static String client_addr;
    private static int client_port;
    private static int console_port;
    private static String admin_password;
    private static String server_password;

    public static void main(String args[]) {
        System.out.println("PLP Classroom Collaboration Tool version 1\n");
        if(args.length == 0) {
            printUsage();
            System.exit(0);
        }

        int i = 0;
        while(i < args.length) {
            if(args[i].equals("--dry-run")) {
                System.exit(0);

            } else if (args[i].equals("--server") && (i + 1) < args.length && !client && !stdin) {
                try {
                    port = parseInteger(args[i+1]);
                } catch(NumberFormatException e) {
                    System.err.print("Error: Unable to parse " + args[i+1] + ".");
                    System.exit(1);
                }
                server = true;
                i+=1;
            } else if (args[i].equals("--stdin") && (i + 1) < args.length && !client && !server) {
                try {
                    stdin_addr = args[i+1].split(":")[0];
                    stdin_port = parseInteger(args[i+1].split(":")[1]);
                } catch(NumberFormatException e) {
                    System.err.print("Error: Unable to parse " + args[i+1] + ".");
                    System.exit(1);
                }
                stdin = true;
                i+=1;
            } else if (args[i].equals("--client") && (i + 1) < args.length && !stdin && !server) {
                try {
                    client_addr = args[i+1].split(":")[0];
                    client_port = parseInteger(args[i+1].split(":")[1]);
                } catch(NumberFormatException e) {
                    System.err.print("Error: Unable to parse " + args[i+1] + ".");
                    System.exit(1);
                }
                client = true;
                i+=1;
            } else if (args[i].equals("--disable-ssl")) {
                ssl = false;
            } else if (args[i].equals("--console") && (i + 1) < args.length) {
                console = true;
                try {
                    console_port = parseInteger(args[i+1]);
                } catch(NumberFormatException e) {
                    System.err.print("Error: Unable to parse " + args[i+1] + ".");
                    System.exit(1);
                }
                i+=1;
            } else if (args[i].equals("--adminpw") && (i + 1) < args.length) {
                admin_password = args[i+1];
                i+=1;
            } else if (args[i].equals("--serverpw") && (i + 1) < args.length) {
                server_password = args[i+1];
                i+=1;
            } else if(args[i].equals("--help")) {
                printUsage();
                System.exit(0);
            } else {
                System.out.println("Invalid argument: " + args[i] + "\n");
                printUsage();
                System.exit(0);
            }
            i++;
        }

        if(admin_password != null && !server) {
            System.err.println("--adminpw can only be used with --server");
            System.exit(1);
        } else if(server_password != null && !server) {
            System.err.println("--serverpw can only be used with --server");
            System.exit(1);
        }

        /* forward stdin to socket */
        if(stdin) {
            Socket s = null;
            String d;
            PrintWriter out = null;
            BufferedReader in = null;
            try {
                s = new Socket(InetAddress.getByName(stdin_addr), stdin_port);
                out = new PrintWriter(new OutputStreamWriter(s.getOutputStream(), "UTF-8"));
                in = new BufferedReader(new InputStreamReader(System.in));

            } catch(UnknownHostException e) {
                System.err.println("Error: Unable to open " + stdin_addr + ":" + stdin_port);
                System.exit(1);
            } catch(IOException e) {
                System.err.println("Error: Unable to open " + stdin_addr + ":" + stdin_port);
                System.exit(1);
            }
            System.out.print("> ");
            try {
                while((d = in.readLine()) != null) {
                    if(d.equals("quit")) {
                        out.close();
                        s.close();
                        System.exit(0);
                    }
                    out.println(d);
                    out.flush();
                    System.out.print("> ");
                }
            } catch (IOException e) {
                System.err.println("Error: I/O error.");
                System.exit(1);
            }
            System.out.println("\nbye!");
        }

        if(client) {
            Client cli = null;
            if(ssl) {
                System.err.println("Error: Client SSL support is not implemented yet.");
                System.exit(1);
            } else {
                try {
                    cli = new Client(client_addr, client_port, null);
                    cli.start(); /* hand off */
                } catch(IOException e) {
                    System.err.println("Error: Client failed to open socket.");
                    System.exit(1);
                }
            }
            if(console) {
                Socket s = null;
                ServerSocket sv;
                BufferedReader in;
                try {
                    sv = new ServerSocket(console_port, 0, InetAddress.getByName("127.0.0.1"));
                    System.out.println("Console: Listening on " + sv.getLocalSocketAddress());
                    while(true) {
                        try {
                            s = sv.accept();
                            in = new BufferedReader(new InputStreamReader(s.getInputStream(), "UTF-8"));
                            (new Console(in, cli)).start();
                        } catch(IOException e) {
                            System.err.println("Error: Unable to open socket / establish connection for console.");
                        }
                    }
                } catch(IOException e) {
                    System.err.println("Console: Failed to open socket.");
                }
            }
        }

        if(server) {
            Server srv = null;
            if(ssl) {
                System.err.println("Error: Server SSL support is not implemented yet.");
                System.exit(1);
            } else {
                try {
                    srv = new Server(port, null);
                    if(admin_password != null)
                        srv.opts.put("admin_password", admin_password);
                    if(server_password != null)
                        srv.opts.put("password_required", server_password);
                    srv.accept(); /* hand off, this will start server conn. listener thread */
                } catch(IOException e) {
                    System.err.println("Error: Server failed to open socket.");
                    System.exit(1);
                }
            }
            if(console) {
                Socket s;
                ServerSocket sv;
                BufferedReader in;
                try {
                    sv = new ServerSocket(console_port, 0, InetAddress.getByName("127.0.0.1"));
                    System.out.println("Console: Listening on " + sv.getLocalSocketAddress());
                    while(true) {
                        try {
                            s = sv.accept();
                            in = new BufferedReader(new InputStreamReader(s.getInputStream(), "UTF-8"));
                            (new Console(in, srv)).start();
                        } catch(IOException e) {
                            System.err.println("Error: Unable to open socket / establish connection for console.");
                        }
                    }
                } catch(IOException e) {
                    System.err.println("Console: Failed to open socket.");
                }

            }
        }
    }

    public static void printUsage() {
        System.out.println("  To run a server:   java PLPCollab.jar --server <port>");
        System.out.println("  To run a client:   java PLPCollab.jar --client <address>:<port>");
        System.out.println("  To forward stdin:  java PLPCollab.jar --stdin <address>:<port>");
        System.out.println();
        System.out.println("  Common server and client options:");
        System.out.println("    --disable-ssl     Disable SSL encryption");
        System.out.println("    --console <port>  Open localhost-only input console at <port>");
        System.out.println();
        System.out.println("  Server options:");
        System.out.println("    --config <file>   Load server configuration file");
        System.out.println();
        System.out.println("  Server debug options (you should use --config to hide these arguments):");
        System.out.println("    --adminpw <port>  Set administrator's password");
        System.out.println("    --serverpw <port> Password-protect the server");
        System.out.println();
        System.out.println("Developer contact: wheerdam@gmail.com");
    }

    private static int parseInteger(String s) throws NumberFormatException {
        return Integer.parseInt(s);
    }
}
