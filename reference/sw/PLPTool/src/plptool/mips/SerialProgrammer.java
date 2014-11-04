/*
    Copyright 2010-2013 David Fritz, Brian Gordon, Wira Mulia

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
import java.io.IOException;

import plptool.Msg;
import plptool.Constants;
import plptool.Config;

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

    private long objCode[];
    private long addrTable[];

    private int chunkIndex = 0;
    private long chunkStartAddr;

    // Serial programmer baudrate
    private static int BAUDRATE = 57600;

    // Serial programmer preambles
    private static long preamble[] = new long[2];

    // Preamble IDs
    private static long PLP_ISA_EMU_G02 = 0x02000000L;

    public int connect(String portName) throws Exception {
        Msg.D("Connecting to " + portName, 2, this);
        
        try {
            portIdentifier = CommPortIdentifier.getPortIdentifier(portName);
        } catch(NoClassDefFoundError e) {
            return Msg.E("Failed to link with native RXTX library.",
                            Constants.PLP_GENERIC_ERROR, this);
        } catch(UnsatisfiedLinkError e) {
            return Msg.E("Failed to link with native RXTX library.",
                            Constants.PLP_GENERIC_ERROR, this);
        } catch(NoSuchPortException e) {
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
                BAUDRATE = ((plptool.mips.Architecture)plp.getArch()).isUsingNexysBoard() ? 57600 : 115200;
                Msg.D("Baudrate=" + BAUDRATE, 2, this);
                plp.p_port = (SerialPort) commPort;
                plp.p_port.setSerialPortParams(BAUDRATE, SerialPort.DATABITS_8,
                                               SerialPort.STOPBITS_1,
                                               SerialPort.PARITY_NONE);

                in = plp.p_port.getInputStream();
                out = plp.p_port.getOutputStream();
                this.portName = portName;

                Msg.D("Port name: " + plp.p_port.getName(), 4, this);
            }
            else {
                return Msg.E(portName + " is not a serial port.",
                                    Constants.PLP_PRG_NOT_A_SERIAL_PORT, this);
            }
        }

        return Constants.PLP_OK;
    }

    public int close() {
        Msg.D("Closing " + portName, 2, this);

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
            int ret;
            objCode = plp.asm.getObjectCode();
            addrTable = plp.asm.getAddrTable();
            byte inData = '\0';
            byte buff[] = new byte[5];
            byte[] chunk = new byte[Constants.PLP_PRG_CHUNK_BUFFER_SIZE];
            boolean done = false;
            
            String status;
            plp.p_port.enableReceiveTimeout(Config.prgReadTimeout);

            long startTime = System.currentTimeMillis();

            ret = Constants.PLP_OK;

            Msg.D("Writing out first address " + String.format("0x%08x", addrTable[0]), 2, this);
            buff[0] = (byte) 'a';
            buff[1] = (byte) (addrTable[0] >> 24);
            buff[2] = (byte) (addrTable[0] >> 16);
            buff[3] = (byte) (addrTable[0] >> 8);
            buff[4] = (byte) (addrTable[0]);
            out.write(buff, 0, 5);
            if(isProgramming()) inData = (byte) in.read();
            if(inData != 'f') {
                Msg.D("Acknowledgement byte: " +
                         String.format("0x%x", inData), 2, this);
                return Msg.E("Programming failed, no/invalid acknowledgement received. " +
                                "Check if the board is in programming mode.",
                                Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);
            }

            chunkStartAddr = addrTable[0];
            status = "Programming...";
            for(int i = 0; i < objCode.length && isProgramming(); i++) {
                progress = i;
                plp.p_progress = i;

                if(plp.g())
                    plp.g_prg.getStatusField().setText(status);

                if(plp.g()) {
                    plp.g_prg.getProgressBar().setValue(progress);
                    if(!Config.prgProgramInChunks)
                        status = progress + ": " +
                                String.format("0x%08x", addrTable[progress]) + " " +
                                String.format("0x%08x", objCode[progress]);
                    else
                        status = "Buffering " +
                                progress + " of " + objCode.length + " words";

                    plp.g_prg.repaint();
                }

                Msg.D(progress + " out of " + (objCode.length - 1), 5, this);

                // non-chunk programming mode, send each word one-by-one
                if(!Config.prgProgramInChunks) {
                    buff[0] = 'd';
                    buff[1] = (byte) (objCode[i] >> 24);
                    buff[2] = (byte) (objCode[i] >> 16);
                    buff[3] = (byte) (objCode[i] >> 8);
                    buff[4] = (byte) (objCode[i]);
                    out.write(buff, 0, 5);
                    if(isProgramming()) inData = (byte) in.read();
                    if(inData != 'f')
                        return Msg.E("Programming failed, no acknowledgement received.",
                                        Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);

                // chunk mode, buffer object until next address is not addr+4
                // or we reach the end of the program
                } else {
                    chunk[chunkIndex++] = (byte) (objCode[i] >> 24);
                    chunk[chunkIndex++] = (byte) (objCode[i] >> 16);
                    chunk[chunkIndex++] = (byte) (objCode[i] >> 8);
                    chunk[chunkIndex++] = (byte) (objCode[i]);
                }

                if((i < objCode.length - 1) && (addrTable[i + 1] != addrTable[i] + 4)) {
                    // address jump, send chunk now
                    if(Config.prgProgramInChunks) {
                        ret = sendChunk(chunk, chunkIndex, i);
                        chunk = new byte[Constants.PLP_PRG_CHUNK_BUFFER_SIZE];
                        chunkIndex = 0;
                        chunkStartAddr = addrTable[i + 1];

                        if(ret != Constants.PLP_OK)
                            return ret;
                    }

                    Msg.D(String.format("Address jump: %08x to %08x",
                               addrTable[i], addrTable[i + 1]), 3, this);

                    // and send new address
                    buff[0] = (byte) 'a';
                    buff[1] = (byte) (addrTable[i + 1] >> 24);
                    buff[2] = (byte) (addrTable[i + 1] >> 16);
                    buff[3] = (byte) (addrTable[i + 1] >> 8);
                    buff[4] = (byte) (addrTable[i + 1]);
                    out.write(buff, 0, 5);
                    if(isProgramming()) inData = (byte) in.read();
                    if(inData != 'f')
                        return Msg.E("Programming failed, no acknowledgement received.",
                                        Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);
                    
                // if we buffered up to maximum chunk size, send data
                } else if(Config.prgProgramInChunks &&
                          chunkIndex == Config.prgMaxChunkSize &&
                          (i < objCode.length - 1)) {
                    if(!plp.g())
                        Msg.I("Buffered " +
                               progress + " of " + objCode.length + " words", this);
                    
                    ret = sendChunk(chunk, chunkIndex, i);
                    chunk = new byte[Constants.PLP_PRG_CHUNK_BUFFER_SIZE];
                    chunkIndex = 0;

                    if(i != objCode.length - 1)
                        chunkStartAddr = addrTable[i + 1];

                    if(ret != Constants.PLP_OK)
                        return ret;
                }

                // we're done
                if(i == objCode.length - 1) {

                    // send final chunk
                    if(Config.prgProgramInChunks) {
                        ret = sendChunk(chunk, chunkIndex, i);

                        if(ret != Constants.PLP_OK)
                            return ret;
                    }

                    done = true;
                }
            }

            if(done) {
                // jump to entrypoint
                long entry = plp.asm.getEntryPoint();
                Msg.D("Jumping to " + String.format("%08x", entry), 3, this);
                buff[0] = (byte) 'a';
                buff[1] = (byte) (entry >> 24);
                buff[2] = (byte) (entry >> 16);
                buff[3] = (byte) (entry >> 8);
                buff[4] = (byte) (entry);
                out.write(buff, 0, 5);
                if(isProgramming()) inData = (byte) in.read();
                if(inData != 'f')
                    return Msg.E("Programming failed, no acknowledgement received.",
                                    Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);

                out.write('j');
                if(isProgramming()) inData = (byte) in.read();
                if(inData != 'f')
                    return Msg.E("Programming failed, no acknowledgement received.",
                                    Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);

                double timeSecs = (System.currentTimeMillis()- startTime) / 1000.0;
                if(plp.g()) {
                    plp.g_prg.enableControls();
                    plp.g_prg.getStatusField().setText("Done. " + objCode.length +
                            " words in " + timeSecs + " seconds.");
                }
                Msg.I("done. " + objCode.length +
                            " words in " + timeSecs + " seconds.", this);

            // we were interrupted, try to reset board
            } else {
                Msg.I("interrupted.", this);

                Msg.D("Jumping to " + String.format("%08x", 0), 3, this);
                buff[0] = (byte) 'a';
                buff[1] = 0;
                buff[2] = 0;
                buff[3] = 0;
                buff[4] = 0;
                out.write(buff, 0, 5);
                if(isProgramming()) inData = (byte) in.read();
                if(inData != 'f')
                    return Msg.E("Programming failed, no acknowledgement received.",
                                    Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);

                out.write('j');
                if(isProgramming()) inData = (byte) in.read();
                if(inData != 'f')
                    return Msg.E("Programming failed, no acknowledgement received.",
                                    Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);
            }
        } else {
            return Msg.E("Source is not assembled.",
                            Constants.PLP_PRG_SOURCES_NOT_ASSEMBLED, this);
        }

        return Constants.PLP_OK;
    }

    private int sendChunk(byte[] chunk, int size, int i) throws IOException {
        int len = size / 4; // per chunk protocol, len is in WORDS
        
        Msg.D("Sending chunk of size " + size + " bytes.", 3, this);

        if(plp.g())
            plp.g_prg.getStatusField().setText("Transmitting " +
                    String.format("0x%08x", chunkStartAddr) + " to " +
                    String.format("0x%08x", addrTable[i]) +
                    " (" + chunkIndex +" bytes)");
        else
            Msg.I("Transmitting " +
                    String.format("0x%08x", chunkStartAddr) + " to " +
                    String.format("0x%08x", addrTable[i]) +
                    " (" + chunkIndex +" bytes)", this);

        byte buff[] = new byte[5];
        buff[0] = 'c'; 
        buff[1] = (byte) (len >> 24);
        buff[2] = (byte) (len >> 16);
        buff[3] = (byte) (len >> 8);
        buff[4] = (byte) (len);
        out.write(buff, 0, 5);
        out.write(chunk, 0, size);
        byte inData = '\0';
        if(isProgramming()) inData = (byte) in.read();
            if(inData != 'f')
                return Msg.E("Programming failed, no acknowledgement received.",
                                Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);

        return Constants.PLP_OK;
    }

    public static void resetPreamble() {
        for(int i = 0; i < preamble.length; i++)
            preamble[i] = 0;
    }

    public int sendPreamble(long preamble, InputStream in, OutputStream out) throws IOException {
        byte buff[] = new byte[5];
        byte inData = '\0';

        buff[0] = (byte) 'p';
        buff[1] = (byte) (preamble >> 24);
        buff[2] = (byte) (preamble >> 16);
        buff[3] = (byte) (preamble >> 8);
        buff[4] = (byte) (preamble);

        out.write(buff, 0, 5);
        if(isProgramming()) inData = (byte) in.read();
        if(inData != 'f')
            return Msg.E("Failed to send preamble, no acknowledgement received.",
                            Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, null);

        return Constants.PLP_OK;
    }

    public OutputStream getOutputStream() {
        return out;
    }

    public InputStream getInputStream() {
        return in;
    }

    @Override
    public String toString() {
        return "SerialProgrammer";
    }
}
