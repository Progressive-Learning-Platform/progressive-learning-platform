/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.mods;
import plptool.PLPSimBusModule;
import plptool.Constants;
import plptool.PLPMsg;

/*
 * UART registers:
 * +0: command (1 for send, 2 for clear rdy, always reads 0)
 * +4: status  ([0] cts, [1] rdy, read only)
 * +8: send buffer
 * +c: receive buffer
 */

/**
 *
 * @author fritz
 */
public class UART extends PLPSimBusModule {
    long receiveB, sendB;
    boolean ready;
    UARTFrame frame;

    public UART(long addr) {
        super(addr, addr+12, true);
    }

    public void setFrame(Object x) {
        frame = (UARTFrame)x;
    }
    public int eval() {
        return Constants.PLP_OK;
    }

    public int gui_eval(Object x) {
        //no need to update every cycle
	return Constants.PLP_OK;
    }

    public String introduce() {
        return "UART";
    }

    @Override
    public String toString() {
        return "UART";
    }

    @Override
    public Object read(long addr) {
        Long ret = new Long(0);

        if (addr == startAddr) { /* command register */
            return ret; /* always reads 0 */
        } else if (addr == startAddr+4) {
            if (ready)
                ret = new Long(3);
            else
                ret = new Long(1); /* cts is always set, because we're MAGIC! */
        } else if (addr == startAddr+8) {
            ret = new Long(receiveB);
        } else if (addr == startAddr+12) {
            ret = new Long(sendB);
        }

        return ret;
    }

    @Override
    public int write(long addr, Object data, boolean isInstr) {
        long d = (Long)data;

        if (addr == startAddr) { /* command register */
            if (d == 1) { /* send */
                frame.addText(sendB);
            } else if (d == 2) /* clear rdy */
                ready = false;
            else
                PLPMsg.M(String.format("[UART] invalid write to command register: %d", d));
        } else if (addr == startAddr + 4) { /* status register */
            PLPMsg.M(String.format("[UART] invalid write to status register: %d ", d));
        } else if (addr == startAddr + 8) { /* receive buffer */
            PLPMsg.M(String.format("[UART] invalid write to receive buffer: %d", d));
        } else if (addr == startAddr + 12) { /* send buffer */
            sendB = d;
        } else {
            PLPMsg.M("[UART] invalid register");
        }

        return Constants.PLP_OK;
    }
}
