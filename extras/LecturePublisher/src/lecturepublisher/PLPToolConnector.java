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

package lecturepublisher;

import plptool.PLPGenericModule;
import plptool.Msg;
import plptool.gui.ProjectDriver;
import plptool.gui.ProjectEvent;
import plptool.Constants;
import plptool.PLPAsmSource;
import plptool.PLPToolbox;

import org.apache.commons.compress.archivers.tar.TarArchiveOutputStream;
import org.apache.commons.compress.archivers.tar.TarArchiveEntry;

import java.util.ArrayList;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.event.DocumentListener;
import javax.swing.event.DocumentEvent;

import java.io.*;

import javax.sound.sampled.DataLine;
import javax.sound.sampled.TargetDataLine;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.UnsupportedAudioFileException;
import javax.sound.sampled.SourceDataLine;

import org.xiph.speex.spi.*;

public class PLPToolConnector implements PLPGenericModule {
    private final int VIDEO_BUFFER_SIZE = 512000;

    private boolean init = false;
    private boolean record = false;
    private boolean audio = true;
    private boolean superimpose = false;
    private ProjectDriver plp = null;
    private ArrayList<ProjectEvent> events;

    private ArrayList<PLPAsmSource> snapshot_Asms;
    private int snapshot_OpenAsm;
    private String videoURL;
    private Controls controls;
    private DevDocListener editorDocListener;
    private AudioRecorder audioRecorderThread;
    private AudioPlayer audioPlayerThread;
    private Runner runnerThread;

    private JMenuItem menuDevShowFrame;

    private int recordDelaySeconds = 2;

    public String getVersion() { return "4.0-beta"; }

    public Object hook(Object param) {
        if(param instanceof String) {
            if(param.equals("show") && init)        controls.setVisible(true);
            else if(param.equals("help"))           return hookHelp();
            else if(param.equals("record"))         return hookRecord();
            else if(param.equals("stop"))           return hookStop();
            else if(param.equals("pause") && init)  return hookPause();
            else if(param.equals("replay") && init) return hookReplay();
            else if(param.equals("clear") && init)  return hookClear();

        } else if(param instanceof ProjectDriver)       return hookInit(param);
        else if(param instanceof ProjectEvent && init)  return hookEvent(param);

	return null;
    }

    public Object hookInit(Object param) {
        this.plp = (ProjectDriver) param;
        if(!plp.g()) {
            Msg.E("Lecture Publisher requires PLPTool GUI", Constants.PLP_GENERIC_ERROR, this);
            return null;
        }

        events = new ArrayList<ProjectEvent>();
        Msg.I("<em>Lecture Publisher</em> is ready &mdash; Use <b>Tools" +
                "</b>&rarr;<b>Show Lecture Publisher Window</b> to start!",
                null);
        init = true;

        File temp = new File(PLPToolbox.getConfDir() + "/lecture_temp_audio.wav");
        if(temp.exists()) temp.delete();

        controls = new Controls((ProjectDriver) param, this);

        menuDevShowFrame = new JMenuItem();
        menuDevShowFrame.setText("Show Lecture Publisher Window");
        menuDevShowFrame.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                hook("show");
            }
        });
        plp.g_dev.addToolsItem(menuDevShowFrame);

        return param;
    }

    public Object hookEvent(Object param) {
        ProjectEvent e = (ProjectEvent) param;
        int id = e.getIdentifier();
        Msg.D(e.getSystemTimestamp() + ":" + e.getIdentifier(), 3, this);
        if(record &&
           id != ProjectEvent.PROJECT_OPEN_ENTRY &&
           id != ProjectEvent.PROJECT_SAVE &&
           id != ProjectEvent.EDITOR_TEXT_SET &&
           id != ProjectEvent.NEW_PROJECT &&
           id != ProjectEvent.EXIT) {
            events.add(e);
        } else {
            try {
                switch(id) {

                    // TODO: clear videoURL if user creates a new project!

                    case ProjectEvent.THIRDPARTY_LICENSE:
                        Msg.M(getJSpeexLicense().replace(" ", "&nbsp;").replace("\n", "<br />"));

                        break;

                    case ProjectEvent.EDITOR_TEXT_SET:
                        plp.g_dev.getEditor().getDocument().addDocumentListener(editorDocListener);

                        break;

                    case ProjectEvent.PROJECT_OPEN:
                    case ProjectEvent.NEW_PROJECT:
                        events = new ArrayList<ProjectEvent>();
                        snapshot_Asms = new ArrayList<PLPAsmSource>();
                        videoURL = null;
                        break;

                    case ProjectEvent.PROJECT_OPEN_ENTRY:
                        int sLevel = 0;
                        String entryName = (String) ((Object[])e.getParameters())[0];
                        byte[] image = (byte[]) ((Object[])e.getParameters())[1];
                        File plpFile = (File) ((Object[])e.getParameters())[2];
                        if(entryName.equals("plp.lecturerecord")) {
                            Msg.I("Loading saved lecture record...", this);
                            String str = new String(image);
                            ProjectEvent ev;
                            String[] lines = str.split("\\r?\\n");
                            String[] tokens;
                            int evId;
                            long evSystemTimestamp;
                            long evTimestamp;
                            for(int i = 0; i < lines.length; i++) {
                                tokens = lines[i].split("::");
                                evId = Integer.parseInt(tokens[0]);
                                evSystemTimestamp = Long.parseLong(tokens[1]);
                                evTimestamp = Long.parseLong(tokens[2]);
                                ev = new ProjectEvent(
                                        evId, evTimestamp, null);
                                ev.setSystemTimestamp(evSystemTimestamp);
                                switch(ev.getIdentifier()) {
                                    case ProjectEvent.EDITOR_INSERT:
                                        Object[] eParamsInsert = {Integer.parseInt(tokens[3]), tokens[4].replaceAll("\\\\n", "\n")};
                                        ev.setParameters(eParamsInsert);
                                        break;
                                    case ProjectEvent.EDITOR_REMOVE:
                                        Object[] eParamsRemove = {Integer.parseInt(tokens[3]), Integer.parseInt(tokens[4])};
                                        ev.setParameters(eParamsRemove);
                                        break;
                                    case ProjectEvent.OPENASM_CHANGE:
                                        Integer eParamsOpenasmChange = Integer.parseInt(tokens[3]);
                                        ev.setParameters(eParamsOpenasmChange);
                                        break;
                                }
                                events.add(ev);
                            }
                            return true;
                        } else if(entryName.equals("plp.lecturerecord_openasm")) {
                            String str = new String(image);
                            snapshot_OpenAsm = Integer.parseInt(str);
                            return true;
                        } else if(entryName.startsWith("plp.lecturerecord_snapshot.")) {
                            String str = new String(image);
                            String fName = entryName.substring(27, entryName.length());
                            snapshot_Asms.add(new PLPAsmSource(str, fName, sLevel));
                            sLevel++;
                            return true;
                        } else if(entryName.equals("plp.lecturevideo")) {
                            videoURL = PLPToolbox.getConfDir() +
                                    "/plp.lecturevideo." + plpFile.getName();
                            FileOutputStream fo = new FileOutputStream(videoURL);
                            fo.write(image);
                            
                            Msg.I("<b>This project file has an embedded video file</b>. " +
                                  "This video will play when you replay the lecture.", this);
                            return true;
                        }

                        break;

                    case ProjectEvent.PROJECT_SAVE:
                        if(events == null || events.isEmpty())
                            return null;
                        TarArchiveOutputStream tOut = (TarArchiveOutputStream) e.getParameters();
                        TarArchiveEntry entry;
                        String data = "";
                        Msg.D("Saving lecture snapshot...", 2, this);

                        for(int i = 0; i < snapshot_Asms.size(); i++) {
                            entry = new TarArchiveEntry("plp.lecturerecord_snapshot." +
                                    snapshot_Asms.get(i).getAsmFilePath());
                            data = snapshot_Asms.get(i).getAsmString();
                            entry.setSize(data.length());
                            tOut.putArchiveEntry(entry);
                            tOut.write(data.getBytes());
                            tOut.flush();
                            tOut.closeArchiveEntry();
                        }

                        entry = new TarArchiveEntry("plp.lecturerecord_openasm");
                        data = "" + snapshot_OpenAsm;
                        entry.setSize(data.length());
                        tOut.putArchiveEntry(entry);
                        tOut.write(data.getBytes());
                        tOut.flush();
                        tOut.closeArchiveEntry();

                        Msg.D("Saving lecture record...", 2, this);
                        entry = new TarArchiveEntry("plp.lecturerecord");

                        ProjectEvent ev;
                        data = "";
                        for(int i = 0; i < events.size(); i++) {
                            ev = events.get(i);
                            data += ev.getIdentifier() + "::";
                            data += ev.getSystemTimestamp() + "::";
                            data += ev.getTimestamp() + "::";

                            switch(ev.getIdentifier()) {
                                case ProjectEvent.EDITOR_INSERT:
                                    data += (Integer)((Object[]) ev.getParameters())[0] + "::";
                                    String tStr = (String)((Object[]) ev.getParameters())[1];
                                    tStr = tStr.replaceAll("\\n", "\\\\n");
                                    data += tStr;
                                    break;
                                case ProjectEvent.EDITOR_REMOVE:
                                    data += (Integer)((Object[]) ev.getParameters())[0] + "::";
                                    data += (Integer)((Object[]) ev.getParameters())[1];
                                    break;
                                case ProjectEvent.OPENASM_CHANGE:
                                    data += (Integer)((Object[]) ev.getParameters())[0];
                            }

                            data += "\n";
                        }

                        entry.setSize(data.length());
                        tOut.putArchiveEntry(entry);
                        tOut.write(data.getBytes());
                        tOut.flush();
                        tOut.closeArchiveEntry();

                        if(videoURL != null) {
                            Msg.D("Embedding video...", 2, this);
                            long size = (new File(videoURL)).length();
                            entry = new TarArchiveEntry("plp.lecturevideo");
                            entry.setSize(size);
                            tOut.putArchiveEntry(entry);
                            FileInputStream fi = new FileInputStream(videoURL);
                            byte[] inData = new byte[VIDEO_BUFFER_SIZE];
                            int readSize;
                            while((readSize = fi.read(inData)) != -1) {
                                tOut.write(inData, 0, readSize);
                            }

                            tOut.flush();
                            tOut.closeArchiveEntry();
                        }

                        break;

                    case ProjectEvent.EXIT:
                        File tempVideo = new File(PLPToolbox.getConfDir() +
                                    "/plp.lecturevideo." + plp.plpfile.getName());
                        if(tempVideo.exists())
                            tempVideo.delete();
                }
            } catch(Exception ex) {
                Msg.E("Whoops!", Constants.PLP_GENERIC_ERROR, this);
                if(Constants.debugLevel >= 2)
                    ex.printStackTrace();
            }
        }

        return null;
    }

    public Object hookRecord() {
        if(plp.plpfile == null) {
            Msg.I("No project is open.", this);
            return null;
        }
        Msg.I("Taking snapshot of the project...", this);
        snapshot_Asms = new ArrayList<PLPAsmSource>();
        plp.refreshProjectView(true);
        for(int i = 0; i < plp.getAsms().size(); i++) {
            PLPAsmSource s = plp.getAsm(i);
            snapshot_Asms.add(new PLPAsmSource(s.getAsmString(), s.getAsmFilePath(), i));
            snapshot_OpenAsm = plp.getOpenAsm();
        }

        events.clear();
        Msg.I("<font color=red><b>Recording project events.</b></font>", this);
        events.add(new ProjectEvent(ProjectEvent.GENERIC, -1)); // start marker
        if(audio) {
            audioRecorderThread = new AudioRecorder(PLPToolbox.getConfDir() + "/lecture_temp_audio.wav");
            if(audioRecorderThread.isReady()) audioRecorderThread.start();
        }
        if(editorDocListener != null)
            plp.g_dev.getEditor().getDocument().removeDocumentListener(editorDocListener);
        editorDocListener = new DevDocListener(this);
        plp.g_dev.getEditor().getDocument().addDocumentListener(editorDocListener);
        record = true;
        return true;
    }

    public Object hookReplay() {
        int ret;
        if(events == null || events.isEmpty()) {
            Msg.I("No lecture to replay.", this);
            return null;
        }

        ret = JOptionPane.showConfirmDialog(plp.g_dev, "Replaying the lecture will revert the project state " +
                "to right before recording started. Would you like to continue?", "Lecture Replay",
                JOptionPane.YES_NO_OPTION);
        if(ret == JOptionPane.YES_OPTION) {
            ArrayList tempList = new ArrayList<PLPAsmSource>();
            for(int i = 0; i < snapshot_Asms.size(); i++) {
                PLPAsmSource s = snapshot_Asms.get(i);
                tempList.add(new PLPAsmSource(s.getAsmString(), s.getAsmFilePath(), i));
            }
            plp.setAsms(tempList);
            plp.setOpenAsm(snapshot_OpenAsm);
            plp.refreshProjectView(false);
            File audioFile = new File(PLPToolbox.getConfDir() + "/lecture_temp_audio.wav");
            if(!superimpose && audioFile.exists()) {
                audioRecorderThread = null;
                audioPlayerThread = new AudioPlayer(PLPToolbox.getConfDir() + "/lecture_temp_audio.wav");
            } else {
                audioPlayerThread = null;
                audioRecorderThread = new AudioRecorder(PLPToolbox.getConfDir() + "/lecture_temp_audio.wav");
            }
            
            runnerThread = new Runner(events, plp, audioPlayerThread, audioRecorderThread, videoURL, controls);
            runnerThread.start();
        }
        return true;
    }

    public Object hookStop() {
        if(record) {
            if(audio) audioRecorderThread.stopRecording();
            plp.g_dev.getEditor().getDocument().removeDocumentListener(editorDocListener);
            events.add(new ProjectEvent(ProjectEvent.GENERIC, -1)); // end marker
            record = false;
            Msg.I("<b>Stopped recording.</b>", this);
        } else if(runnerThread != null) {
            runnerThread.stopReplay();
            if(audioPlayerThread != null)
                audioPlayerThread.stopPlay();
            controls.updateComponents();
            controls.externalStop();
        } else
            Msg.I("Not recording!", this);

        return true;
    }

    public Object hookClear() {
        Msg.I("Replay events cleared", this);
        events = new ArrayList<ProjectEvent>();
        return true;
    }

    public Object hookPause() {
        return true;
    }

    public Object hookHelp() {
        return "This is BETA\nPlease read the manual!";
    }

    public void setVideoURL(String url) {
        videoURL = url;
    }

    public boolean hasEmbeddedVideo() {
        return videoURL != null;
    }

    public void setRecordPlaybackParams(boolean audio, boolean superimpose) {
        this.audio = audio;
        this.superimpose = superimpose;
    }

    public long getStartTime() {
        if(events == null || events.isEmpty() || events.size() == 1)
            return -1;

        return events.get(0).getSystemTimestamp();
    }

    public long getLectureLength() {
        if(events == null || events.isEmpty() || events.size() == 1)
            return -1;

        return events.get(events.size()-1).getSystemTimestamp() - events.get(0).getSystemTimestamp();
    }

    public void resetStates() {
        controls.setRecordState(false);
        controls.setPlaybackState(false);
    }

    public String getJSpeexLicense() {
        String ret = "" +
"\nLecture Publisher uses JSpeex 0.9.7 which has the following copyright notice:\n" +
" ******************************************************************************\n" +
" *                                                                            *\n" +
" * Copyright (c) 1999-2003 Wimba S.A., All Rights Reserved.                   *\n" +
" *                                                                            *\n" +
" * COPYRIGHT:                                                                 *\n" +
" *      This software is the property of Wimba S.A.                           *\n" +
" *      This software is redistributed under the Xiph.org variant of          *\n" +
" *      the BSD license.                                                      *\n" +
" *      Redistribution and use in source and binary forms, with or without    *\n" +
" *      modification, are permitted provided that the following conditions    *\n" +
" *      are met:                                                              *\n" +
" *      - Redistributions of source code must retain the above copyright      *\n" +
" *      notice, this list of conditions and the following disclaimer.         *\n" +
" *      - Redistributions in binary form must reproduce the above copyright   *\n" +
" *      notice, this list of conditions and the following disclaimer in the   *\n" +
" *      documentation and/or other materials provided with the distribution.  *\n" +
" *      - Neither the name of Wimba, the Xiph.org Foundation nor the names of *\n" +
" *      its contributors may be used to endorse or promote products derived   *\n" +
" *      from this software without specific prior written permission.         *\n" +
" *                                                                            *\n" +
" * WARRANTIES:                                                                *\n" +
" *      This software is made available by the authors in the hope            *\n" +
" *      that it will be useful, but without any warranty.                     *\n" +
" *      Wimba S.A. is not liable for any consequence related to the           *\n" +
" *      use of the provided software.                                         *\n" +
" *                                                                            *\n" +
" * Date: 22nd April 2003                                                      *\n" +
" *                                                                            *\n" +
" ****************************************************************************** \n" +
"\n" +
"   Copyright (C) 2002 Jean-Marc Valin\n" +
"\n" +
"   Redistribution and use in source and binary forms, with or without\n" +
"   modification, are permitted provided that the following conditions\n" +
"   are met:\n" +
"   \n" +
"   - Redistributions of source code must retain the above copyright\n" +
"   notice, this list of conditions and the following disclaimer.\n" +
"   \n" +
"   - Redistributions in binary form must reproduce the above copyright\n" +
"   notice, this list of conditions and the following disclaimer in the\n" +
"   documentation and/or other materials provided with the distribution.\n" +
"   \n" +
"   - Neither the name of the Xiph.org Foundation nor the names of its\n" +
"   contributors may be used to endorse or promote products derived from\n" +
"   this software without specific prior written permission.\n" +
"   \n" +
"   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n" +
"   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n" +
"   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR\n" +
"   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR\n" +
"   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,\n" +
"   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,\n" +
"   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR\n" +
"   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF\n" +
"   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING\n" +
"   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\n" +
"   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n" +
"\n" +
                "";
        
        return ret;
    }

    @Override
    public String toString() {
        return "LectureRecorder";
    }

    class AudioRecorder extends Thread {
        private AudioFormat audioFormat;
        private TargetDataLine targetDataLine;
        private AudioInputStream audioInputStream;
        private File output;
        private boolean ready = false;

        public AudioRecorder(String path) {
            output = new File(path);
            if(output.exists()) output.delete();
            audioFormat = new AudioFormat(
                    AudioFormat.Encoding.PCM_SIGNED,
                    44100.0F, 16, 2, 4, 44100.0F, false);
            DataLine.Info info = new DataLine.Info(TargetDataLine.class, audioFormat);
            targetDataLine = null;

            try
            {
                    targetDataLine = (TargetDataLine) AudioSystem.getLine(info);
                    targetDataLine.open(audioFormat);
            } catch (LineUnavailableException e) {
                    Msg.E("unable to get a recording line",
                            Constants.PLP_GENERIC_ERROR, this);
                    e.printStackTrace();
            }

            //AudioFileFormat.Type targetType = AudioFileFormat.Type.WAVE;
            //AudioFileFormat.Type targetType = SpeexFileFormatType.SPEEX;
            audioInputStream = new AudioInputStream(targetDataLine);
            ready = true;
        }

        @Override
        public void run() {
            targetDataLine.start();
            try {
                //SpeexAudioFileWriter speexWriter = new SpeexAudioFileWriter();
                //speexWriter.write(audioInputStream, SpeexFileFormatType.SPEEX,
                //        new File(PLPToolbox.getConfDir() + "/testspeex.spx"));
                //(new SpeexAudioFileWriter()).write(audioInputStream, SpeexFileFormatType.SPEEX, output);
                AudioSystem.write(audioInputStream, AudioFileFormat.Type.WAVE, output);
            } catch(IOException e) {
                Msg.W("I/O Error during audio recording.", this);
            } catch(Exception e) {
                Msg.W("General error during audio recording.", this);
            }
        }

        public boolean isReady() {
            return ready;
        }

        public void stopRecording() {
            targetDataLine.stop();
            targetDataLine.close();
            //JSpeexEnc enc = new JSpeexEnc();
        }

        @Override
        public String toString() {
            return "LectureAudioRecorder";
        }
    }

    class AudioPlayer extends Thread {
        File audioFile;
        private AudioInputStream in;
        private final int EXTERNAL_BUFFER_SIZE = 524288; // 128Kb
        private boolean stop;

        private SourceDataLine line;

        public AudioPlayer(String path) {
            audioFile = new File(path);

        }

        public void seekTo(long ms) {

        }

        @Override
        public void run() {
            if(!audioFile.exists()) {
                Msg.W("Audio '" + audioFile.getAbsolutePath() + "' not found!", this);
                return;
            }

            try {
                in = AudioSystem.getAudioInputStream(audioFile);
                //in = (new SpeexAudioFileReader()).getAudioInputStream(new File(PLPToolbox.getConfDir() + "/testspeex.spx"));
            } catch(UnsupportedAudioFileException uafe) {
                Msg.W("Audio format unsupported.", this);
            } catch(IOException ioe) {
                Msg.W("I/O error loading audio file.", this);
            }

            AudioFormat format = in.getFormat();
            line = null;
            DataLine.Info info = new DataLine.Info(SourceDataLine.class, format);

            try {
                line = (SourceDataLine) AudioSystem.getLine(info);
                line.open(format);
            } catch (LineUnavailableException e) {
                Msg.W("Unable to open playback audio line.", this);
                return;
            } catch (Exception e) {
                e.printStackTrace();
                return;
            }

            stop = false;
            line.start();

            int nBytesRead = 0;
            byte[] abData = new byte[EXTERNAL_BUFFER_SIZE];

            try {
                while (nBytesRead != -1 && !stop) {
                    nBytesRead = in.read(abData, 0, abData.length);
                    if (nBytesRead >= 0)
                        line.write(abData, 0, nBytesRead);
                }
            } catch (IOException e) {
                e.printStackTrace();
                return;
            } finally {
                line.drain();
                line.close();
            }
        }

        public void stopPlay() {
            stop = true;
        }

        @Override
        public String toString() {
            return "LectureAudioPlayer";
        }
    }

    class Runner extends Thread {
        private ProjectDriver plp;
        private ArrayList<ProjectEvent> events;
        private long startTime;
        private AudioPlayer audioPlayerThread;
        private AudioRecorder audioRecorderThread;
        private String videoURL;
        private Controls controls;
        private boolean stop;

        public Runner(ArrayList<ProjectEvent> events, ProjectDriver plp,
                AudioPlayer audioPlayerThread, AudioRecorder audioRecorderThread,
                String videoURL, Controls controls) {
            this.events = events;
            this.plp = plp;
            this.audioPlayerThread = audioPlayerThread;
            this.audioRecorderThread = audioRecorderThread;
            this.videoURL = videoURL;
            this.controls = controls;
            this.stop = false;
            if(events.size() >= 1)
                startTime = events.get(0).getSystemTimestamp();
        }

        @Override
        public void run() {
            try {
                ProjectEvent e;
                long curTime = startTime;
                long diff;
                if(videoURL != null) {
                    controls.initVideo(videoURL);
                    controls.startVideo();
                    controls.pauseVideo();
                }
                Msg.I("Replay will start in 2 seconds...", this);
                Thread.sleep(1000);
                Msg.I("Replay will start in 1 second...", this);
                Thread.sleep(1000);
                Msg.I("Replaying...", this);
                if(videoURL != null)
                    controls.playVideo();
                if(audioPlayerThread != null) audioPlayerThread.start();
                if(audioRecorderThread != null && audioRecorderThread.isReady())
                    audioRecorderThread.start();
                for(int i = 0; i < events.size() && !stop; i++) {
                    e = events.get(i);
                    diff = e.getSystemTimestamp() - curTime;
                    Msg.D(i + "\t: " + diff + "ms", 3, this);
                    Thread.sleep(diff);
                    plp.replay(e);
                    curTime = e.getSystemTimestamp();
                }
                if(audioRecorderThread != null)
                    audioRecorderThread.stopRecording();
                Msg.I("Replay done.", this);
                resetStates();
            } catch(Exception e) {
            }
        }

        public void stopReplay() {
            stop = true;
        }

        @Override
        public String toString() {
            return "LecturePublisher";
        }
    }

    class DevDocListener implements DocumentListener {
        private lecturepublisher.PLPToolConnector l;

        public DevDocListener(lecturepublisher.PLPToolConnector l) {
            this.l = l;
        }

        public void changedUpdate(DocumentEvent e) {
            l.hook(new ProjectEvent(ProjectEvent.EDITOR_CHANGE, -1, e));
        }

        public void removeUpdate(DocumentEvent e) {
            Object[] eParams = {new Integer(e.getOffset()), new Integer(e.getLength())};
            l.hook(new ProjectEvent(ProjectEvent.EDITOR_REMOVE, -1, eParams));
        }

        public void insertUpdate(DocumentEvent e) {
            String str = "";
            try {
                str = e.getDocument().getText(e.getOffset(), e.getLength());
            } catch(Exception ble) {

            }
            Object[] eParams = {new Integer(e.getOffset()), str};
            l.hook(new ProjectEvent(ProjectEvent.EDITOR_INSERT, -1, eParams));
        }
    }
}


