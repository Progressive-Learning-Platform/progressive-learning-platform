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

package autosaver;

import plptool.Constants;
import plptool.Msg;
import plptool.PLPGenericModule;
import plptool.PLPToolbox;
import plptool.gui.ProjectDriver;
import plptool.gui.ProjectEvent;
import plptool.gui.PLPToolApp;

import java.io.File;
import javax.swing.event.DocumentListener;
import javax.swing.event.DocumentEvent;

/**
 *
 * @author wira
 */
public class PLPToolConnector implements PLPGenericModule {

    private ProjectDriver plp;
    private ProjectDriver temporaryProject;
    private AutoSaveThread thread;
    private DevDocListener editorDocListener;
    private int autoSaveRateMilliseconds = 5000;

    public String getVersion() { return "0.0.1"; }

    public Object hook(Object param) {
        if(param instanceof ProjectDriver) {
            Msg.D("Initializing autosaver...", 2, this);
            this.plp = (ProjectDriver) param;
            temporaryProject = new ProjectDriver(Constants.PLP_DEFAULT);
            editorDocListener = new DevDocListener();
            String interval = PLPToolApp.getAttributes().get("autosaver_interval");
            if(interval != null) {
                try {
                    autoSaveRateMilliseconds = Integer.parseInt(interval);
                    Msg.I("Autosave interval is set to " + interval + "ms", this);
                } catch(Exception e) {

                }
            }
        } else if(param instanceof ProjectEvent) {
            ProjectEvent e = (ProjectEvent) param;
            switch(e.getIdentifier()) {
                case ProjectEvent.NEW_PROJECT:
                case ProjectEvent.PROJECT_OPEN:
                    Msg.D("Starting autosave thread...", 2, this);
                    temporaryProject.plpfile =
                        new File(PLPToolbox.getConfDir() + "/plp.autosave." + plp.plpfile.getName());
                    temporaryProject.setArch(plp.getArch().getID());
                    if(thread != null)
                        thread.stopThread();                    
                    thread = new AutoSaveThread();
                    thread.start();
                    break;
                case ProjectEvent.EDITOR_TEXT_SET:
                    plp.g_dev.getEditor().getDocument().addDocumentListener(editorDocListener);
                    break;
                case ProjectEvent.EXIT:
                    File confDir = new File(PLPToolbox.getConfDir());
                    File[] list = confDir.listFiles();
                    for(int i = 0; i < list.length; i++) {
                        if(list[i].getName().startsWith("plp.autosave."))
                            list[i].delete();
                    }
                    break;
            }
        }

        return null;
    }

    public String toString() {
        return "AutoSaver";
    }

    class AutoSaveThread extends Thread {
        private boolean stop = false;
        private boolean needSave = true;

        @Override
        public void run() {
            boolean temp;
            while(!stop) {
                try {
                    Thread.sleep(autoSaveRateMilliseconds);
                } catch(Exception e) {
                    
                }
                if(needSave) {
                    Msg.D("Autosaving...", 2, this);
                    temp = Msg.silent;
                    temporaryProject.setAsms(plp.getAsms());
                    temporaryProject.getAsm(plp.getOpenAsm()).setAsmString(plp.g_dev.getEditorText());
                    Msg.silent = true;
                    temporaryProject.save();
                    Msg.silent = temp;
                    needSave = false;
                }
            }
        }

        public void stopThread() {
            stop = true;
        }

        public void setNeedSave() {
            needSave = true;
        }

        public String toString() {
            return "AutoSaveThread";
        }
    }

    class DevDocListener implements DocumentListener {
        public DevDocListener() {

        }

        public void changedUpdate(DocumentEvent e) {
            thread.setNeedSave();
        }

        public void removeUpdate(DocumentEvent e) {
            thread.setNeedSave();
        }

        public void insertUpdate(DocumentEvent e) {
            thread.setNeedSave();
        }
    }
}
