/*
    Copyright 2010-2014 David Fritz, Brian Gordon, Wira Mulia

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

import javax.swing.text.StyledDocument;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import java.awt.Color;
import java.awt.Font;
import java.util.Enumeration;
import java.util.ArrayList;

import plptool.Constants;
import plptool.Config;
import plptool.PLPToolbox;
import plptool.Msg;

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
    private boolean streamReaderRunning;
    private String lastCommand;
    private boolean serialSupport;
    private ArrayList<String> historyBuffer;
    private int historyBufferIterator;

    protected boolean stop;

    /** Creates new form SerialTerminal */
    public SerialTerminal(boolean standalone) {
        initComponents();

        stop = true;
        streamReaderRunning = false;

        if(standalone) {
            this.addWindowListener(new java.awt.event.WindowAdapter() {
                @Override
                public void windowClosing(java.awt.event.WindowEvent winEvt) {
                    System.exit(-1);
                }
            });
        } else {
            javax.swing.KeyStroke escapeKeyStroke = javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_ESCAPE, 0, false);
            javax.swing.Action escapeAction = new javax.swing.AbstractAction() {
                public void actionPerformed(java.awt.event.ActionEvent e) {
                    if(btnClose.isEnabled())
                        btnCloseActionPerformed(null);

                    setVisible(false);
                }
            };

            getRootPane().getInputMap(javax.swing.JComponent.WHEN_IN_FOCUSED_WINDOW).put(escapeKeyStroke, "ESCAPE");
            getRootPane().getActionMap().put("ESCAPE", escapeAction);

            this.addWindowListener(new java.awt.event.WindowAdapter() {
                @Override
                public void windowClosing(java.awt.event.WindowEvent winEvt) {
                    if(btnClose.isEnabled())
                        btnCloseActionPerformed(null);
                    setVisible(false);
                }
            });
        }

        historyBuffer = new ArrayList<String>();
        historyBufferIterator = 0;

        cmbBaud.removeAllItems();
        cmbBaud.addItem(9600);
        cmbBaud.addItem(57600);
        cmbBaud.addItem(115200);
        cmbBaud.setSelectedIndex(1);

        serialSupport = false;
        try {
            gnu.io.RXTXVersion.getVersion();
            serialSupport = true;
        } catch(UnsatisfiedLinkError e) {
            appendString("Failed to link with RXTX native library.");
            btnOpen.setEnabled(false);
        } catch(NoClassDefFoundError e) {
            appendString("Failed to link with RXTX native library.");
            btnOpen.setEnabled(false);
        }

        cmbPort.removeAllItems();
        if(serialSupport && Config.serialTerminalAutoDetectPorts) {
            Enumeration portList = CommPortIdentifier.getPortIdentifiers();
            while (portList.hasMoreElements()) {
                CommPortIdentifier portId = (CommPortIdentifier) portList.nextElement();
                Msg.D("rxtx portId name: " + portId.getName() + " type: " + portId.getPortType(), 2 , null);
                if (portId.getPortType() == CommPortIdentifier.PORT_SERIAL) {
                    cmbPort.addItem(portId.getName());
                }
            }
        } else {
            if(PLPToolbox.getOS(false) == Constants.PLP_OS_LINUX_32 ||
               PLPToolbox.getOS(false) == Constants.PLP_OS_LINUX_64) {
                cmbPort.addItem("/dev/ttyUSB0");
                cmbPort.addItem("/dev/ttyUSB1");
                cmbPort.addItem("/dev/ttyS0");
                cmbPort.addItem("/dev/ttyS1");
            } else if(PLPToolbox.getOS(false) == Constants.PLP_OS_WIN_32 ||
               PLPToolbox.getOS(false) == Constants.PLP_OS_WIN_64) {
                cmbPort.addItem("COM1");
                cmbPort.addItem("COM2");
                cmbPort.addItem("COM3");
                cmbPort.addItem("COM4");
            } else
                cmbPort.addItem("Specify your serial port here.");
        }

        cmbOpts.removeAllItems();
        cmbOpts.addItem("8N1");

        cmbEnter.removeAllItems();
        cmbEnter.addItem("CR (0xD)");
        cmbEnter.addItem("LF (0xA)");
        cmbEnter.addItem("CR LF");
        cmbEnter.addItem("LF CR");

        cmbFormat.removeAllItems();
        cmbFormat.addItem("ASCII String");
        cmbFormat.addItem("1-byte number");
        cmbFormat.addItem("Space-delimited numbers");
        cmbFormat.addItem("ASCII String, append CR (0xD)");

        cmbFontSize.removeAllItems();
        cmbFontSize.addItem("8");
        cmbFontSize.addItem("9");
        cmbFontSize.addItem("10");
        cmbFontSize.addItem("11");
        cmbFontSize.addItem("12");
        cmbFontSize.addItem("14");
        cmbFontSize.addItem("16");
        cmbFontSize.addItem("20");
        cmbFontSize.addItem("24");
        cmbFontSize.addItem("32");
        cmbFontSize.addItem("48");
        cmbFontSize.setSelectedIndex(4);

        console.setFont(new Font("Monospaced", Font.PLAIN, 12));
        console.setForeground(Color.GREEN);
        console.setBackground(Color.BLACK);

        this.setLocationRelativeTo(null);
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

    protected final void appendStringFormatted(String data, Color color) throws Exception {
        StyledDocument doc = console.getStyledDocument();

        SimpleAttributeSet attrib = new SimpleAttributeSet();
        StyleConstants.setBold(attrib, true);
        StyleConstants.setForeground(attrib, color);

        String toWrite = "";

        if(chkHEX.isSelected()) {
            for(int i = 0; i < data.length(); i++)
                toWrite += String.format("0x%x ", (int) data.charAt(i));
        } else {
            toWrite = data + "";
        }

        doc.insertString(doc.getLength(), toWrite, attrib);

        console.setCaretPosition(doc.getLength() - 1);
    }

    protected final void appendString(String str) {
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
        cmbPort = new javax.swing.JComboBox<String>();
        lblBaud = new javax.swing.JLabel();
        cmbBaud = new javax.swing.JComboBox<Integer>();
        lblOpts = new javax.swing.JLabel();
        cmbOpts = new javax.swing.JComboBox<String>();
        txtInput = new javax.swing.JTextField();
        btnOpen = new javax.swing.JButton();
        btnClose = new javax.swing.JButton();
        btnClear = new javax.swing.JButton();
        btnSend = new javax.swing.JButton();
        cmbFormat = new javax.swing.JComboBox<String>();
        jScrollPane1 = new javax.swing.JScrollPane();
        console = new javax.swing.JTextPane();
        chkHEX = new javax.swing.JCheckBox();
        chkEcho = new javax.swing.JCheckBox();
        btnCopyAll = new javax.swing.JButton();
        btnCopySelection = new javax.swing.JButton();
        chkUnprintable = new javax.swing.JCheckBox();
        chkEnter = new javax.swing.JCheckBox();
        cmbEnter = new javax.swing.JComboBox<String>();
        jLabel1 = new javax.swing.JLabel();
        lblClickNotice = new javax.swing.JLabel();
        btnSave = new javax.swing.JButton();
        cmbFontSize = new javax.swing.JComboBox<String>();
        lblFontSize = new javax.swing.JLabel();
        btnSendFile = new javax.swing.JButton();

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

        btnClear.setMnemonic('L');
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
        console.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusGained(java.awt.event.FocusEvent evt) {
                consoleFocusGained(evt);
            }
            public void focusLost(java.awt.event.FocusEvent evt) {
                consoleFocusLost(evt);
            }
        });
        console.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                consoleKeyTyped(evt);
            }
            public void keyPressed(java.awt.event.KeyEvent evt) {
                consoleKeyPressed(evt);
            }
        });
        jScrollPane1.setViewportView(console);

        chkHEX.setMnemonic('X');
        chkHEX.setText(resourceMap.getString("chkHEX.text")); // NOI18N
        chkHEX.setName("chkHEX"); // NOI18N

        chkEcho.setMnemonic('E');
        chkEcho.setText(resourceMap.getString("chkEcho.text")); // NOI18N
        chkEcho.setName("chkEcho"); // NOI18N

        btnCopyAll.setMnemonic('A');
        btnCopyAll.setText(resourceMap.getString("btnCopyAll.text")); // NOI18N
        btnCopyAll.setName("btnCopyAll"); // NOI18N
        btnCopyAll.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnCopyAllActionPerformed(evt);
            }
        });

        btnCopySelection.setMnemonic('S');
        btnCopySelection.setText(resourceMap.getString("btnCopySelection.text")); // NOI18N
        btnCopySelection.setName("btnCopySelection"); // NOI18N
        btnCopySelection.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnCopySelectionActionPerformed(evt);
            }
        });

        chkUnprintable.setMnemonic('D');
        chkUnprintable.setSelected(true);
        chkUnprintable.setText(resourceMap.getString("chkUnprintable.text")); // NOI18N
        chkUnprintable.setName("chkUnprintable"); // NOI18N

        chkEnter.setMnemonic('I');
        chkEnter.setText(resourceMap.getString("chkEnter.text")); // NOI18N
        chkEnter.setName("chkEnter"); // NOI18N

        cmbEnter.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Item 1", "Item 2", "Item 3", "Item 4" }));
        cmbEnter.setName("cmbEnter"); // NOI18N

        jLabel1.setText(resourceMap.getString("jLabel1.text")); // NOI18N
        jLabel1.setName("jLabel1"); // NOI18N

        lblClickNotice.setForeground(resourceMap.getColor("lblClickNotice.foreground")); // NOI18N
        lblClickNotice.setText(resourceMap.getString("lblClickNotice.text")); // NOI18N
        lblClickNotice.setName("lblClickNotice"); // NOI18N

        btnSave.setText(resourceMap.getString("btnSave.text")); // NOI18N
        btnSave.setName("btnSave"); // NOI18N
        btnSave.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSaveActionPerformed(evt);
            }
        });

        cmbFontSize.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Item 1", "Item 2", "Item 3", "Item 4" }));
        cmbFontSize.setName("cmbFontSize"); // NOI18N
        cmbFontSize.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cmbFontSizeActionPerformed(evt);
            }
        });

        lblFontSize.setText(resourceMap.getString("lblFontSize.text")); // NOI18N
        lblFontSize.setName("lblFontSize"); // NOI18N

        btnSendFile.setText(resourceMap.getString("btnSendFile.text")); // NOI18N
        btnSendFile.setName("btnSendFile"); // NOI18N
        btnSendFile.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSendFileActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(jScrollPane1, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 823, Short.MAX_VALUE)
                    .addGroup(javax.swing.GroupLayout.Alignment.LEADING, layout.createSequentialGroup()
                        .addComponent(chkHEX)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(chkEcho)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(chkUnprintable)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(chkEnter)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(cmbEnter, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(btnSendFile)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 214, Short.MAX_VALUE)
                        .addComponent(btnSave))
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
                        .addComponent(cmbOpts, javax.swing.GroupLayout.PREFERRED_SIZE, 86, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(javax.swing.GroupLayout.Alignment.LEADING, layout.createSequentialGroup()
                        .addComponent(btnOpen)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(btnClose)
                        .addGap(18, 18, 18)
                        .addComponent(lblClickNotice)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 22, Short.MAX_VALUE)
                        .addComponent(lblFontSize)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(cmbFontSize, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(btnCopySelection)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(btnCopyAll)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(btnClear))
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(jLabel1)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(cmbFormat, javax.swing.GroupLayout.PREFERRED_SIZE, 186, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(txtInput, javax.swing.GroupLayout.DEFAULT_SIZE, 522, Short.MAX_VALUE)
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
                    .addComponent(btnClear)
                    .addComponent(btnCopyAll)
                    .addComponent(btnCopySelection)
                    .addComponent(lblClickNotice)
                    .addComponent(cmbFontSize, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(lblFontSize))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 289, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(chkHEX)
                    .addComponent(chkEcho)
                    .addComponent(chkUnprintable)
                    .addComponent(chkEnter)
                    .addComponent(cmbEnter, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(btnSave)
                    .addComponent(btnSendFile))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(txtInput, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(btnSend)
                    .addComponent(jLabel1)
                    .addComponent(cmbFormat, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
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

            case 3:
                out.write(txtInput.getText().getBytes());
                out.write(0xD);
                if(chkEcho.isSelected())
                    for(int i = 0; i < txtInput.getText().length(); i++)
                        appendByte((char) txtInput.getText().charAt(i),
                                (chkHEX.isSelected() ? Color.MAGENTA :Color.BLUE));
                break;

            default:
        }

        } catch(Exception e) {
            try {
                
            appendString("Send failed.");

            } catch(Exception eb) {

            }
        }

        historyBuffer.add(txtInput.getText());
        historyBufferIterator = historyBuffer.size();
        txtInput.setText("");
    }//GEN-LAST:event_btnSendActionPerformed

    private void txtInputKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_txtInputKeyPressed
        if(evt.getKeyCode() == java.awt.event.KeyEvent.VK_ENTER)
            btnSendActionPerformed(null);
        else if(evt.getKeyCode() == java.awt.event.KeyEvent.VK_UP) {
            if(historyBufferIterator == historyBuffer.size()){
                lastCommand = txtInput.getText();
            }
            if(historyBufferIterator > 0) {
                txtInput.setText(historyBuffer.get(historyBufferIterator-1));
                historyBufferIterator--;
            }
        } else if(evt.getKeyCode() == java.awt.event.KeyEvent.VK_DOWN) {
            if(historyBufferIterator < historyBuffer.size()-1) {
                txtInput.setText(historyBuffer.get(historyBufferIterator+1));
                historyBufferIterator++;
            } else if(historyBufferIterator == historyBuffer.size()-1){
                txtInput.setText(lastCommand);
                historyBufferIterator++;
            }
        }
    }//GEN-LAST:event_txtInputKeyPressed

    private void consoleKeyTyped(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_consoleKeyTyped

    }//GEN-LAST:event_consoleKeyTyped

    private void btnCopyAllActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnCopyAllActionPerformed
        PLPToolbox.copy(console.getText());
    }//GEN-LAST:event_btnCopyAllActionPerformed

    private void btnCopySelectionActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnCopySelectionActionPerformed
        PLPToolbox.copy(console.getSelectedText());
    }//GEN-LAST:event_btnCopySelectionActionPerformed

    private void consoleKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_consoleKeyPressed
        char data = evt.getKeyChar();
        int code = evt.getKeyCode();
        
        try {
            if(code == java.awt.event.KeyEvent.VK_CONTROL) {

            } else if(code == java.awt.event.KeyEvent.VK_ALT) {

            } else if(code == java.awt.event.KeyEvent.VK_SHIFT) {

            } else if(code == java.awt.event.KeyEvent.VK_META) {

            } else if(chkEnter.isSelected() && code == java.awt.event.KeyEvent.VK_ENTER) {
                switch(cmbEnter.getSelectedIndex()) {
                    case 0:
                        out.write(0xD);
                        break;
                    case 1:
                        out.write(0xA);
                        break;
                    case 2:
                        out.write(0xD);
                        out.write(0xA);
                        break;
                    case 3:
                        out.write(0xA);
                        out.write(0xD);
                        break;
                    default:

                }
            } else {
                out.write(data);

                if(chkEcho.isSelected())
                    appendByte((char) data,
                            (chkHEX.isSelected() ? Color.MAGENTA : Color.BLUE));
            }

        } catch(Exception e) {
            try {

            appendString("Send failed.");

            } catch(Exception eb) {

            }
        }
    }//GEN-LAST:event_consoleKeyPressed

    private void btnSaveActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSaveActionPerformed
        java.io.File f = PLPToolbox.saveFileDialog(Constants.launchPath);
        if(f != null)
            PLPToolbox.writeFile(console.getText(), f.getAbsolutePath());
    }//GEN-LAST:event_btnSaveActionPerformed

    private void consoleFocusGained(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_consoleFocusGained
        lblClickNotice.setVisible(false);
    }//GEN-LAST:event_consoleFocusGained

    private void consoleFocusLost(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_consoleFocusLost
        lblClickNotice.setVisible(true);        
    }//GEN-LAST:event_consoleFocusLost

    private void cmbFontSizeActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cmbFontSizeActionPerformed
        String size = (String) cmbFontSize.getSelectedItem();
        if(size != null) {
            console.setFont(new Font("Monospaced", Font.PLAIN, Integer.parseInt(size)));
        }
    }//GEN-LAST:event_cmbFontSizeActionPerformed

    private void btnSendFileActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSendFileActionPerformed
        java.io.File f = PLPToolbox.openFileDialog(System.getProperty("user.home"));
        if(f != null) {
            byte[] buf = PLPToolbox.readFile(f.getAbsolutePath());
            try {
                out.write(buf);
            } catch(Exception e) {
                appendString("Send failed.");
            }
        }
    }//GEN-LAST:event_btnSendFileActionPerformed

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
    private javax.swing.JButton btnCopyAll;
    private javax.swing.JButton btnCopySelection;
    private javax.swing.JButton btnOpen;
    private javax.swing.JButton btnSave;
    private javax.swing.JButton btnSend;
    private javax.swing.JButton btnSendFile;
    private javax.swing.JCheckBox chkEcho;
    private javax.swing.JCheckBox chkEnter;
    private javax.swing.JCheckBox chkHEX;
    private javax.swing.JCheckBox chkUnprintable;
    private javax.swing.JComboBox<Integer> cmbBaud;
    private javax.swing.JComboBox<String> cmbEnter;
    private javax.swing.JComboBox<String> cmbFontSize;
    private javax.swing.JComboBox<String> cmbFormat;
    private javax.swing.JComboBox<String> cmbOpts;
    private javax.swing.JComboBox<String> cmbPort;
    private javax.swing.JTextPane console;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JLabel lblBaud;
    private javax.swing.JLabel lblClickNotice;
    private javax.swing.JLabel lblFontSize;
    private javax.swing.JLabel lblOpts;
    private javax.swing.JLabel lblPort;
    private javax.swing.JTextField txtInput;
    // End of variables declaration//GEN-END:variables


    class SerialStreamReader extends Thread {
        int bytes;

        @Override
        public void run() {
            final byte[] buffer = new byte[Config.serialTerminalBufferSize];           
            byte c;
            int i;

            try {                
                if(streamReaderRunning) {
                   appendString("Another stream reader thread is already running.");
                   return;
                }
                appendString("Stream reader is running.");

                streamReaderRunning = true;

                while(!stop) {
                    bytes = in.read(buffer);
                    Msg.D("term: " + bytes + " bytes read.", 6, this);
                    if(bytes > 0)
                        try {
                            for(i = 0; i < bytes; i++) {
                                c = buffer[i];
                                if(chkUnprintable.isSelected() &&
                                        (c > 127 || c < 9 || (c < 32 && c > 13) || c == 11 || c == 12))
                                    buffer[i] = '.';
                            }

                            appendStringFormatted(new String(buffer, 0, bytes, "US-ASCII"), Color.GREEN);
                        } catch(Exception e) {

                        }
                    Thread.sleep(Config.serialTerminalReadDelayMs);
                }

                streamReaderRunning = false;
                appendString("Stream reader is stopped.");

            } catch(Exception e) {
                streamReaderRunning = false;
            }
        }
    }
}
