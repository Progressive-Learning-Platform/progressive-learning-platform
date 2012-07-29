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

import java.util.HashMap;
import java.util.ArrayList;

/**
 *
 * @author Wira
 */
public class ClientList {
    private HashMap<Integer, ClientHandle> clients;

    public ClientList() {
        clients = new HashMap<Integer, ClientHandle>();
    }

    public synchronized boolean add(ClientHandle c) {
        if(checkID(c.getID()))
            return false;
        clients.put(c.getNumID(), c);
        return true;
    }

    public ClientHandle get(int numID) {
        return clients.get(numID);
    }

    public void remove(int numID) {
        clients.remove(numID);
    }

    public void queryAll(Server.Query q, String...args) {
        ArrayList<ClientHandle> c =  new ArrayList<ClientHandle>(clients.values());
        for(int i = 0; i < c.size(); i++) {
            c.get(i).query(q, args);
        }
    }

    public void setRoleForAll(ClientHandle.Role r) {
        ArrayList<ClientHandle> c =  new ArrayList<ClientHandle>(clients.values());
        for(int i = 0; i < c.size(); i++) {
            c.get(i).setRole(r);
        }
    }

    public void setStateForAll(ClientHandle.State st) {
        ArrayList<ClientHandle> c =  new ArrayList<ClientHandle>(clients.values());
        for(int i = 0; i < c.size(); i++) {
            c.get(i).setState(st);
        }
    }

    public void sendAll(String d) {
        ArrayList<ClientHandle> c =  new ArrayList<ClientHandle>(clients.values());
        for(int i = 0; i < c.size(); i++) {
            c.get(i).send(d);
        }
    }

    /* returns true if this list contains the client with the ID */
    public boolean checkID(String ID) {
        ArrayList<ClientHandle> c =  new ArrayList<ClientHandle>(clients.values());
        for(int i = 0; i < c.size(); i++) {
            if(c.get(i).getID().equals(ID))
                return true;
        }
        return false;
    }

    /* returns true if this list contains the client with the numID */
    public boolean checkNumID(int numID) {
        return clients.containsKey(numID);
    }
    
    public ArrayList<ClientHandle> getClients() {
        return new ArrayList<ClientHandle>(clients.values());
    }
}
