/*
    Copyright 2012 PLP Contributors

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

package lecturepublisher;

/**
 * Lecture Publisher auto install
 *
 * @author wira
 */
public class Main {
    private static final String moduleName = "Lecture Publisher";
    private static final String moduleFile = "LecturePublisher.jar";

    public static void main(String args[]) {
        java.io.File jar =
                new java.io.File((new java.io.File(".")).getAbsolutePath() +
                "/" + moduleFile);
        System.out.println("Looking for " + jar.getAbsolutePath());
        if(!jar.exists()) {
            javax.swing.JFileChooser fc = new javax.swing.JFileChooser();
            fc.setDialogTitle("Choose JAR Module to Install");
            fc.setCurrentDirectory(new java.io.File("."));
            if(fc.showOpenDialog(fc) == fc.APPROVE_OPTION) {
                jar = new java.io.File(fc.getSelectedFile().getAbsolutePath());
            } else
                System.exit(-1);
        }

        if(installModule("file:///" + jar))
            javax.swing.JOptionPane.showMessageDialog(null, moduleFile +
                    "will start when PLPTool launches.");
    }

    /**
     * PLPTool module installer routine
     *
     * @param URL URL to JAR file
     */
    public static boolean installModule(String URL) {

        java.io.File autoloadDir = new java.io.File(getConfDir() + "/autoload");
        if(!autoloadDir.exists())
            autoloadDir.mkdir();
        String fileName = "";
        try {
            java.net.URL jar = new java.net.URL(URL);
            java.nio.channels.ReadableByteChannel rbc = java.nio.channels.Channels.newChannel(jar.openStream());
            String[] tokens = jar.getFile().split("/");
            fileName = tokens[tokens.length-1];
            java.io.FileOutputStream fos = new java.io.FileOutputStream(
                    getConfDir() + "/autoload/" + fileName);
            fos.getChannel().transferFrom(rbc, 0, 1 << 24);
            fos.close();
        } catch(Exception e) {
            System.err.println("Failed to fetch " + URL + ".");
            return false;
        }
        return true;
    }

    /**
     * Get the configuration directory path
     *
     * @return Configuration directory path in String
     */
    public static String getConfDir() {
        java.io.File confDir = new java.io.File(System.getProperty("user.home") + "/.plp");
        if(!confDir.exists())
            confDir.mkdir();

        return confDir.getAbsolutePath();
    }
}
