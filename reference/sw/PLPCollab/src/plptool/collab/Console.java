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
import java.io.*;

/**
 *
 * @author Wira
 */
public class Console extends Thread {
    private BufferedReader in;
    private Server srv;
    private Client cli;

    public Console(BufferedReader in, Server srv) {
        this.in = in;
        this.srv = srv;
    }

    public Console(BufferedReader in, Client cli) {
        this.in = in;
        this.cli = cli;
    }

    @Override public void run() {
        System.out.println("Console: active.");
        String d;

        try {
            while((d = in.readLine()) != null) {
                System.out.println("Console: recv=" + d);
                if(srv != null)
                    srv.stdin(d, null);
                else if(cli != null)
                    cli.stdin(d);
            }
        } catch(IOException e) {
            System.err.println("Error: Lost console connection.");
        }
    }
}
