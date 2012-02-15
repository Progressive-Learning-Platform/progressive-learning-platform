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

import java.awt.event.ActionEvent;
import java.io.File;

/**
 *
 * @author wira
 */
public class PLPToolbox {
    /**
     * Parse a hexadecimal or binary number into unsigned long
     *
     * @param number Hex or binary number
     * @return number in long, -1 if error occurred
     */
    public static long parseNum(String number) {
        try {

        if(number.startsWith("0x") || number.startsWith("0h")) {
            number = number.substring(2);
            return Long.parseLong(number, 16);
        }
        else if(number.startsWith("0b")) {
            number = number.substring(2);
            return Long.parseLong(number, 2);
        }
        else
            return Long.parseLong(number);

        } catch(Exception e) {
            Msg.lastError = -1;
            return Msg.E("parseNum: '" + number + "' is not a valid number",
                            Constants.PLP_NUMBER_ERROR, null);
        }
    }

    /**
     * Parse a hexadecimal or binary number into unsigned long, but stay silent
     * if parsing failed.
     *
     * @param number Hex or binary number
     * @return number in long, -1 if error occurred
     */
    public static long parseNumSilent(String number) {
        try {

        if(number.startsWith("0x") || number.startsWith("0h")) {
            number = number.substring(2);
            return Long.parseLong(number, 16);
        }
        else if(number.startsWith("0b")) {
            number = number.substring(2);
            return Long.parseLong(number, 2);
        }
        else
            return Long.parseLong(number);

        } catch(Exception e) {
            Msg.lastError = -1;
            return Constants.PLP_NUMBER_ERROR;
        }
    }

    /**
     * Parse a hexadecimal or binary number into unsigned int
     *
     * @param number Hex or binary number
     * @return number in int, -1 if error occurred
     */
    public static int parseNumInt(String number) {
        try {

        if(number.startsWith("0x") || number.startsWith("0h")) {
            number = number.substring(2);
            return Integer.parseInt(number, 16);
        }
        else if(number.startsWith("0b")) {
            number = number.substring(2);
            return Integer.parseInt(number, 2);
        }
        else
            return Integer.parseInt(number);

        } catch(Exception e) {
            Msg.lastError = -1;
            return Msg.E("parseNum: '" + number + "' is not a valid number",
                            Constants.PLP_NUMBER_ERROR, null);
        }
    }

    /**
     * Try if the given label resolves to an address. If not, parse the
     * string as an address.
     *
     * @param label Label to resolve
     * @param asm Assembly object
     * @return The address of label if the label resolves, if not, the address
     * if it parses as an address, -1 otherwise
     */
    public static long tryResolveLabel(String label, PLPAsm asm) {
        long addr = -1;

        addr = asm.resolveAddress(label);
        if(addr == -1)
            addr = parseNumSilent(label);

        return addr;
    }

    /**
     * Format a long as 8 digit hexadecimal in String prefixed with '0x'
     *
     * @param num Number to format
     * @return 8 digit hex format of the number in String
     */
    public static String format32Hex(long num) {
        return String.format("0x%08x", num);
    }

    /**
     * Convert 32-bit word into printable ASCII
     *
     * @param word Word to format in long
     * @return String representation of the word
     */
    public static String asciiWord(long word) {
        String tStr = "";
        long tVal;
        for(int j = 3; j >= 0; j--) {
            tVal = word >> (8 * j);
            tVal &= 0xFF;
            if(tVal >= 0x21 && tVal <= 0x7E)
                tStr += (char) tVal + (j == 0 ? "" : " ");
            else
                tStr += "." + (j == 0 ? "" : " ");
        }

        return tStr;
    }

    /**
     * Get the configuration directory path
     *
     * @return Configuration directory path in String
     */
    public static String getConfDir() {
        return System.getProperty("user.home") + "/.plp";
    }

    /**
     * Check if the configuration directory exists.
     *
     * @return True if yes, false otherwise
     */
    public static boolean confDirExists() {
        java.io.File cfgDir = new java.io.File(getConfDir());
        return (cfgDir.exists() && cfgDir.isDirectory());
    }

    /**
     * Get host OS ID
     *
     * @param print Print host OS information to message out
     * @return OS code (See Constants.PLP_OS_*)
     */
    public static int getOS(boolean print) {
        String osIdentifier = System.getProperty("os.name");
        String osArch = System.getProperty("os.arch");

        if(print) Msg.M(osIdentifier + " " + osArch);

        if(osIdentifier.equals("Linux")) {
            if(osArch.equals("x86") || osArch.equals("i386"))
                return Constants.PLP_OS_LINUX_32;
            else if(osArch.equals("x86_64") || osArch.equals("amd64"))
                return Constants.PLP_OS_LINUX_64;
        }
        else if(osIdentifier.startsWith("Windows")) {
            if(osArch.equals("x86") || osArch.equals("i386"))
                return Constants.PLP_OS_WIN_32;
            else if(osArch.equals("x86_64") || osArch.equals("amd64"))
                return Constants.PLP_OS_WIN_64;
        }

        return Constants.PLP_OS_UNKNOWN;
    }

    /**
     * Check whether the host OS is GNU/Linux
     *
     * @return True for GNU/Linux, false otherwise
     */
    public static boolean isHostLinux() {
        return (getOS(false) == Constants.PLP_OS_LINUX_32 ||
                getOS(false) == Constants.PLP_OS_LINUX_64);
    }

    /**
     * Attach a keylistener that will hide the frame when the escape key is
     * pressed by the user
     *
     * @param frame Reference to the frame to attach the listener to
     */
    public static void attachHideOnEscapeListener(final javax.swing.JFrame frame) {
        javax.swing.KeyStroke escapeKeyStroke = javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_ESCAPE, 0, false);
        javax.swing.Action escapeAction = new javax.swing.AbstractAction() {
            public void actionPerformed(ActionEvent e) {
               frame.setVisible(false);
            }
        };

        frame.getRootPane().getInputMap(javax.swing.JComponent.WHEN_IN_FOCUSED_WINDOW).put(escapeKeyStroke, "ESCAPE");
        frame.getRootPane().getActionMap().put("ESCAPE", escapeAction);
    }

    /**
     * Attach a keylistener that will hide the dialog when the escape key is
     * pressed by the user
     *
     * @param frame Reference to the dialog to attach the listener to
     */
    public static void attachHideOnEscapeListener(final javax.swing.JDialog frame) {
        javax.swing.KeyStroke escapeKeyStroke = javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_ESCAPE, 0, false);
        javax.swing.Action escapeAction = new javax.swing.AbstractAction() {
            public void actionPerformed(ActionEvent e) {
               frame.setVisible(false);
            }
        };

        frame.getRootPane().getInputMap(javax.swing.JComponent.WHEN_IN_FOCUSED_WINDOW).put(escapeKeyStroke, "ESCAPE");
        frame.getRootPane().getActionMap().put("ESCAPE", escapeAction);
    }

    /**
     * Setup Classroom extras
     */
    public static void setupClassroomExtras(plptool.gui.ProjectDriver plp) {
        javax.swing.JOptionPane.showMessageDialog(plp.g_dev, "Not implemented yet!");
    }

    /**
     * Attempt to download a JAR file from an URL. Also provides an option of
     * loading and applying the module's manifest immediately after.
     *
     * @param URL Location of the JAR file
     * @param plp Reference to the ProjectDriver instance
     * @param load Should the method load the module after downloading
     */
    public static boolean downloadJARForAutoload(String URL, plptool.gui.ProjectDriver plp, boolean load) {
       
        File autoloadDir = new File(getConfDir() + "/autoload");
        if(!autoloadDir.exists())
            autoloadDir.mkdir();
        int ret = 0;
        if(plp != null && plp.g())
            ret = javax.swing.JOptionPane.showConfirmDialog(plp.g_dev,
                          "Attempt to download " + URL + " and cache it in user's directory?",
                          "Download JAR Module", javax.swing.JOptionPane.YES_NO_OPTION);
        if(plp == null || !plp.g() || ret == javax.swing.JOptionPane.YES_OPTION) {
            String fileName = "";
            try {
                Msg.I("Downloading " + URL + "...", null);
                java.net.URL jar = new java.net.URL(URL);
                java.nio.channels.ReadableByteChannel rbc = java.nio.channels.Channels.newChannel(jar.openStream());
                String[] tokens = jar.getFile().split("/");
                fileName = tokens[tokens.length-1];
                java.io.FileOutputStream fos = new java.io.FileOutputStream(
                        PLPToolbox.getConfDir() + "/autoload/" + fileName);
                fos.getChannel().transferFrom(rbc, 0, 1 << 24);
                fos.close();
                if(!DynamicModuleFramework.checkForManifest(PLPToolbox.getConfDir() + "/autoload/" + fileName)) {
                    Msg.E("Downloaded JAR file does not contain plp.manifest",
                          Constants.PLP_GENERIC_ERROR, null);
                    (new File(PLPToolbox.getConfDir() + "/autoload/" + fileName)).delete();
                    return false;
                }
            } catch(Exception e) {
                Msg.E("Failed to fetch " + URL + ".",
                        Constants.PLP_GENERIC_ERROR, null);
                if(Constants.debugLevel >= 2)
                    e.printStackTrace();
                return false;
            }
            if(load) {
                String searchPath = PLPToolbox.getConfDir() + "/autoload/" + fileName;
                String[] manifest = DynamicModuleFramework.loadJarWithManifest(searchPath);
                if(manifest != null)
                    DynamicModuleFramework.applyManifestEntries(manifest, plp);
            }
            return true;
        }
        return false;
    }
}

