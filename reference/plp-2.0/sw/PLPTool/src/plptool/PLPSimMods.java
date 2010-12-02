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

package plptool;

/**
 *
 * @author wira
 */
public class PLPSimMods {
     
}

class io_example_external extends PLPSimBusModule {
    public io_example_external(long addr) {
        super(addr, addr, true);
        // This I/O only has 1 register
    }

    // When this module is evaluated, do this:
    public int eval() {
        if(!enabled)
            return PLPMsg.PLP_OK;

        // Get register value
        long value = super.read(startAddr);

        // Combinational logic
        if(value == 0xBEEF)
            PLPMsg.M(this + ": Hey, it's beef!");
        else
            PLPMsg.M(this + "No beef :(");

        return PLPMsg.PLP_OK;
    }

    public int gui_eval(Object x) { return PLPMsg.PLP_OK; }

    public String introduce() {
        return "PLPTool 2.0 io_example: an example of PLP bus module, try writing 0xbeef to " +
                String.format("0x%08x", startAddr) + "!";
    }

    // Make sure to do this for error tracking
    @Override public String toString() {
        return "io_example";
    }
}

class io_leds extends PLPSimBusModule {
    public io_leds(long addr) {
        super(addr, addr, true);
        // This I/O only has 1 register
    }

    // When this module is evaluated, do this:
    public int eval() {
        if(!enabled)
            return PLPMsg.PLP_OK;

        // Get register value
        long value = super.read(startAddr);

        System.out.print(this + ": ");

        // Combinational logic
        for(int i = 7; i >= 0; i--) {
            if((value & (long) Math.pow(2, i)) == (long) Math.pow(2, i))
                System.out.print("* ");
            else
                System.out.print(". ");
        }

        System.out.println();

        return PLPMsg.PLP_OK;
    }

    public int gui_eval(Object x) {
        return PLPMsg.PLP_OK;
    }

    public String introduce() {
        return "PLPTool 2.0 io_leds: 8-LED array attached to " +
               String.format("0x%08x", startAddr);
    }

    // Make sure to do this for error tracking
    @Override public String toString() {
        return "io_leds";
    }
}

class cache_hier extends PLPSimBusModule {
    PLPSimMemModule L1_I;
    PLPSimMemModule L1_D;
    PLPSimMemModule L2;

    public cache_hier() {
        super(0, (long) Math.pow(2,62), true);
    }

    // When this module is evaluated, do this:
    public int eval() {
        return PLPMsg.PLP_OK;
    }

    public int gui_eval(Object x) {
        return PLPMsg.PLP_OK;
    }

    public String introduce() {
        return "PLPTool 2.0 Cache Hierarchy Module";
    }

    // Make sure to do this for error tracking
    @Override public String toString() {
        return "cache_hier";
    }
}