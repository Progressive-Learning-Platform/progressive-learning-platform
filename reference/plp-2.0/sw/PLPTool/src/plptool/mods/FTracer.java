package plptool.mods;
import plptool.Constants;
import plptool.PLPSimBusModule;
import plptool.PLPMsg;

/**
 * Trace module, outputs bus activity to file
 *
 * @see PLPSimBusModule
 * @author fritz
 */
public class FTracer extends PLPSimBusModule {

    public FTracer(long addr, long size) {
        super(addr, size, true);
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

    public int file(String f) {
        //open file
        
        return Constants.PLP_OK;
    }

    @Override
    public int write(long addr, Object data, boolean isInstr) {
        //trace!

        return super.writeReg(addr, data, isInstr);
    }

    @Override
    public Object read(long addr) {
        Object ret = super.readReg(addr);

        //trace ret

        return ret;
    }

    @Override
    public String toString() {
        return "Tracer";
    }
}