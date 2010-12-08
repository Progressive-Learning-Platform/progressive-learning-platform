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

package plpmips;

import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileInputStream;

import java.util.Scanner;

import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;
import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import plptool.PLPMsg;
import plptool.Constants;

/**
 * PLPTool serial programmer backend.
 *
 * @author wira
 */
public class PLPProgrammer {
    public PLPProgrammer() {
        super();
    }
    
    private SerialPort serialPort;
    private CommPort commPort;
    private CommPortIdentifier portIdentifier;
    private InputStream in;
    private OutputStream out;

    public int connect(String portName, int baudRate) throws Exception {
        portIdentifier = CommPortIdentifier.getPortIdentifier(portName);
        if ( portIdentifier.isCurrentlyOwned() )
        {
            return PLPMsg.E("Serial port " + portName + " is in use.",
                                    Constants.PLP_PRG_PORT_IN_USE, this);
        }
        else {
            commPort = portIdentifier.open(this.getClass().getName(),2000);

            if ( commPort instanceof SerialPort ) {
                serialPort = (SerialPort) commPort;
                serialPort.setSerialPortParams(baudRate, SerialPort.DATABITS_8,
                                               SerialPort.STOPBITS_1,
                                               SerialPort.PARITY_NONE);

                in = serialPort.getInputStream();
                out = serialPort.getOutputStream();
            }
            else {
                return PLPMsg.E(portName + " is not a serial port.",
                                    Constants.PLP_PRG_NOT_A_SERIAL_PORT, this);
            }
        }

        return Constants.PLP_OK;
    }

    public int programWithPLPFile(String plpFilePath) throws Exception {
        File plpFile = new File(plpFilePath);
        TarArchiveEntry entry;
        String metaStr;
        byte[] image;
        byte inData;

        if(!plpFile.exists())
            return PLPMsg.E(plpFilePath + " not found.",
                            Constants.PLP_PRG_PLP_FILE_NOT_FOUND, this);

        TarArchiveInputStream tIn = new TarArchiveInputStream(new FileInputStream(plpFile));

        while((entry = tIn.getNextTarEntry()) != null) {
            if(entry.getName().equals("plp.metafile")) {
                image = new byte[(int) entry.getSize()];
                tIn.read(image, 0, (int) entry.getSize());
                metaStr = new String(image);
                Scanner fScan = new Scanner(metaStr);
                fScan.findWithinHorizon("DIRTY=", 0);
                if(fScan.nextInt() == 1) {
                    return PLPMsg.E(plpFile + " does not have up to date image.",
                                    Constants.PLP_PRG_IMAGE_OUT_OF_DATE, this);
                }
            }
        }

        tIn = new TarArchiveInputStream(new FileInputStream(plpFile));
        while((entry = tIn.getNextTarEntry()) != null) {
            if(entry.getName().equals("plp.image")) {
                image = new byte[(int) entry.getSize()];
                tIn.read(image, 0, (int) entry.getSize());

                if(image.length % 4 != 0)
                    return PLPMsg.E(plpFilePath + " contains invalid image file.",
                        Constants.PLP_PRG_INVALID_IMAGE_FILE, this);
                
                out.write('a');
                out.write(0);
                out.write(0);
                out.write(0);
                out.write(0);
                inData = (byte) in.read();
                if(inData != 'f')
                    return PLPMsg.E("Programming failed, no acknowledgement received.",
                                    Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);

                for(int i = 0; i < image.length; i++) {
                    if(i % 4 == 0) {
                        if(i != 0) {
                            inData = (byte) in.read();
                            if(inData != 'f')
                            return PLPMsg.E("Programming failed, no acknowledgement received.",
                                         Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);
                        }
                        out.write('d');
                    }
                    out.write(image[i]);
                }
                out.write('a');
                out.write(0);
                out.write(0);
                out.write(0);
                out.write(0);
                inData = (byte) in.read();
                if(inData != 'f')
                    return PLPMsg.E("Programming failed, no acknowledgement received.",
                                    Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);

                out.write('j');
                inData = (byte) in.read();
                if(inData != 'f')
                    return PLPMsg.E("Programming failed, no acknowledgement received.",
                                    Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);

                tIn.close();

                return Constants.PLP_OK;
            }
        }

        tIn.close();

        return PLPMsg.E(plpFilePath + " is not a valid plp file.",
                        Constants.PLP_PRG_INVALID_PLP_FILE, this);
    }

    public int programWithAsm (PLPAsm asm) throws Exception {
        if(asm.isAssembled()) {
            long objCode[] = asm.getObjectCode();
            long addrTable[] = asm.getAddrTable();
            byte inData;

            for(int i = 0; i < objCode.length; i++) {
                if(i < objCode.length - 1) {
                    if(addrTable[i + 1] != addrTable[i] + 4) {
                        out.write('a');
                        out.write((int) (addrTable[i] >> 24));
                        out.write((int) (addrTable[i] >> 16));
                        out.write((int) (addrTable[i] >> 8));
                        out.write((int) (addrTable[i]));
                        inData = (byte) in.read();
                        if(inData != 'f')
                            return PLPMsg.E("Programming failed, no acknowledgement received.",
                                            Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);
                    }
                }
                out.write('d');
                out.write((int) (objCode[i] >> 24));
                out.write((int) (objCode[i] >> 16));
                out.write((int) (objCode[i] >> 8));
                out.write((int) (objCode[i]));
                inData = (byte) in.read();
                if(inData != 'f')
                    return PLPMsg.E("Programming failed, no acknowledgement received.",
                                    Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);
            }

            out.write('a');
            out.write(0);
            out.write(0);
            out.write(0);
            out.write(0);
            inData = (byte) in.read();
            if(inData != 'f')
                return PLPMsg.E("Programming failed, no acknowledgement received.",
                                Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);

            out.write('j');
            inData = (byte) in.read();
            if(inData != 'f')
                return PLPMsg.E("Programming failed, no acknowledgement received.",
                                Constants.PLP_PRG_SERIAL_TRANSMISSION_ERROR, this);
                    
        } else {
            return PLPMsg.E("Source is not assembled.",
                            Constants.PLP_PRG_SOURCES_NOT_ASSEMBLED, this);
        }

        return Constants.PLP_OK;
    }
}
