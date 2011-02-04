/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.gui;
import plptool.PLPCfg;

/**
 *
 * @author joshua
 */
public class PlpUndoManager extends javax.swing.undo.UndoManager{

    @Override
    public boolean addEdit(javax.swing.undo.UndoableEdit anEdit) {
            if(PLPCfg.nothighlighting) {
                return super.addEdit(anEdit);
            }
            return false;
    }

}
