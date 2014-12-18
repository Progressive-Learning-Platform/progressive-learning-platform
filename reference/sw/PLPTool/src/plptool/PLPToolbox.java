/*
    Copyright 2012-2013 David Fritz, Brian Gordon, Wira Mulia

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

import plptool.dmf.DynamicModuleFramework;
import java.awt.event.ActionEvent;
import java.io.*;
import java.util.jar.*;
import java.util.*;
import java.lang.management.ManagementFactory;
import java.lang.management.RuntimeMXBean;

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
            return Msg.E("Number error: '" + number + "' is not a valid number",
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
            return Msg.E("Number error: '" + number + "' is not a valid number",
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
        File confDir = new File(System.getProperty("user.home") + "/.plp");
        if(!confDir.exists())
            confDir.mkdir();

        return confDir.getAbsolutePath();
    }

    /**
     * Get the temporary directory path
     *
     * @return Temporary directory path in String
     */
    public static String getTmpDir() {
        checkCreateTempDirectory();
        return getConfDir() + "/tmp";
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
     * Attach a keylistener that will bring up the debug console
     *
     * @param frame Reference to the frame to attach the listener to
     */
    public static void attachDebugConsoleMagicComboListener(final javax.swing.JFrame frame, final plptool.gui.ProjectDriver plp, final boolean show) {
        javax.swing.KeyStroke consoleKeyStroke = javax.swing.KeyStroke.getKeyStroke(
                java.awt.event.KeyEvent.VK_BACK_SLASH,
                java.awt.event.InputEvent.SHIFT_DOWN_MASK | java.awt.event.InputEvent.CTRL_DOWN_MASK);
        javax.swing.Action consoleAction = new javax.swing.AbstractAction() {
            public void actionPerformed(ActionEvent e) {
                if(plptool.gui.PLPToolApp.con == null)
                    plptool.gui.PLPToolApp.con = new plptool.gui.frames.ConsoleFrame(plp);
                if(show)
                    plptool.gui.PLPToolApp.con.setVisible(true);
                else
                    plptool.gui.PLPToolApp.con.setVisible(false);
            }
        };

        frame.getRootPane().getInputMap(javax.swing.JComponent.WHEN_IN_FOCUSED_WINDOW).put(consoleKeyStroke, "CONSOLE");
        frame.getRootPane().getActionMap().put("CONSOLE", consoleAction);
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
    public static boolean downloadJARForAutoload(
            String URL, plptool.gui.ProjectDriver plp, boolean load) {
       
        File autoloadDir = new File(getConfDir() + "/autoload");
        if(!autoloadDir.exists())
            autoloadDir.mkdir();
        int ret = 0;
        if(plp != null && plp.g())
            ret = javax.swing.JOptionPane.showConfirmDialog(plp.g_dev,
                          "Attempt to download " + URL +
                          " and cache it in user's directory?",
                          "Download JAR Module",
                          javax.swing.JOptionPane.YES_NO_OPTION);
        if(plp == null || !plp.g() ||
                ret == javax.swing.JOptionPane.YES_OPTION) {
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
                if(!DynamicModuleFramework.checkForManifest(
                        PLPToolbox.getConfDir() + "/autoload/" + fileName)) {
                    Msg.E("Downloaded JAR file does not contain plp.manifest",
                          Constants.PLP_DMOD_NO_MANIFEST_FOUND, null);
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
                    DynamicModuleFramework.applyManifestEntries(
                            PLPToolbox.getConfDir() + "/autoload/" + fileName,
                            manifest, plp);
            }
            return true;
        }
        return false;
    }

    /**
     * Read a line of string from standard input
     *
     * @return A line of string if successful, null otherwise
     */
    public static String readLine() {
        try {
            java.io.BufferedReader stdin = new java.io.BufferedReader(new java.io.InputStreamReader(System.in));
            return stdin.readLine();
        } catch (Exception e) {
            return null;
        }
    }

    /**
     * Copy a file from source to destination
     *
     * @param src Path to source file
     * @param dest Path to destination file
     * @return PLP_OK on successful copy, error code otherwise
     */
    public static int copyFile(String src, String dest) {
        File s = new File(src);
        File d = new File(dest);

        try {
            FileInputStream in = new FileInputStream(s);
            FileOutputStream out = new FileOutputStream(d);

            byte[] buf = new byte[Constants.DEFAULT_IO_BUFFER_SIZE];

            int readBytes;
            while((readBytes = in.read(buf)) != -1) {
                out.write(buf, 0, readBytes);
            }
            out.close();
            in.close();
            
        } catch(IOException e) {
            return Msg.E("File copy error: '" + src + "' to '" + dest +
                    "'." + (Constants.debugLevel >= 2 ? "Exception: " + e : "")
                    , Constants.PLP_GENERAL_IO_ERROR, null);
        }

        return Constants.PLP_OK;
    }

    /**
     * Write a new file filled with the provided data in string
     *
     * @param data Data to be written
     * @param file Destination file
     * @return PLP_OK on successful write, IO_WRITE_ERROR otherwise
     */
    public static int writeFile(String data, String file) {
        File d = new File(file);

        try {
            FileWriter out = new FileWriter(d);
            out.write(data);
            out.close();
        } catch(IOException e) {
            return Msg.E("File write error: '" + file +
                    "'." + (Constants.debugLevel >= 2 ? "Exception: " + e : "")
                    , Constants.PLP_IO_WRITE_ERROR, null);
        }

        return Constants.PLP_OK;
    }

    /**
     * Write a new file filled with the provided data in byte array
     *
     * @param data Data to be written
     * @param file Destination file
     * @return PLP_OK on successful write, IO_WRITE_ERROR otherwise
     */
    public static int writeFile(byte[] data, String file) {
        File d = new File(file);

        try {
            FileOutputStream out = new FileOutputStream(d);
            out.write(data);
            out.close();

        } catch(IOException e) {
            return Msg.E("File write error: '" + file +
                    "'." + (Constants.debugLevel >= 2 ? "Exception: " + e : "")
                    , Constants.PLP_IO_WRITE_ERROR, null);
        }

        return Constants.PLP_OK;
    }

    /**
     * Read a file and return the contents as string.
     *
     * @param path Path to the file to read from
     * @return Contents of the file as string, or null if there was an error
     */
    public static String readFileAsString(String path) {
        String data = null;
        try {
            FileInputStream in = new FileInputStream(new File(path));
            BufferedReader r = new BufferedReader(new InputStreamReader(in));
            String line;

            while((line = r.readLine()) != null) {
                if(data == null)
                    data = "";
                data += line + "\n";
            }

            r.close();
            in.close();

        } catch(Exception e) {
            Msg.E("File open error: '" + path +
                    "'." + (Constants.debugLevel >= 2 ? "Exception: " + e : "")
                    , Constants.PLP_GENERAL_IO_ERROR, null);
            return null;
        }

        return data;
    }

    /**
     * Read a file and return it as a byte array
     *
     * @param path Path to the file to read from
     * @return Contents of the file as byte array, or null if there was an error
     */
    public static byte[] readFile(String path) {
        byte[] data = new byte[0];
        int readBytes;
        try {
            FileInputStream in = new FileInputStream(new File(path));
            byte[] buf = new byte[Constants.DEFAULT_IO_BUFFER_SIZE];
            byte[] copyBuf;

            while((readBytes = in.read(buf)) != -1) {
                copyBuf = new byte[data.length+readBytes];
                System.arraycopy(buf, 0, copyBuf, data.length, readBytes);
                System.arraycopy(data, 0, copyBuf, 0, data.length);
                data = copyBuf;
            }

            in.close();

        } catch(Exception e) {
            Msg.E("File open error: '" + path +
                    "'." + (Constants.debugLevel >= 2 ? "Exception: " + e : "")
                    , Constants.PLP_GENERAL_IO_ERROR, null);
            return null;
        }
        return data;
    }

    /**
     * Extract a file from a jar file
     *
     * @param jar Path to JAR file to extract the file from
     * @param entry Name of the file to extract
     * @param dest Destination path to write the file to
     * @return PLP_OK on successful copy, error code otherwise
     */
    public static int copyFromJar(String jar, String entry, String dest) {
        try {
            JarFile jarFile = new JarFile(jar);
            JarEntry jarEntry = jarFile.getJarEntry(entry);
            if(jarEntry == null)
                return Msg.E("copyFromJar: " +
                        "Can not find entry: '" + entry +
                        "' in '" + jar + "'.",
                    Constants.PLP_GENERAL_IO_ERROR, null);
            File destFile = new File(dest);
            if(destFile.exists())
                return Msg.E("copyFromJar: " + "'" + dest + "' exists.",
                        Constants.PLP_GENERAL_IO_ERROR, null);
            InputStream in = jarFile.getInputStream(jarEntry);
            FileOutputStream out = new FileOutputStream(dest);
            while(in.available() > 0)
                out.write(in.read());
            out.close();
            in.close();
            jarFile.close();

        } catch(IOException e) {
            return Msg.E("copyFromJar: " +
                    "Jar extract error: '" + entry + "' to '" + dest +
                    "' from '" + jar + "'." +
                    (Constants.debugLevel >= 2 ? "Exception: " + e : "")
                    , Constants.PLP_GENERAL_IO_ERROR, null);
        }

        return Constants.PLP_OK;
    }

    /**
     * Pack a directory into a JAR file
     *
     * @param jar JAR file to create
     * @param dirPath Directory to pack
     * @return PLP_OK on successful packing, error code otherwise
     */
    public static int createJar(String jar, String dirPath) {
        File dir = new File(dirPath);
        int ret;
        if(!dir.exists())
            return Msg.E("'" + dir.getAbsolutePath() + "' does not exist",
                    Constants.PLP_IO_FILE_DOES_NOT_EXIST, null);
        else if(!dir.isDirectory())
            return Msg.E("'" + dir.getAbsolutePath() + "' is not a directory",
                    Constants.PLP_IO_IS_NOT_A_DIRECTORY, null);

        try {
			Manifest m = new Manifest();
			m.getMainAttributes().put(Attributes.Name.MANIFEST_VERSION, "1.0");
            FileOutputStream fOut = new FileOutputStream(new File(jar));
            JarOutputStream out = new JarOutputStream(fOut, m);
            ret = addDirToJar(out, dir, dir);
            out.close();
            fOut.close();
            if(ret != Constants.PLP_OK)
                return ret;
        } catch(IOException e) {
            return Msg.E("I/O error while trying to archive '" +
                    dir.getAbsolutePath() + "' into '" + jar + "'",
                    Constants.PLP_GENERAL_IO_ERROR, null);
        }

        return Constants.PLP_OK;
    }        

    /**
     * Helper method for the createJar method. This method will recurse into
     * directories and add entries to the JAR file. Entries are formatted
     * with UNIX-style path separator (forward slashes) for compatibility
     * with PLPTool class loader
     *
     * @param out JAR output stream
     * @param dir Directory to recurse into
     * @param root Root directory
     * @return PLP_OK on successful operation, error code otherwise
     */
    private static int addDirToJar(JarOutputStream out, File dir, File root) {
        String entryPath;
        File[] files = dir.listFiles();
        FileInputStream in;
        byte buf[] = new byte[Constants.DEFAULT_IO_BUFFER_SIZE];
        int readBytes;
        int ret;
        if(files != null) {
            for(int i = 0; i < files.length; i++) {
                if(files[i].isDirectory()) {
                    if((ret = addDirToJar(out, files[i], root)) != Constants.PLP_OK)
                        return ret;
                } else {
                    try {
                        in = new FileInputStream(files[i]);
                        entryPath = files[i].getAbsolutePath().replace(root.getAbsolutePath(), "").substring(1);
                        if(File.separatorChar == '\\')
                            entryPath = entryPath.replace("\\", "/");
                        Msg.M("- Adding '" + entryPath + "'...");
                        JarEntry entry = new JarEntry(entryPath);
                        entry.setSize(files[i].length());
                        out.putNextEntry(entry);
                        while((readBytes = in.read(buf)) != -1)
                            out.write(buf, 0, readBytes);
                        out.flush();
                        out.closeEntry();
                    } catch(IOException e) {
                        return Msg.E("I/O error while adding '" +
                                files[i].getAbsolutePath() + "' to JAR archive.",
                                Constants.PLP_GENERAL_IO_ERROR, null);
                    }
                }
            }
        }
        return Constants.PLP_OK;
    }
    
    /**
     * Add an entry to a jar file
     * 
     * @param jar Jar file to add entry to (will be created if it doesn't exist)
     * @param entryPath Path of the new entry
     * @param data Data to be written
     * @return PLP_OK on successful operation, error code otherwise (I/O error)
     */
    public static int addToJar(String jar, String entryPath, byte[] data) {        
        JarOutputStream out;
        JarInputStream in;
        File tJar = null;
        JarEntry t;
        File fJar = new File(jar);
        byte buf[] = new byte[Constants.DEFAULT_IO_BUFFER_SIZE];
        int readBytes;
        Manifest m = null;

        if(fJar.exists()) {           
            // First we need to read the JAR file entirely, we cannot just add
            // entries (it will overwrite the file)
            try {
                in = new JarInputStream(new FileInputStream(fJar));
                m = in.getManifest();
				if(m == null) {
					m = new Manifest();
					m.getMainAttributes().put(Attributes.Name.MANIFEST_VERSION, "1.0");
				}
            } catch(IOException e) {
                Msg.trace(e);
                return Msg.E("Failed to open input stream (I/O error)",
                                    Constants.PLP_GENERAL_IO_ERROR, null);
            } catch(Exception e) {
                Msg.trace(e);
                return Msg.E("Failed to open input stream (general exception)",
                                    Constants.PLP_GENERAL_IO_ERROR, null);
            }

            try {
                tJar = new File (getTmpDir() + "/tmp.jar");
                out = new JarOutputStream(new FileOutputStream(tJar), m);

                while((t = in.getNextJarEntry()) != null) {
                    if(!t.getName().equals(entryPath)) {
                        out.putNextEntry(t);
                        while((readBytes = in.read(buf)) != -1)
                            out.write(buf, 0, readBytes);
                        out.flush();
                        out.closeEntry();
                    }
                }
                in.close();
            } catch(IOException e) {
                Msg.trace(e);
                return Msg.E("Failed to open output stream (I/O error)",
                                    Constants.PLP_GENERAL_IO_ERROR, null);
            }                        
        } else {
            try {
                out = new JarOutputStream(new FileOutputStream(fJar));
            } catch(IOException e) {
                Msg.trace(e);
                return Msg.E("Failed to open output stream (I/O error)",
                                    Constants.PLP_GENERAL_IO_ERROR, null);
            }
        }
                      
        try {
            JarEntry entry;
            entry = new JarEntry(entryPath);
            entry.setSize(data.length);
            out.putNextEntry(entry);
            out.write(data);
            out.flush();
            out.closeEntry();
            out.close();
            if(tJar != null) {
                fJar.delete();
                tJar.renameTo(fJar);
            }
        } catch(java.util.zip.ZipException e) {
            Msg.trace(e);
            return Msg.E("Zip Exception: " + e.getMessage(), Constants.PLP_GENERAL_IO_ERROR, null);
        } catch(IOException e) {
            Msg.trace(e);
            return Msg.E("Failed to add \"" + entryPath + "\" (I/O error)",
                                Constants.PLP_GENERAL_IO_ERROR, null);
        }

        try {
            (new File(getTmpDir() + "/tmp.jar")).delete();
        } catch(Exception e) {
            Msg.trace(e);
            return Msg.E("Failed to delete temporary JAR file",
                                Constants.PLP_GENERAL_IO_ERROR, null);
        }
        
        return Constants.PLP_OK;
    }

    /**
     * Check if ~/.plp/tmp exists and create it if it doesn't.
     */
    public static void checkCreateTempDirectory() {
        File temp = new File(getConfDir() + "/tmp");
        if(!temp.exists()) {
            temp.mkdir();
        } else if(temp.exists() && !temp.isDirectory()) {
            Msg.E("Temporary directory creation failed.",
                    Constants.PLP_GENERAL_IO_ERROR, null);
        }
    }

    /**
     * Convert a map to 2-dimensional array
     *
     * @param map Reference to the map object to convert
     * @return 2-dimensional array with keys in the first field and the matching
     * values in the second
     */
    public static Object[][] mapToArray(java.util.Map map) {
        Object[][] mapArray = new Object[map.size()][2];
        Object[] keys = map.keySet().toArray();
        for(int i = 0; i < mapArray.length; i++) {
            mapArray[i][0] = keys[i];
            mapArray[i][1] = map.get(keys[i]);
        }

        return mapArray;
    }

    /**
     * Execute an external program
     *
     * @param program Command to be executed by the Runtime class
     */
    public static void execute(final String program) {
        Msg.M("execute: '" + program + "'");
        Process p;
        try {
            p = Runtime.getRuntime().exec(program);
            p.waitFor();
        } catch(IOException ioe) {
            Msg.E("I/O error while attempting to execute '" + program + "'",
                    Constants.PLP_GENERAL_IO_ERROR, null);
        } catch(InterruptedException ie) {

        }
    }

    /**
     * Bring up the file save dialog
     *
     * @param startPath Starting path to browse from
     * @param filters Filters to be used with the dialog
     * @return The file object on successful browsing, null otherwise
     */
    public static File saveFileDialog(String startPath, javax.swing.filechooser.FileFilter...filters) {
        final javax.swing.JFileChooser fc = new javax.swing.JFileChooser();
        fc.setAcceptAllFileFilterUsed(false);
        if(filters.length == 1)
            fc.setFileFilter(filters[0]);
        else if(filters.length > 1) {
            for(int i = 0; i < filters.length; i++) {
                fc.addChoosableFileFilter(filters[i]);
            }
        } else
            fc.setAcceptAllFileFilterUsed(true);
        fc.setCurrentDirectory(new File(startPath));

        int retVal = fc.showSaveDialog(null);

        if(retVal == javax.swing.JFileChooser.APPROVE_OPTION)
            return fc.getSelectedFile();
        return null;
    }

    /**
     * Bring up the file open dialog
     *
     * @param startPath Starting path to browse from
     * @param filters Filters to be used with the dialog
     * @return The file object on successful browsing, null otherwise
     */
    public static File openFileDialog(String startPath, javax.swing.filechooser.FileFilter...filters) {
        final javax.swing.JFileChooser fc = new javax.swing.JFileChooser();
        fc.setAcceptAllFileFilterUsed(false);
        if(filters.length == 1)
            fc.setFileFilter(filters[0]);
        else if(filters.length > 1) {
            for(int i = 0; i < filters.length; i++) {
                fc.addChoosableFileFilter(filters[i]);
            }
        } else
            fc.setAcceptAllFileFilterUsed(true);
        fc.setCurrentDirectory(new File(startPath));

        int retVal = fc.showOpenDialog(null);

        if(retVal == javax.swing.JFileChooser.APPROVE_OPTION)
            return fc.getSelectedFile();
        return null;
    }

    /**
     * Create a filter for JFileChooser. Extensions are given in a comma-
     * delimited list with no preceding period. e.g. "txt,text" will create
     * a filter for files with .txt or .text extension
     *
     * @param extensions Comma-delimited list of extensions to filter for
     * @param description Description of the filter to be shown in the
     * filechooser
     * @return An instance of the requested FileFilter
     */
    public static javax.swing.filechooser.FileFilter createFileFilter(final String extensions, final String description) {
        javax.swing.filechooser.FileFilter filter = new javax.swing.filechooser.FileFilter() {

            @Override
            public boolean accept(File f) {
                if(f.isDirectory())
                    return true;

                String[] extensionTokens = extensions.split(",");
                for(int i = 0; i < extensionTokens.length; i++) {
                    if(f.getAbsolutePath().endsWith("." + extensionTokens[i]))
                        return true;
                }

                return false;
            }

            @Override
            public String getDescription() {
                return description;
            }
        };

        return filter;
    }

    /**
     * Consume an argument and return a string array without the consumed
     * argument
     *
     * @param args Input string array
     * @param index Index of element to gobble
     * @return String array without the gobbled element
     */
    public static String[] gobble(String[] args, int index) {
        String[] temp = new String[args.length-1];
        System.arraycopy(args, 0, temp, 0, index);
        System.arraycopy(args, index+1, temp, index, args.length-index-1);
        return temp;
    }

    /**
     * Takes a string representing a character enclosed by single quotes and
     * checks for escaped characters. If escaped character is detected, the
     * function will return the actual character code. It will only strip
     * the single quotes otherwise. For example, 'A' will be returned as
     * A as long, and '\n' will be returned as the newline character.
     *
     * @param str The string representing the character
     * @return Character code in long
     */
    public static long parseEscapeCharacter(String str) throws Exception {
        if (str.startsWith("'") && str.endsWith("'")) {
            if(str.length() == 3)
                return str.charAt(1);
            else if (str.length() == 4) {
                if(str.charAt(1) == '\\') {
                    switch(str.charAt(2)) {
                        case 'n':
                            return '\n';
                        case 'r':
                            return '\r';
                        case 't':
                            return '\t';
                        case '\\':
                            return '\\';
                        case '\"':
                            return '\"';
                        case '\'':
                            return '\'';
                        case '0':
                            return '\0';
                        default:
                            throw new Exception("Invalid escape character");
                    }
                } else
                    throw new Exception("Invalid character format");
            } else
                throw new Exception("Invalid character format");
        }
        return Constants.PLP_NUMBER_ERROR;
    }

    /**
     * NOT IMPLEMENTED YET
     * Transform a string with embedded escapes to a character array
     *
     * @param str String to transform
     * @param escapes Escaped characters
     * @return
     */
    public static char[] parseStringAsChars(String str, char[]... escapes) {
        char[] ret = new char[str.length()]; // result is at least length of str

        for(int i = 0; i < str.length(); i++) {

        }

        return null;
    }

    public static String parseStringReplaceEscapedChars(String str) throws Exception {
        String out = str;
        int j;

        // check for escaped characters
        for(j = 0; j < out.length(); j++) {
            if(out.charAt(j) == '\\' && j != out.length() - 1) {
                switch(out.charAt(j + 1)) {

                    // Linefeed (0xA)
                    case 'n':
                        out = new StringBuffer(out).replace(j, j + 2, "\n").toString();
                        break;

                    // Carriage return (0xD)
                    case 'r':
                        out = new StringBuffer(out).replace(j, j + 2, "\r").toString();
                        break;

                    // Tab
                    case 't':
                        out = new StringBuffer(out).replace(j, j + 2, "\t").toString();
                        break;

                    // Backslash
                    case '\\':
                        out = new StringBuffer(out).replace(j, j + 2, "\\").toString();
                        break;

                    // Double quotes
                    case '\"':
                        out = new StringBuffer(out).replace(j, j + 2, "\"").toString();

                    // Single quote
                    case '\'':
                        out = new StringBuffer(out).replace(j, j + 2, "\'").toString();

                    // Null
                    case '0':
                        out = new StringBuffer(out).replace(j, j + 2, "\0").toString();
                        break;

                    default:
                        throw new Exception("Invalid escape character encountered");
                }
            }
        }

        return out;
    }

    /**
     * Copy a string to the system clipboard
     *
     * @param str String to copy
     */
    public static void copy(String str) {
        try {
            java.awt.Toolkit.getDefaultToolkit().getSystemClipboard().setContents(
                    new java.awt.datatransfer.StringSelection(str), null);
        } catch(Exception e) {
            Msg.W("clipboard copy failed.", null);
        }
    }

    /**
     * Show an error dialog box
     *
     * @param parent Parent component for the dialog box
     * @param str String to display
     */
    public static void showErrorDialog(java.awt.Component parent, String str) {
        javax.swing.JOptionPane.showMessageDialog(parent, str, "Error",
                javax.swing.JOptionPane.ERROR_MESSAGE);
    }

    /**
     * Show a yes/no confirmation dialog
     *
     * @param parent Parent component for dialog box
     * @param str String to display
     * @param title Title to display
     * @return True if the user clicked YES, false otherwise
     */
    public static boolean showYesNoDialog(java.awt.Component parent, String str, String title) {
        int ret;
        ret = javax.swing.JOptionPane.showConfirmDialog(parent, str, title,
                javax.swing.JOptionPane.YES_NO_OPTION);
        switch(ret) {
            case javax.swing.JOptionPane.YES_OPTION:
                return true;
            default:
                return false;
        }
    }

    /**
     * Find a file in the specified directory
     *
     * @param dirStr Directory to search
     * @param fileStr File name to be searched
     * @param recurse Recurse into subdirectories
     * @param t Thread synchronization object. Useful for canceling the
     * search before it is completed via a thread and to display the current
     * path being searched
     * @return Absolute path of the file if found, null otherwise
     */
    public static String findFileInDirectory(String dirStr, String fileStr, boolean recurse, ThreadSync t) {
        String ret = null;
        File dir = new File(dirStr);
        ArrayList<File> dirs = new ArrayList<File>();
        t.setString(dirStr);
        if(!dir.isDirectory())
            return ret;
        File[] files = dir.listFiles();
        if(files == null)
            return ret;
        for(int i = 0; i < files.length && !t.isStopped(); i++) {
            if(files[i].getName().equals(fileStr))
                return files[i].getAbsolutePath();
            if(files[i].isDirectory())
                dirs.add(files[i]);
        }
        if(recurse) {
            for(int i = 0; i < dirs.size() && !t.isStopped(); i++) {
                ret = findFileInDirectory(dirs.get(i).getAbsolutePath(), fileStr, true, t);
                if(ret != null)
                    return ret;
            }
        }

        return ret;
    }

    public static boolean isFileReadable(String file) {
        File f = new File(file);
        return f.exists() && f.canRead();
    }

    /**
     * Return a formatted hyperlink to be used by the IDE output to locate
     * the source line (file:#, where # is the line number)
     *
     * @param fileName Filename of the source
     * @param lineNumber Line number of the source
     * @return Formatted hyperlink as string
     */
    public static String formatHyperLink(String fileName, int lineNumber) {
        return "<font color=blue><u><a href=\"" + fileName + "::" + lineNumber + "\">" +
                fileName + ":" + lineNumber + "</a></u></font>";
    }

    /**
     * A very hacky function to return a sorted list of &lt;String, Long&gt;
     * by comparing the Long value
     *
     * @param map HashMap that contains the key-value pairs
     * @return Object array that contains the sorted key-value list
     */
    public static Object[][] getSortedStringByLongValue(HashMap<String, Long> map) {
        List<StringLongEntry> entries = new ArrayList<StringLongEntry>();
        Object[] keys = map.keySet().toArray();
        for(int i = 0; i < keys.length; i++)
            entries.add(new StringLongEntry((String) keys[i], map.get(keys[i])));

        Collections.sort(entries, new Comparator<StringLongEntry>() {
            public int compare(StringLongEntry o1, StringLongEntry o2) {
                return (int) (o1.getLong() - o2.getLong());
            }
        });

        Object[][] ret = new Object[entries.size()][2];

        for(int i = 0; i < ret.length; i++) {
            ret[i][0] = entries.get(i).getString();
            ret[i][1] = entries.get(i).getLong();
        }

        return ret;
    }

    /**
     * Parse a config file and return a hashmap with key-value pairs. The
     * delimiter is "::"
     *
     * @param file File to parse
     * @return HashMap with configuration key-value pairs
     */
    public static HashMap<String, String> parseConfig(String file) {
        HashMap<String, String> config = new HashMap<String, String>();
        String[] tokens;

        String lines[] = readFileAsString(file).split("\\r?\\n");

        if(lines == null)
            return null;

        for(int i = 0; i < lines.length; i++) {
            
            tokens = lines[i].split("::", 2);
            if(tokens.length == 2) {
                Msg.D("config parse (" + file + "): " + tokens[0] + "::" + tokens[1], 3, null);
                config.put(tokens[0], tokens[1]);
            } else {
                Msg.D("config parse (" + file + "): " + tokens[0] + "::[null]", 3, null);
                config.put(tokens[0], null);
            }
        }
        return config;
    }

    /**
     * Write out a hashmap of configuration to a file
     *
     * @param config A HashMap with key-value config pairs
     * @param file File to write configuration to
     */
    public static void writeConfig(HashMap<String, String> config, String file) {
        String out = "";
        java.util.Map.Entry<String, String> e;
        java.util.Iterator<java.util.Map.Entry<String, String>> i = config.entrySet().iterator();
        while(i.hasNext()) {
            e = i.next();
            out += e.getKey() + "::" + e.getValue() + "\n";
        }
        writeFile(out, file);
    }

    /**
     * Get the command line arguments PLPTool was launched with
     *
     * @return Command line arguments as string
     */
    public static String getCommandLineArgs() {
        return System.getProperty("sun.java.command");
    }

    /**
     * Get the command line arguments passed to the JVM when PLPTool was
     * launched
     *
     * @return JVM command line arguments as string
     */
    public static String getJVMCommandLineArgs() {
        String ret = "";
        RuntimeMXBean bean = ManagementFactory.getRuntimeMXBean();
        List<String> aList = bean.getInputArguments();

        for (int i = 0; i < aList.size(); i++) {
            ret += aList.get(i) + (i == aList.size()-1 ? "" : " ");
        }

        return ret;
    }

    public static boolean deleteFile(String path) {
        File f = new File(path);
        return f.delete();
    }

     /**
      * By default File#delete fails for non-empty directories, it works like "rm".
      * We need something a little more brutual - this does the equivalent of "rm -r"
      *
      * http://stackoverflow.com/a/4026761
      * Paulitex
      *
      * @param path Root File Path
      * @return true iff the file and all sub files/directories have been removed
      * @throws FileNotFoundException
      */
    public static boolean deleteRecursive(File path) throws FileNotFoundException {
        if (!path.exists())
            throw new FileNotFoundException(path.getAbsolutePath());
        boolean ret = true;
        if (path.isDirectory()) {
            for (File f : path.listFiles()){
                ret = ret && deleteRecursive(f);
            }
        }
        return ret && path.delete();
    }

    /**
     * Return the effective address of addresses in (base+offset) or
     * (base-offset) format
     *
     * @param location base+offset address in String
     * @return resolved address in long, -1 if the address is failed to be
     * resolved
     */
    public static long resolveBaseOffset(PLPAsm asm, String location) {
        long ret = -1;
        StringTokenizer tokens = new StringTokenizer(location.replaceAll("\\s",""), "+-", true);
        int tokenCount = tokens.countTokens();
        if(!tokens.hasMoreTokens()) {
            return Msg.E("resolveBaseOffset: empty expression", Constants.PLP_NUMBER_ERROR, null);
        }
        String base = tokens.nextToken();        
        long addr = asm.resolveAddress(base);
        ret = (addr == -1) ? parseNum(base) : addr;
        Msg.D("resolveBaseOffset: base=" + base + " asmresolved=" + format32Hex(ret) + " tokens=" + tokenCount, 4, null);
        if(ret != -1 && tokenCount == 3) {
            String offsetOperator = tokens.nextToken();
            long offset = parseNum(tokens.nextToken());
            if(offset == -1) {
                return ret;
            }
            if(offsetOperator.equals("+")) {
                ret = ret + offset;
            } else if(offsetOperator.equals("-")) {
                ret = ret - offset;
            }
        } else if(ret == -1 || tokenCount != 1) {
            return Msg.E("resolveBaseOffset: invalid address expression", Constants.PLP_NUMBER_ERROR, null);
        }
        return ret;
    }

    static class StringLongEntry {
        private String s;
        private Long l;

        public StringLongEntry(String s, Long l) {
            this.s = s;
            this.l = l;
        }

        public String getString() {
            return s;
        }

        public Long getLong() {
            return l;
        }
    }
}

