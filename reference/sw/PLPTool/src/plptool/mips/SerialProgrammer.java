/*
    Copyright 2010-2011 David Fritz, Brian Gordon, Wira Mulia

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

package plptool.mips;

import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.NoSuchPortException;
import gnu.io.SerialPort;

import java.io.InputStream;
import java.io.OutputStream;

import plptool.Msg;
import plptool.Constants;

/**
 * PLPTool serial programmer plp.
 *
 * @author wira
 */
public class SerialProgrammer extends plptool.PLPSerialProgrammer {
    public SerialProgrammer(plptool.gui.ProjectDriver plp) {
        super(plp);
    }
    
    private CommPort commPort;
    private CommPortIdentifier portIdentifier;
    private InputStream in;
    private OutputStream out;

    public int connect(String portName, int baudRate) throws Exception {
        try {
            portIdentifier = CommPortIdentifier.getPortIdentifier(portName);
        } catch(NoClassDefFoundError e) {
            return Msg.E("Failed to link with native RXTX library.",
                            Constants.PLP_GENERIC_ERROR, this);
        } catch(UnsatisfiedLinkError e) {
            return Msg.E("Failed to link with native RXTX library.",
                            Constants.PLP_GENERIC_ERROR, this);
        } catch(gnu.io.NoSuchPortException e) {
            return Msg.E(portName + " is not found.",
                            Constants.PLP_GENERIC_ERROR, this);
        }

        if ( portIdentifier.isCurrentlyOwned() )
        {
            return Msg.E("Serial port " + portName + " is in use.",
                                    Constants.PLP_PRG_PORT_IN_USE, this);
        }
        else {
            commPort = portIdentifier.open(this.getClass().getName(),2000);

            if ( commPort instanceof SerialPort ) {
                plp.p_port = (SerialPort) commPort;
                plp.p_port.setSerialPortParams(baudRate, SerialPort.DATABITS_8,
                                               SerialPort.STOPBITS_1,
                                               SerialPort.PARITY_NONE);

                in = plp.p_port.getInputStream();
                out = plp.p_port.getOutputStream();
            }
            else {
                return Msg.E(portName + " is not a serial port.",
                                    Constants.PLP_PRG_NOT_A_SERIAL_PORT, this);
            }
        }

        return Constants.PLP_OK;
    }

    public int close() {
        try {
            in.close();
            out.close();
            plp.p_port.close();
            commPort.close();

            return Constants.PLP_OK;
        } catch(Exception e) {
            return Msg.E("Can not close serial port.",
                    Constants.PLP_PRG_UNABLE_TO_CLOSE_PORT, null);
        }
    }

    public int programWithAsm() throws Exception {
        if(plp.asm.isAssembled() && out != null) {
            long objCode[] = plp.asm.getObjectCode();
            long addrTable[] = plp.asm.getAddrTable();
            byte inData = '\0';
            plp.p_port.enableReceiveTimeout(500);

            Msg.D("Writing out first address " + String.format("0x%08x", addrTable[0]), 2, this);
            out.write('a');
            out.write((byte) (addrTable[0] >> 24));
            out.write((byte) (addrTable[0] >> 16));
            out.write((byte) (addrTable[0] >> 8));
            out.write((byte) (addrTable[0]));
            if(busy) inData = (byte) in.read();
            if(inData != 'f') {
                Msg.D("Acknowledgement byte: " +
                         String.format("0x%x", inData), 2, this);
                return Msg.E("Programming failed, no/invalid acknowledgement received. " +
                                "Check if the board is in programming mode.",
                                Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);
            }

            for(int i = 0; i < objCode.length; i++) {
                progress = i;
                plp.p_progress = i;

                if(plp.g()) {
                    plp.g_prg.getProgressBar().setValue(progress);
                    plp.g_prg.getStatusField().setText(progress + ": " +
                            String.format("0x%08x", addrTable[progress]) + " " +
                            String.format("0x%08x", objCode[progress]));
                    plp.g_prg.repaint();
                }

                Msg.D(progress + " out of " + (objCode.length - 1), 3, this);
                if(i < objCode.length - 1) {
                    if(addrTable[i + 1] != addrTable[i] + 4) {
                        out.write('a');
                        out.write((byte) (addrTable[i] >> 24));
                        out.write((byte) (addrTable[i] >> 16));
                        out.write((byte) (addrTable[i] >> 8));
                        out.write((byte) (addrTable[i]));
                        if(busy) inData = (byte) in.read();
                        if(inData != 'f')
                            return Msg.E("Programming failed, no acknowledgement received.",
                                            Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);
                    }
                }
                out.write('d');
                out.write((byte) (objCode[i] >> 24));
                out.write((byte) (objCode[i] >> 16));
                out.write((byte) (objCode[i] >> 8));
                out.write((byte) (objCode[i]));
                if(busy) inData = (byte) in.read();
                if(inData != 'f')
                    return Msg.E("Programming failed, no acknowledgement received.",
                                    Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);
            }

            out.write('a');
            out.write((byte) (addrTable[0] >> 24));
            out.write((byte) (addrTable[0] >> 16));
            out.write((byte) (addrTable[0] >> 8));
            out.write((byte) (addrTable[0]));
            if(busy) inData = (byte) in.read();
            if(inData != 'f')
                return Msg.E("Programming failed, no acknowledgement received.",
                                Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);

            out.write('j');
            if(busy) inData = (byte) in.read();
            if(inData != 'f')
                return Msg.E("Programming failed, no acknowledgement received.",
                                Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);
                    
        } else {
            return Msg.E("Source is not assembled.",
                            Constants.PLP_PRG_SOURCES_NOT_ASSEMBLED, this);
        }

        Msg.I("programWithAsm(): done!", this);

        return Constants.PLP_OK;
    }

    @Override
    public String toString() {
        return "plptool.mips.SerialProgrammer";
    }
}
