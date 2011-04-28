/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.gui;



/**
 *
 * @author joshua
 */
public class DoManager {
    private DoStack undo_action = null;
    private DoStack redo_action = null;
    private String form;

    public DoManager() {
        form = "";
    }

    public DoManager(String currentDoc){
        form = currentDoc;
    }

    public void undone() {
        redo_action = undo_action;
        undo_action = undo_action.undo();
    }

    public void redone() {
        undo_action = redo_action;
        redo_action = redo_action.redo();
    }

    public boolean can_undo() {
        return undo_action != null;
    }

    public boolean undo_action() {
        //True if undo adds text
        return undo_action.adding_text();
    }

    public String undo_text() {
        return undo_action.text();
    }

    public int undo_location() {
        return undo_action.location();
    }

    public boolean can_redo() {
        return redo_action != null;
    }

    public boolean redo_action() {
        //True if redo deletes text
        return !redo_action.adding_text();
    }

    public String redo_text() {
        return redo_action.text();
    }

    public int redo_location() {
        return redo_action.location();
    }

    public void modify(String action, String entry, int selection_start, int selection_end, int location, int current_length) {
        //System.out.println("Called modify");
        if(undo_action == null) {
            undo_action = new DoStack();
        }
        redo_action = null;
        String selection;
        try {
            selection = form.substring(selection_start, selection_end);
        } catch(IndexOutOfBoundsException ioobe) {
            selection = "";
        }
        //System.out.println(selection + " " + selection_start + " " + selection_end);
        if(!selection.equals("")) {
            try {
                undo_action = undo_action.add("delete", form.substring(selection_start,selection_end), location);
                form = form.substring(0,selection_start).concat(form.substring(selection_end));
            } catch(IndexOutOfBoundsException ioobe) { 
                //System.out.println("0" + " " + (selection_start) + " " + (selection_end + 1) + " " + selection_end + " " + form.length());
            }
        } else if(current_length < form.length()) {
            try {
                undo_action = undo_action.add("delete", form.substring(selection_start,selection_start + form.length()-current_length), location);
                form = form.substring(0,selection_start).concat(form.substring(selection_start+ form.length()-current_length));
            } catch(IndexOutOfBoundsException ioobe) {
                //System.out.println("0" + " " + (selection_start) + " " + (form.length()-current_length) + " " + form.length());
            }
        } else {
            if(action.equals("backspace")) {
                try {
                    undo_action = undo_action.add("backspace", form.substring(location,location+1), location);
                    form = form.substring(0,location).concat(form.substring(location+1));
                } catch(IndexOutOfBoundsException ioobe) { }
            } else if(action.equals("delete")) {
                try {
                    undo_action = undo_action.add("delete", form.substring(location,location+1), location);
                    form = form.substring(0,location).concat(form.substring(location+1));
                } catch(IndexOutOfBoundsException ioobe) { }
            }
        }
        if(action.equals("insert")) {
            undo_action = undo_action.add("insert", entry, location);
            try {
                form = form.substring(0,location).concat(entry).concat(form.substring(location));
            } catch(IndexOutOfBoundsException ioobe) {
                //System.out.println("Caught");
                form = form.concat(entry);
            }
        } else if(action.equals("paste")) {
            undo_action = undo_action.add("insert", entry, location-entry.length());
            //System.out.println(selection_start + " " + selection_end + form.length());
            form = form.substring(0,selection_start-entry.length()).concat(entry).concat(form.substring(selection_end-entry.length()));
        }
        //System.out.println(form);
        //System.out.println(undo_action);
    }
}
