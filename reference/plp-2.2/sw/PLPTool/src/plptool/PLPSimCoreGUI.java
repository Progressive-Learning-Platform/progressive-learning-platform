/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool;

/**
 *
 * @author wira
 */
public abstract class PLPSimCoreGUI extends javax.swing.JFrame {

    protected PLPSimCore sim;

    abstract public void updateComponents();
    abstract public void updateBusTable();

    public PLPSimCore getSim() {
        return sim;
    }
}
