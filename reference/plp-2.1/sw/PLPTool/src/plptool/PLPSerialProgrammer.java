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
    protected plptool.gui.PLPBackend backend;

    public boolean busy = false;
    public int     progress = 0;

    public PLPSerialProgrammer(plptool.gui.PLPBackend backend) {
        this.backend = backend;
    }

    @Override
    public void run() {
        try {

        busy = true;
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
