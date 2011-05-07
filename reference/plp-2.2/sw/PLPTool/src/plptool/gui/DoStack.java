/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.gui;

import plptool.Config;

/**
 * There are two heads, to reduce confusion, a redo pointer
 * and an undo pointer
 *
 * The DoManager implements this stack
 *
 * @author holla
 */
public class DoStack {

    private DoStack undo_link = null;
    private DoStack redo_link = null;
    //True if adding text
    private String changeType = "";
    private String word = "";
    private int begin = -1;

    public DoStack() {
    }

    private DoStack(DoStack link) {
        undo_link = link;
    }

    public boolean adding_text() {
        return changeType.equals("backspace") || changeType.equals("delete");
    }

    public String text() {
        return word;
    }

    public int location() {
        return begin;
    }

    public DoStack undo() {
        return undo_link;
    }

    public DoStack redo() {
        return redo_link;
    }

    public void clear() {
        changeType = "";
        word = "";
        begin = -1;
    }

    public void clearRedo() {
        if(redo_link != null) {
            redo_link.clearRedo();
        }
        redo_link = null;
        undo_link = null;
    }

    public void clearUndo() {
        if(undo_link != null) {
            undo_link.clearRedo();
        }
        undo_link = null;
        redo_link = null;
    }

    public DoStack add(String action, String entry, int location) {
        /**
         * Only extends current entry if a single character is entered and up to
         * a certain length
         *
         * True means entry adds text
         */
        //System.out.println(word + ":" + entry);
        redo_link = null;
        if(begin == -1) {
            changeType = action;
        }
        if(action.equals(changeType) && ((action.equals("backspace") && location == begin - 1) || (action.equals("delete") && location == begin) || (action.equals("insert") && location == begin + word.length()) || begin == -1)) {
            if(entry.length() == 1) {
                if(word.length() < Config.maxUndoLength) {
                    if(changeType.equals("backspace")) {
                        word = entry.concat(word);
                        begin = location;
                    } else {
                        word = word.concat(entry);
                    }
                    if(begin == -1) {
                        begin = location;
                    }
                    return this;
                } else {
                    redo_link = new DoStack(this);
                    return redo_link.add(action, entry, location);
                }
            } else {
                if(word.length() == 0) {
                    word = entry;
                    begin = location;
                    redo_link = new DoStack(this);
                    return redo_link;
                } else {
                    redo_link = new DoStack(this);
                    return redo_link.add(action, entry, location);
                }
            }
        } else {
            redo_link = new DoStack(this);
            return redo_link.add(action, entry, location);
        }
    }

    @Override
    public String toString() {
        if(undo_link == null) {
            return "entry: " + word + "\tat: " + begin + "\ttype: " + changeType;
        } else {
            return "entry: " + word + "\tat: " + begin + "\ttype: " + changeType +"\n" + undo_link.toString();
        }
    }
}