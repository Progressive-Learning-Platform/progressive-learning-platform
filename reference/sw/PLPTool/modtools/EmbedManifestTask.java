/*
    Copyright 2013 PLP Contributors

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

import plptool.PLPToolbox;
import plptool.Msg;
import plptool.dmf.DynamicModuleFramework;
import org.apache.tools.ant.Task;

/**
 *
 * @author wira
 */
public class EmbedManifestTask extends Task {
    private String jar;
    private String title;
    private String authors;
    private String license;
    private String descr;
    private String version;

    public void setJar(String str) {
        jar = str;
    }   
    
    public void setTitle(String str) {
        title = str;    
    }   
    
    public void setAuthors(String str) {
        authors = str;
    }   
    
    public void setLicense(String str) {
        license = str;
    }   
    
    public void setDescr(String str) {
        descr = str;
    }

    public void setVersion(String str) {
        version = str;
    }

    @Override
    public void execute() {
        Msg.M("EmbedManifestTask: '" + jar + "'");
        String manifest = DynamicModuleFramework.generateManifest(jar, title, authors, license, descr, version);
        if(manifest != null) {
            Msg.M("EmbedManifestTask: Embedding plp.manifest");
            PLPToolbox.addToJar(jar, "plp.manifest", manifest.getBytes());
        }
    }
}
