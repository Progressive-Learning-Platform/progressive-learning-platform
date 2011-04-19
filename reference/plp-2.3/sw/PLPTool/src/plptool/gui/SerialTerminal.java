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

package plptool.gui;

import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;

import java.io.InputStream;
import java.io.OutputStream;

import javax.swing.JTextPane;
import javax.swing.text.StyledDocument;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import java.awt.Color;

import plptool.Msg;
import plptool.Constants;
import plptool.PLPToolbox;

/**
 *
 * @author wira
 */
public class SerialTerminal extends javax.swing.JFrame {

    private CommPort commPort;
    private CommPortIdentifier portIdentifier;
    protected InputStream in;
    private OutputStream out;
    private SerialPort port;
    private boolean standalone;

    protected boolean stop;

    /** Creates new form SerialTerminal */
    public SerialTerminal(boolean standalone) {
        initComponents();

        this.standalone = standalone;
        stop = true;

        if(standalone) {
            this.addWindowListener(new java.awt.event.WindowAdapter() {
                @Override
                public void windowClosing(java.awt.event.WindowEvent winEvt) {
                    System.exit(-1);
                }
            });
        } else {
            this.addWindowListener(new java.awt.event.WindowAdapter() {
                @Override
                public void windowClosing(java.awt.event.WindowEvent winEvt) {
                    if(btnClose.isEnabled())
                        btnCloseActionPerformed(null);
                    setVisible(false);
                }
            });
        }



        cmbBaud.removeAllItems();
        cmbBaud.addItem(9600);
        cmbBaud.addItem(57600);
        cmbBaud.addItem(115200);
        cmbBaud.setSelectedIndex(1);

        cmbPort.removeAllItems();
        if(PLPToolbox.getOS(false) == Constants.PLP_OS_LINUX_32 ||
           PLPToolbox.getOS(false) == Constants.PLP_OS_LINUX_64) {
            cmbPort.addItem("/dev/ttyUSB0");
            cmbPort.addItem("/dev/ttyUSB1");
            cmbPort.addItem("/dev/ttyS0");
            cmbPort.addItem("/dev/ttyS1");
        }
        if(PLPToolbox.getOS(false) == Constants.PLP_OS_WIN_32 ||
           PLPToolbox.getOS(false) == Constants.PLP_OS_WIN_64) {
            cmbPort.addItem("COM1");
            cmbPort.addItem("COM2");
            cmbPort.addItem("COM3");
            cmbPort.addItem("COM4");
        }
        else
            cmbPort.addItem("Specify your serial port here.");

        cmbOpts.removeAllItems();
        cmbOpts.addItem("8N1");

        cmbFormat.removeAllItems();
        cmbFormat.addItem("ASCII String");
        cmbFormat.addItem("1-byte raw");
        cmbFormat.addItem("Space-delimited raw");

        this.setLocationRelativeTo(null);

        try {
            gnu.io.RXTXVersion.getVersion();
        } catch(UnsatisfiedLinkError e) {
            appendString("Failed to link with RXTX native library.");
            btnOpen.setEnabled(false);
        } catch(NoClassDefFoundError e) {
            appendString("Failed to link with RXTX native library.");
            btnOpen.setEnabled(false);
        }
    }

    protected void appendByte(char data, Color color) throws Exception {
        StyledDocument doc = console.getStyledDocument();

        SimpleAttributeSet attrib = new SimpleAttributeSet();
        StyleConstants.setBold(attrib, true);
        StyleConstants.setForeground(attrib, color);

        String toWrite;

        if(chkHEX.isSelected()) 
            toWrite = String.format("0x%x ", (int) data);
        else
            toWrite = data + "";

        doc.insertString(doc.getLength(), toWrite, attrib);

        console.setCaretPosition(doc.getLength() - 1);
    }

    protected void appendString(String str) {
        try {

        StyledDocument doc = console.getStyledDocument();

        SimpleAttributeSet attrib = new SimpleAttributeSet();
        StyleConstants.setBold(attrib, false);
        StyleConstants.setForeground(attrib, Color.GRAY);

        doc.insertString(doc.getLength(), "--- " + str + "\n", attrib);

        console.setCaretPosition(doc.getLength() - 1);

        } catch(Exception e) {

        }
    }

    public int openPort() {
        try {
            String portName = (String) cmbPort.getSelectedItem();
            appendString("Opening port " + portName + ".");
            int baudRate = (Integer) cmbBaud.getSelectedItem();
            portIdentifier = CommPortIdentifier.getPortIdentifier(portName);

            if (portIdentifier.isCurrentlyOwned()) {
                appendString("Serial port " + portName + " is in use.");
                return -1;
            } else {
                commPort = portIdentifier.open(this.getClass().getName(), 2000);

                if (commPort instanceof SerialPort) {
                    port = (SerialPort) commPort;
                    port.setSerialPortParams(baudRate, SerialPort.DATABITS_8,
                            SerialPort.STOPBITS_1,
                            SerialPort.PARITY_NONE);

                    port.enableReceiveTimeout(1000);
                    in = port.getInputStream();
                    out = port.getOutputStream();

                    btnOpen.setEnabled(false);
                    btnClose.setEnabled(true);
                    txtInput.setEnabled(true);
                    btnSend.setEnabled(true);
                    cmbBaud.setEnabled(false);
                    cmbPort.setEnabled(false);
                    cmbOpts.setEnabled(false);
                    stop = false;

                    (new SerialStreamReader()).start();
                    appendString("Connected.");
                } else {
                    appendString(portName + " is not a serial port.");
                    return -1;
                }
            }
        } catch (Exception e) {
            appendString("Error opening port.");
            System.err.println(e);
        }

        return 0;
    }

    public int closePort() {
        try {

        stop = true;

        in.close();
        out.close();
        port.close();
        commPort.close();

        btnOpen.setEnabled(true);
        btnClose.setEnabled(false);
        txtInput.setEnabled(false);
        btnSend.setEnabled(false);
        cmbBaud.setEnabled(true);
        cmbPort.setEnabled(true);
        cmbOpts.setEnabled(true);

        appendString("Port closed.");

        } catch(Exception e) {
            appendString("Error closing port.");
        }

        return 0;
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        lblPort = new javax.swing.JLabel();
        cmbPort = new javax.swing.JComboBox();
        lblBaud = new javax.swing.JLabel();
        cmbBaud = new javax.swing.JComboBox();
        lblOpts = new javax.swing.JLabel();
        cmbOpts = new javax.swing.JComboBox();
        txtInput = new javax.swing.JTextField();
        btnOpen = new javax.swing.JButton();
        btnClose = new javax.swing.JButton();
        btnClear = new javax.swing.JButton();
        btnSend = new javax.swing.JButton();
        cmbFormat = new javax.swing.JComboBox();
        jScrollPane1 = new javax.swing.JScrollPane();
        console = new javax.swing.JTextPane();
        chkHEX = new javax.swing.JCheckBox();
        chkEcho = new javax.swing.JCheckBox();

        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(plptool.gui.PLPToolApp.class).getContext().getResourceMap(SerialTerminal.class);
        setTitle(resourceMap.getString("Form.title")); // NOI18N
        setName("Form"); // NOI18N

        lblPort.setText(resourceMap.getString("lblPort.text")); // NOI18N
        lblPort.setName("lblPort"); // NOI18N

        cmbPort.setEditable(true);
        cmbPort.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Item 1", "Item 2", "Item 3", "Item 4" }));
        cmbPort.setName("cmbPort"); // NOI18N

        lblBaud.setText(resourceMap.getString("lblBaud.text")); // NOI18N
        lblBaud.setName("lblBaud"); // NOI18N

        cmbBaud.setEditable(true);
        cmbBaud.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Item 1", "Item 2", "Item 3", "Item 4" }));
        cmbBaud.setName("cmbBaud"); // NOI18N

        lblOpts.setText(resourceMap.getString("lblOpts.text")); // NOI18N
        lblOpts.setName("lblOpts"); // NOI18N

        cmbOpts.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Item 1", "Item 2", "Item 3", "Item 4" }));
        cmbOpts.setName("cmbOpts"); // NOI18N

        txtInput.setText(resourceMap.getString("txtInput.text")); // NOI18N
        txtInput.setEnabled(false);
        txtInput.setName("txtInput"); // NOI18N
        txtInput.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                txtInputKeyPressed(evt);
            }
        });

        btnOpen.setText(resourceMap.getString("btnOpen.text")); // NOI18N
        btnOpen.setName("btnOpen"); // NOI18N
        btnOpen.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnOpenActionPerformed(evt);
            }
        });

        btnClose.setText(resourceMap.getString("btnClose.text")); // NOI18N
        btnClose.setEnabled(false);
        btnClose.setName("btnClose"); // NOI18N
        btnClose.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnCloseActionPerformed(evt);
            }
        });

        btnClear.setText(resourceMap.getString("btnClear.text")); // NOI18N
        btnClear.setName("btnClear"); // NOI18N
        btnClear.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnClearActionPerformed(evt);
            }
        });

        btnSend.setText(resourceMap.getString("btnSend.text")); // NOI18N
        btnSend.setEnabled(false);
        btnSend.setName("btnSend"); // NOI18N
        btnSend.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSendActionPerformed(evt);
            }
        });

        cmbFormat.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Item 1", "Item 2", "Item 3", "Item 4" }));
        cmbFormat.setName("cmbFormat"); // NOI18N

        jScrollPane1.setName("jScrollPane1"); // NOI18N

        console.setEditable(false);
        console.setFont(resourceMap.getFont("console.font")); // NOI18N
        console.setName("console"); // NOI18N
        console.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                consoleKeyTyped(evt);
            }
        });
        jScrollPane1.setViewportView(console);

        chkHEX.setText(resourceMap.getString("chkHEX.text")); // NOI18N
        chkHEX.setName("chkHEX"); // NOI18N

        chkEcho.setText(resourceMap.getString("chkEcho.text")); // NOI18N
        chkEcho.setName("chkEcho"); // NOI18N

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(jScrollPane1, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 603, Short.MAX_VALUE)
                    .addGroup(javax.swing.GroupLayout.Alignment.LEADING, layout.createSequentialGroup()
                        .addComponent(chkHEX)
                        .addGap(18, 18, 18)
                        .addComponent(chkEcho))
                    .addGroup(javax.swing.GroupLayout.Alignment.LEADING, layout.createSequentialGroup()
                        .addComponent(lblPort)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(cmbPort, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(lblBaud)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(cmbBaud, javax.swing.GroupLayout.PREFERRED_SIZE, 119, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(lblOpts)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(cmbOpts, 0, 249, Short.MAX_VALUE))
                    .addGroup(javax.swing.GroupLayout.Alignment.LEADING, layout.createSequentialGroup()
                        .addComponent(btnOpen)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(btnClose)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 332, Short.MAX_VALUE)
                        .addComponent(btnClear))
                    .addGroup(javax.swing.GroupLayout.Alignment.LEADING, layout.createSequentialGroup()
                        .addComponent(cmbFormat, javax.swing.GroupLayout.PREFERRED_SIZE, 186, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(txtInput, javax.swing.GroupLayout.DEFAULT_SIZE, 346, Short.MAX_VALUE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(btnSend)))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(lblPort)
                    .addComponent(cmbPort, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(lblBaud)
                    .addComponent(cmbBaud, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(lblOpts)
                    .addComponent(cmbOpts, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(btnOpen)
                    .addComponent(btnClose)
                    .addComponent(btnClear))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 286, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(chkHEX)
                    .addComponent(chkEcho))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(cmbFormat, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(txtInput, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(btnSend))
                .addContainerGap())
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void btnOpenActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnOpenActionPerformed
        openPort();
    }//GEN-LAST:event_btnOpenActionPerformed

    private void btnCloseActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnCloseActionPerformed
        closePort();
    }//GEN-LAST:event_btnCloseActionPerformed

    private void btnClearActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnClearActionPerformed
        console.setText("");
    }//GEN-LAST:event_btnClearActionPerformed

    private void btnSendActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSendActionPerformed
        try {

        int dataout;

        switch(cmbFormat.getSelectedIndex()) {
            case 0:
                out.write(txtInput.getText().getBytes());
                if(chkEcho.isSelected())
                    for(int i = 0; i < txtInput.getText().length(); i++)
                        appendByte((char) txtInput.getText().charAt(i),
                                (chkHEX.isSelected() ? Color.MAGENTA :Color.BLUE));
                break;

            case 1:
                dataout = plptool.PLPToolbox.parseNumInt(txtInput.getText());

                if(dataout < 0 || dataout > 255) {
                    appendString("Invalid number.");
                    return;
                }

                out.write(dataout);
                if(chkEcho.isSelected())
                    appendByte((char) dataout,
                            (chkHEX.isSelected() ? Color.MAGENTA : Color.BLUE));
                break;

            case 2:
                String tokens[] = txtInput.getText().split("\\s+");


                for(int i = 0; i < tokens.length; i++) {
                    dataout = plptool.PLPToolbox.parseNumInt(tokens[i]);

                    if(dataout < 0 || dataout > 255) {
                        appendString("Invalid number.");
                        return;
                    }

                    out.write(dataout);
                    
                    if(chkEcho.isSelected())
                        appendByte((char) dataout,
                                (chkHEX.isSelected() ? Color.MAGENTA : Color.BLUE));
                }

                break;

            default:
        }

        } catch(Exception e) {
            try {
                
            appendString("Send failed.");

            } catch(Exception eb) {

            }
        }
    }//GEN-LAST:event_btnSendActionPerformed

    private void txtInputKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_txtInputKeyPressed
        if(evt.getKeyCode() == java.awt.event.KeyEvent.VK_ENTER)
            btnSendActionPerformed(null);
    }//GEN-LAST:event_txtInputKeyPressed

    private void consoleKeyTyped(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_consoleKeyTyped
        char data = evt.getKeyChar();

        try {

        out.write(data);

        if(chkEcho.isSelected())
            appendByte((char) data,
                    (chkHEX.isSelected() ? Color.MAGENTA : Color.BLUE));

        } catch(Exception e) {
            try {

            appendString("Send failed.");

            } catch(Exception eb) {

            }
        }
    }//GEN-LAST:event_consoleKeyTyped

    /**
    * @param args the command line arguments
    */
    public static void main(String args[]) {
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                new SerialTerminal(true).setVisible(true);
            }
        });
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btnClear;
    private javax.swing.JButton btnClose;
    private javax.swing.JButton btnOpen;
    private javax.swing.JButton btnSend;
    private javax.swing.JCheckBox chkEcho;
    private javax.swing.JCheckBox chkHEX;
    private javax.swing.JComboBox cmbBaud;
    private javax.swing.JComboBox cmbFormat;
    private javax.swing.JComboBox cmbOpts;
    private javax.swing.JComboBox cmbPort;
    private javax.swing.JTextPane console;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JLabel lblBaud;
    private javax.swing.JLabel lblOpts;
    private javax.swing.JLabel lblPort;
    private javax.swing.JTextField txtInput;
    // End of variables declaration//GEN-END:variables


    class SerialStreamReader extends Thread {
        @Override
        public void run() {
            int data;

            try {

            while(!stop) {
                data = in.read();
                if(data > -1)
                    appendByte((char) data, Color.RED);
            }

            appendString("Stream reader exiting.");

            } catch(Exception e) {

            }
        }
    }
}
