/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool;

/**
 * PLP Serial Programmer abstract class.
 *
 * @author wira
 */
public abstract class PLPSerialProgrammer extends Thread {
    protected plptool.gui.ProjectDriver plp;

    public boolean busy = false;
    public int     progress = 0;

    public PLPSerialProgrammer(plptool.gui.ProjectDriver plp) {
        this.plp = plp;
    }

    @Override
    public void run() {
        try {

        busy = true;
        if(plp.p_port != null)
            programWithAsm();
        close();
        
        } catch(Exception e) {
            e.printStackTrace();
        }
    }

    abstract public int connect(String portName, int baudRate) throws Exception;
    abstract public int close();
    abstract public int programWithAsm() throws Exception;
}
