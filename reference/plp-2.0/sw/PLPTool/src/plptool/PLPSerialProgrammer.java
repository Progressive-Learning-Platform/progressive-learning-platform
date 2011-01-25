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
public abstract class PLPSerialProgrammer {
    abstract public int connect(String portName, int baudRate) throws Exception;
    abstract public int close();
    abstract public int programWithPLPFile(String PLPFilePath) throws Exception;
    abstract public int programWithAsm(PLPAsm asm) throws Exception;
}
