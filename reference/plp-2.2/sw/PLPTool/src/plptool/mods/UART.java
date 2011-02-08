/*
    Copyright 2010 David Fritz, Brian Gordon, Wira Mulia

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

package plptool.mods;
import plptool.PLPSimBusModule;
import plptool.Constants;
import plptool.PLPMsg;
import java.util.LinkedList;
import java.util.Queue;

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
    boolean ready = false;
    UARTFrame frame;
    Queue<Long> rb = new LinkedList<Long>();

    public UART(long addr) {
        super(addr, addr+12, true);
    }

    public void receivedData(char c) {
        rb.add(new Long(c));
    }

    public void setFrame(Object x) {
        frame = (UARTFrame)x;
        frame.setUART(this);
    }
    public int eval() {
        /* handle new data received from the host */
        if (!ready && rb.size() != 0) {
            ready = true;
            receiveB = (Long)rb.remove();
        }

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
