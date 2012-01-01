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
import plptool.PLPSimBus;
import plptool.Constants;
import plptool.Msg;

/**
 * PLP VGA module. The VGA module takes the simulation bus as an argument as it
 * draws a contiguous region from memory, starting by the address pointed by the
 * frame pointer. gui_eval(x) is only executed when the control register is
 * set to one.
 *
 * @author wira
 */
public class VGA extends PLPSimBusModule {

    private PLPSimBus bus;
    private VGAFrame frame;

    public VGA(long addr, PLPSimBus bus, VGAFrame frame) {
        super(addr, addr + 4, true);
        this.bus = bus;
        this.frame = frame;
    }

    public int eval() {

        return Constants.PLP_OK;
    }

    public int gui_eval(Object x) {
        if(!super.threaded)
            return draw();
        
        return Constants.PLP_SIM_EVAL_ON_THREADED_MODULE;
    }
    
    private int draw() {
        if(!enabled || frame == null)
            return Constants.PLP_OK;

        if(!super.isInitialized(startAddr + 4))
            return Constants.PLP_SIM_UNINITIALIZED_MEMORY;

        // Check if control register is initialized and whether it's set to one
        if(!super.isInitialized(startAddr) || ((Long) super.read(startAddr) & 0x1) != 1)
            return Constants.PLP_SIM_MODULE_DISABLED;

        long framePointer = (Long) super.read(startAddr + 4);
        Msg.D("Framepointer is at " + String.format("0x%08x", framePointer), 4, this);

        frame.setLabelEnabled(((Long) super.read(startAddr) & 0x1) == 1);
        frame.setFramePointer(framePointer);

        // the image is a 640x480 int array (each color is 8-bit, most significant
        // 8-bit is ignored (we're using INT_RGB_TYPE for BufferedImage).
        int[][] image = new int[640][480];

        for(int y_coord = 0; y_coord < 480; y_coord++) {
            for(int x_coord = 0; x_coord < 160; x_coord++) {
                // Each word is packed with MSB 4 pixel data
                long addr = framePointer + (y_coord * 640) + (x_coord * 4);

                // default data to 0
                long data = 0;
                if(bus.isInitialized(addr)) {
                    /* -------------------- HACK ALERT ------------------------
                     * This is a hack to prevent race condition, and may cause
                     * some artifact in VGA output. Unchecked read on the bus
                     * will return 'null' if the mapped module failed to
                     * return a value and will not generate a simulation-stopping
                     * error. This could happen when the module is being written
                     * to while we issue a read to the bus.
                     */
                    Object dataObj = bus.uncheckedRead(addr);
                    data = dataObj != null ? (Long) dataObj : 0;
                    Msg.D("Initialized pixel at " + String.format("0x%08x", addr), 4, this);
                }

                // unpack pixels from the word and populate the image array.
                for(int i = 0; i < 4; i++) {
                    int pixel = (((int) data) >> i * 8) & 0xff;

                    // 3 bits of red
                    int red = ((int) pixel & 0xE0);
                    red = (red == 0xE0) ? 0xFF : red; // write 0xff if data is 0b111
                                                      // so we can have a white

                    // 3 bits of green
                    int green = ((int) pixel & 0x1C) << 3;
                    green = (green == 0xE0) ? 0xFF : green;

                    // 2 bits of blue
                    int blue = ((int) pixel & 0x03) << 6;
                    blue = (blue == 0xC0) ? 0xFF : blue;

                    Msg.D("Colors: " + red + " " + green + " " + blue, 4, this);
                    
                    // VGA module buffer is MSB, so we put the LSB last on the array
                    image[x_coord * 4 + (3 - i)][y_coord] = (red << 16) | (green << 8) | (blue);
                }
            }
        }

        // draw image to frame
        frame.draw(image);

        return Constants.PLP_OK;
    }

    // this module can be made a thread
    @Override
    public void run() {
        try {
        while(!stop) {
            this.draw();
            Thread.sleep(plptool.Config.threadedModRefreshRate);
        }

        Msg.M("VGA module thread exiting.");

        } catch (Exception e) { }
    }

    @Override public void reset() {
        super.writeReg(startAddr, new Long(0L), false);
        super.writeReg(startAddr+4, new Long(0L), false);
    }

    public String introduce() {
        return "640x480 VGA Module";
    }

    @Override
    public String toString() {
        return "VGA";
    }
}
