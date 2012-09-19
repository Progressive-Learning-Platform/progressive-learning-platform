/*
    Copyright 2012 David Fritz, Brian Gordon, Wira Mulia

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

package plptool.hc11;

import plptool.*;
import plptool.gui.ProjectDriver;

/**
 *
 * @author wira
 */
public class Architecture extends PLPArchitecture {

    public Architecture() {
        super(6811, "hc11", null);
        Msg.M("***************************************");
        Msg.M("PLP HC11 ISA Implementation Module");
        Msg.M("***************************************");
        hasAssembler = true;
        hasSimCore = true;
    }

    public PLPAsm createAssembler() {
        return new Asm(plp.getAsms());
    }

    public PLPSerialProgrammer createProgrammer() {
        return null;
    }

    public PLPSimCore createSimCore() {
        return new SimCore();
    }

    public PLPSimCoreGUI createSimCoreGUI() {
        return null;
    }

    @Override
    public void newProject(ProjectDriver plp) {

    }

    @Override
    public void cleanup() {
        Msg.M("PLP 68HC11 ISA Implementation is cleaning up.");
    }
}
