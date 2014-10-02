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
 * PLP Serial Programmer abstract class.
 *
 * @author wira
 */
public abstract class PLPSerialProgrammer extends Thread {
    protected plptool.gui.ProjectDriver plp;

    protected boolean   programming = false;
    public int          progress = 0;
    protected String    portName;

    /**
     * PLPSerialProgrammer defaults to returning the current version of PLPTool.
     * User assemblers can override this. May be useful for dynamic modules
     *
     * @return String of PLPTool version
     */
    public String getVersion() { return Text.versionString; }

    public PLPSerialProgrammer(plptool.gui.ProjectDriver plp) {
        this.plp = plp;
    }

    @Override
    public void run() {
        int ret;
        try {

        setProgramming();
        if(plp.p_port != null) {
            ret = programWithAsm();
            close();
            if(plp.g()) plp.g_prg.enableControls();
            if(ret != Constants.PLP_OK) {
                if(plp.g())
                    plp.g_prg.getStatusField().setText("Failed!");
                else
                    Msg.I("Failed!", this);
            }
        }
        
        } catch(Exception e) {
            e.printStackTrace();
        }
    }

    private synchronized void setProgramming() {
        programming = true;
    }

    public synchronized boolean isProgramming() {
        return programming;
    }

    public synchronized void cancelProgramming() {
        programming = false;
    }

    /**
     * Overridable developer-specified generic hook.
     *
     * @param param An object to pass to the hook
     * @return A reference to an object returned from the hook function
     */
    public Object hook(Object param) {return null;};

    abstract public int connect(String portName) throws Exception;
    abstract public int close();
    abstract public int programWithAsm() throws Exception;
}
