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

import plptool.Constants;
import plptool.PLPSimBusModule;
import plptool.Msg;

/**
 * Trace module, outputs bus activity to file
 *
 * @see PLPSimBusModule
 * @author fritz
 */
public class FTracer extends PLPSimBusModule {

    private boolean record = false;
    private boolean stdout = true;
    private StringBuilder recordStr;

    public FTracer(long addr, long size) {
        super(addr, addr + size, true);
        Msg.M("TRACER Registered - Core reset required");
        recordStr = new StringBuilder();
        phantom = true;
    }

    public int eval() {
        //nothing to do here...
        return Constants.PLP_OK;
    }

    public int gui_eval(Object x) {
        //nothing to see here...
        return Constants.PLP_OK;
    }

    public String introduce() {
        return "Tracer";
    }

    @Override
    public int write(long addr, Object data, boolean isInstr) {
        //trace!
        String str = "";
        if (!isInstr)
            str = String.format("[TRACE] W %08x %08x", addr, data );
        if(stdout && !isInstr)
            Msg.M(str);
        if(record) {
            recordStr.append(str);
            recordStr.append("\n");
        }
        
        return super.writeReg(addr, data, isInstr);
    }

    @Override
    public Object read(long addr) {
        Object ret = super.readReg(addr);

        //trace ret
        char rType = super.isInstr(addr) ? 'I' : 'R';
        String str = String.format("[TRACE] %c %08x %08x", rType, addr, (Long)ret);

        if(stdout)
            Msg.M(str);
        if(record) {
            recordStr.append(str);
            recordStr.append("\n");
        }

        return ret;
    }

    @Override
    public void reset() {

    }

    @Override
    public String toString() {
        return "Tracer";
    }
}
