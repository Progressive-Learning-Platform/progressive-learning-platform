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
        PLPMsg.M("TRACER Registered");
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
        if (!isInstr)
            PLPMsg.M(String.format("[TRACE] W %08x %08x", addr, data ));
        return super.writeReg(addr, data, isInstr);
    }

    @Override
    public Object read(long addr) {
        Object ret = super.readReg(addr);

        //trace ret
        char rType = super.isInstr(addr) ? 'I' : 'R';

        PLPMsg.M(String.format("[TRACE] %c %08x %08x", rType, addr, (Long)ret));

        return ret;
    }

    @Override
    public String toString() {
        return "Tracer";
    }
}