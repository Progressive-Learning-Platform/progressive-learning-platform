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
package plptool.extras.autosaver5;

import plptool.Constants;
import plptool.Msg;
import plptool.PLPToolbox;
import plptool.gui.ProjectDriver;
import plptool.gui.PLPToolApp;
import plptool.dmf.*;

import java.io.File;
import javax.swing.event.DocumentListener;
import javax.swing.event.DocumentEvent;
import javax.swing.JOptionPane;
import javax.swing.JFileChooser;

/**
 *
 * @author wira
 */
public class PLPToolConnector implements ModuleInterface5 {    
    private ProjectDriver plp;
    private ProjectDriver temporaryProject;
    private AutoSaveThread thread;
    private DevDocListener editorDocListener;
    private int autoSaveRateMilliseconds = 5000;
    
    @Override
    public int initialize(ProjectDriver plp) {
        this.plp = plp;
		if(!plp.g())
			return Constants.PLP_OK;
        Msg.D("Initializing autosaver...", 2, this);
        temporaryProject = new ProjectDriver(Constants.PLP_DEFAULT);
        editorDocListener = new DevDocListener();
        String interval = PLPToolApp.getAttributes().get("autosaver_interval");
        if(interval != null) {
            try {
                autoSaveRateMilliseconds = Integer.parseInt(interval);
                Msg.I("Autosave interval is set to " + interval + "ms", this);
            } catch(Exception e) {}
        }

        // Check if we have autosaved projects from a PLPTool session that
        // did not exit cleanly
        try {
            File confDir = new File(PLPToolbox.getConfDir());
            File[] list = confDir.listFiles();
            int ret;
            String temp;
            boolean keepTrying = true;
            for(int i = 0; i < list.length; i++) {
                if(list[i].getName().startsWith("plp.autosave.")) {
                    temp = list[i].getName().substring(13);
                    ret = JOptionPane.showConfirmDialog(plp.g_dev,
                            "'" + temp + "' was not saved properly in the " +
                            "last PLPTool session.\nWould you like to save " +
                            "a copy of the autosaved version? This " + 
                            "file will be lost otherwise.",
                            "Rescue Project File!", JOptionPane.YES_NO_OPTION);
                    if(ret == JOptionPane.YES_OPTION) {
                        while(keepTrying) {
                            JFileChooser fc = new JFileChooser();
                            ret = fc.showSaveDialog(plp.g_dev);
                            if(ret == JFileChooser.APPROVE_OPTION) {
                                if(PLPToolbox.copyFile(list[i].getAbsolutePath(),
                                        fc.getSelectedFile().getAbsolutePath()) == Constants.PLP_OK) {
                                    keepTrying = false;
                                    Msg.I("'" + temp + "' is saved to " +
                                            fc.getSelectedFile().getAbsolutePath(), this);
                                } else {
                                    Msg.E("Failed to save to " +
                                            fc.getSelectedFile().getAbsolutePath() +
                                            ". Try a different file, or cancel rescue.",
                                            Constants.PLP_GENERIC_ERROR, this);
                                }
                            } else {
                                keepTrying = false;
                            }
                        }
                    }
                }
            }
        } catch(Exception ex) {}

        CallbackRegistry.register(new AutoSaveCallback(), 
                CallbackRegistry.PROJECT_NEW,
                CallbackRegistry.PROJECT_OPEN_SUCCESSFUL,
                CallbackRegistry.EDITOR_TEXT_SET,
                CallbackRegistry.EXIT
            );
        
        return Constants.PLP_OK;
    }       

    @Override    
    public String getName() {
        return "Autosaver Module";
    }

    @Override    
    public String getDescription() {
        return "Autosaver module for PLPTool. Prevents users from losing " +
                "unsaved projects.";
    }

    @Override    
    public int[] getMinimumPLPToolVersion() {
        int[] ret = {5, 0};
        return ret;
    }

    @Override    
    public int[] getVersion() {
        int[] ver = {1, 0};
        return ver;
    }    
    
    @Override
    public String toString() {
        return "AutoSaver";
    }
    
    
    class AutoSaveCallback implements Callback {
        @Override
        public boolean callback(int num, Object param ) {
            switch(num) {
                case CallbackRegistry.PROJECT_OPEN_SUCCESSFUL:
                case CallbackRegistry.PROJECT_NEW:
                    Msg.D("Starting autosave thread...", 2, this);
                    if(num == CallbackRegistry.PROJECT_OPEN_SUCCESSFUL)
                        temporaryProject.plpfile =
                            new File(PLPToolbox.getConfDir() + "/plp.autosave." + ((File) param).getName());
                    else
                        temporaryProject.plpfile =
                            new File(PLPToolbox.getConfDir() + "/plp.autosave.unsaved");
                    temporaryProject.setArch(plp.getArch().getID());
                    if(thread != null)
                        thread.stopThread();                    
                    thread = new AutoSaveThread();
                    thread.start();
                    break;
                case CallbackRegistry.EDITOR_TEXT_SET:
                    plp.g_dev.getEditor().getDocument().addDocumentListener(editorDocListener);
                    break;
                case CallbackRegistry.EXIT:
                    Msg.I("Cleaning up.", this);
                    try {
                        File confDir = new File(PLPToolbox.getConfDir());
                        File[] list = confDir.listFiles();
                        for(int i = 0; i < list.length; i++) {
                            if(list[i].getName().startsWith("plp.autosave."))
                                list[i].delete();
                        }
                    } catch(Exception ex) {}
                    break;
            }
            
            return true;
        }
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

        @Override        
        public String toString() {
            return "AutoSaveThread";
        }
    }

    class DevDocListener implements DocumentListener {
        public DevDocListener() {

        }

        @Override        
        public void changedUpdate(DocumentEvent e) {
            if(thread != null)
                thread.setNeedSave();
        }

        @Override
        public void removeUpdate(DocumentEvent e) {
            if(thread != null)
                thread.setNeedSave();
        }

        @Override        
        public void insertUpdate(DocumentEvent e) {
            if(thread != null)
                thread.setNeedSave();
        }
    }
}
