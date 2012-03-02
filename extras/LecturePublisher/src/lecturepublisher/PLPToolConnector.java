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

import java.awt.Color;
import plptool.PLPGenericModule;
import plptool.Msg;
import plptool.gui.ProjectDriver;
import plptool.gui.ProjectEvent;
import plptool.Constants;
import plptool.PLPAsmSource;
import plptool.PLPToolbox;
import plptool.gui.PLPToolApp;

import org.apache.commons.compress.archivers.tar.TarArchiveOutputStream;
import org.apache.commons.compress.archivers.tar.TarArchiveEntry;

import java.util.ArrayList;
import java.awt.Graphics;
import javax.swing.JComponent;
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

import org.xiph.libvorbis.*;
import org.xiph.libogg.*;

public class PLPToolConnector implements PLPGenericModule {
    private final int SAVE_BUFFER_SIZE = 512000;
    private final String TEMPORARY_AUDIO_FILE =
            PLPToolbox.getConfDir() + "/lecture_temp_audio.wav";

    private boolean init = false;
    private boolean record = false;
    private boolean audio = true;
    private boolean superimpose = false;
    private boolean hasAudioRecord = false;
    private ProjectDriver plp = null;
    private ArrayList<ProjectEvent> events;

    // Runtime options
    private boolean novideo = false;

    private ArrayList<PLPAsmSource> snapshot_Asms;
    private int snapshot_OpenAsm;
    private String videoURL;
    private Controls controls;
    private DevDocListener editorDocListener;
    private AudioRecorder audioRecorderThread;
    private Runner runnerThread;
    private PaintOverlay paintOverlay;

    private JMenuItem menuDevShowFrame;

    private int recordDelaySeconds = 2;

/***************** VORBIS ENCODER MEMBERS *************************************/
    static vorbisenc 			encoder;

    static ogg_stream_state 	os;	// take physical pages, weld into a logical stream of packets

    static ogg_page				og;	// one Ogg bitstream page.  Vorbis packets are inside
    static ogg_packet			op;	// one raw packet of data for decode

    static vorbis_info			vi;	// struct that stores all the static vorbis bitstream settings

    static vorbis_comment		vc;	// struct that stores all the user comments

    static vorbis_dsp_state		vd;	// central working state for the packet->PCM decoder
    static vorbis_block			vb;	// local working space for packet->PCM decode

    static int READ = 1024;
    static byte[] readbuffer = new byte[READ*4+44];

    static int page_count = 0;
    static int block_count = 0;
/******************************************************************************/

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
        deleteTemporaryAudioFile();
        controls = new Controls((ProjectDriver) param, this);

        menuDevShowFrame = new JMenuItem();
        menuDevShowFrame.setText("Show Lecture Publisher Window");
        menuDevShowFrame.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                hook("show");
            }
        });
        paintOverlay = new PaintOverlay(plp);
        //paintOverlay.attachToIDE();
        paintOverlay.repaint();
        plp.g_dev.addToolsItem(menuDevShowFrame);
        String opt = PLPToolApp.getAttributes().get("lecturepublisher_novideo");
        if(opt != null && opt.equals("true")) {
            novideo = true;
        }
        opt = PLPToolApp.getAttributes().get("lecturepublisher_showoninit");
        if(opt != null && opt.equals("true")) {
            hook("show");
        }

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
                    case ProjectEvent.THIRDPARTY_LICENSE:
                        return showThirdPartyLicense();

                    case ProjectEvent.EDITOR_TEXT_SET:
                        plp.g_dev.getEditor().getDocument().addDocumentListener(editorDocListener);
                        break;

                    case ProjectEvent.PROJECT_OPEN:
                    case ProjectEvent.NEW_PROJECT:
                        return handleNewProject(e);

                    case ProjectEvent.PROJECT_OPEN_ENTRY:
                        return handleOpenEntry(e);

                    case ProjectEvent.PROJECT_SAVE:
                        return handleProjectSave(e);

                    case ProjectEvent.EXIT:
                        handleExit(e);
                }
            } catch(Exception ex) {
                Msg.E("Whoops! Set debug level to >= 2 for stack trace",
                        Constants.PLP_GENERIC_ERROR, this);
                if(Constants.debugLevel >= 2)
                    ex.printStackTrace();
            }
        }

        return null;
    }

    public Object handleNewProject(ProjectEvent e) {
        events = new ArrayList<ProjectEvent>();
        snapshot_Asms = new ArrayList<PLPAsmSource>();
        videoURL = null;
        controls.discardVideo();
        deleteTemporaryAudioFile();
        hasAudioRecord = false;        
        return null;
    }

    public Object handleOpenEntry(ProjectEvent e) {
        String entryName = "";
        try {
            int sLevel = 0;
            entryName = (String) ((Object[])e.getParameters())[0];
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
            } else if(entryName.equals("plp.lectureaudio")) {
                FileOutputStream fo = new FileOutputStream(TEMPORARY_AUDIO_FILE + ".ogg");
                fo.write(image);
                fo.close();
                hasAudioRecord = true;

                Msg.I("<b>This project file has an embedded audio file</b>. " +
                      "This audio will play when you replay the lecture.", this);
                return true;
            }
        } catch(IOException ioe) {
            Msg.E("I/O exception while trying to process the file '" +
                  entryName + "'. Use debug level of at least 2 for stack trace.",
                  Constants.PLP_DMOD_FILE_IO_ERROR, this);
            if(Constants.debugLevel >= 2)
                ioe.printStackTrace();

            return null;
        }
        return null;
    }

    public Object handleProjectSave(ProjectEvent e) {
        if(events == null || events.isEmpty())
            return null;
        try {
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
                byte[] inData = new byte[SAVE_BUFFER_SIZE];
                int readSize;
                while((readSize = fi.read(inData)) != -1) {
                    tOut.write(inData, 0, readSize);
                }

                tOut.flush();
                tOut.closeArchiveEntry();
                fi.close();
            }

            if(hasAudioRecord) {
                Msg.D("Embedding audio...", 2, this);

                int readSize;
                byte[] inData = new byte[SAVE_BUFFER_SIZE];
                FileInputStream fi = new FileInputStream(TEMPORARY_AUDIO_FILE + ".ogg");
                entry = new TarArchiveEntry("plp.lectureaudio");
                long size = (new File(TEMPORARY_AUDIO_FILE + ".ogg")).length();
                Msg.D("Compressed audio size: " + size + " bytes.", 2, this);
                entry.setSize(size);
                tOut.putArchiveEntry(entry);
                while((readSize = fi.read(inData)) != -1) {
                    tOut.write(inData, 0, readSize);
                }
                tOut.flush();
                tOut.closeArchiveEntry();
                fi.close();
            }
            return true;
        } catch(IOException ioe) {
            Msg.E("I/O exception during project save. Use debug level of at " +
                  "least 2 for stack trace.", Constants.PLP_DMOD_FILE_IO_ERROR,
                  this);
            if(Constants.debugLevel >= 2)
                ioe.printStackTrace();

            return null;
        }
    }
    
    public boolean encodeAudio() {
        Msg.I("Encoding audio, standby.", this);
        boolean eos = false;

        vi = new vorbis_info();

        encoder = new vorbisenc();

        if ( !encoder.vorbis_encode_init_vbr( vi, 2, 44100, .3f ) ) {
            Msg.E("Failed to Initialize vorbisenc", Constants.PLP_GENERIC_ERROR, this);
            return false;
        }

        vc = new vorbis_comment();
        vc.vorbis_comment_add_tag("ENCODER", "Java Vorbis Encoder");

        vd = new vorbis_dsp_state();

        if ( !vd.vorbis_analysis_init( vi ) ) {
            Msg.E("Failed to Initialize vorbis_dsp_state", Constants.PLP_GENERIC_ERROR, this);
            return false;
        }

        vb = new vorbis_block( vd );

        java.util.Random generator = new java.util.Random();  // need to randomize seed
        os = new ogg_stream_state( generator.nextInt(256) );

        System.out.print("Writing header.");
        ogg_packet header = new ogg_packet();
        ogg_packet header_comm = new ogg_packet();
        ogg_packet header_code = new ogg_packet();

        vd.vorbis_analysis_headerout( vc, header, header_comm, header_code );

        os.ogg_stream_packetin( header); // automatically placed in its own page
        os.ogg_stream_packetin( header_comm );
        os.ogg_stream_packetin( header_code );

        og = new ogg_page();
        op = new ogg_packet();

        try {

            FileOutputStream fos = new FileOutputStream(TEMPORARY_AUDIO_FILE + ".ogg");
            while( !eos ) {
                if ( !os.ogg_stream_flush( og ) )
                        break;

                fos.write( og.header, 0, og.header_len );
                fos.write( og.body, 0, og.body_len );
                System.out.print( "." );
            }
            System.out.print("Done.\n");
            FileInputStream fin = new FileInputStream(TEMPORARY_AUDIO_FILE);

            System.out.print("Encoding.");
            while ( !eos ) {

                int i;
                int bytes = fin.read( readbuffer, 0, READ*4 ); // stereo hardwired here

                int break_count = 0;

                if ( bytes==0 ) {

                    // end of file.  this can be done implicitly in the mainline,
                    // but it's easier to see here in non-clever fashion.
                    // Tell the library we're at end of stream so that it can handle
                    // the last frame and mark end of stream in the output properly

                    vd.vorbis_analysis_wrote( 0 );

                } else {

                    // data to encode

                    // expose the buffer to submit data
                    float[][] buffer = vd.vorbis_analysis_buffer( READ );

                    // uninterleave samples
                    for ( i=0; i < bytes/4; i++ ) {
                            buffer[0][vd.pcm_current + i] = ( (readbuffer[i*4+1]<<8) | (0x00ff&(int)readbuffer[i*4]) ) / 32768.f;
                            buffer[1][vd.pcm_current + i] = ( (readbuffer[i*4+3]<<8) | (0x00ff&(int)readbuffer[i*4+2]) ) / 32768.f;
                    }

                    // tell the library how much we actually submitted
                    vd.vorbis_analysis_wrote( i );
                }

                // vorbis does some data preanalysis, then divvies up blocks for more involved
                // (potentially parallel) processing.  Get a single block for encoding now

                while ( vb.vorbis_analysis_blockout( vd ) ) {

                    // analysis, assume we want to use bitrate management

                    vb.vorbis_analysis( null );
                    vb.vorbis_bitrate_addblock();

                    while ( vd.vorbis_bitrate_flushpacket( op ) ) {

                        // weld the packet into the bitstream
                        os.ogg_stream_packetin( op );

                        // write out pages (if any)
                        while ( !eos ) {

                            if ( !os.ogg_stream_pageout( og ) ) {
                                    break_count++;
                                    break;
                            }

                            fos.write( og.header, 0, og.header_len );
                            fos.write( og.body, 0, og.body_len );

                            // this could be set above, but for illustrative purposes, I do
                            // it here (to show that vorbis does know where the stream ends)
                            if ( og.ogg_page_eos() > 0 )
                                    eos = true;
                        }
                    }
                }
                System.out.print( "." );
            }

            fin.close();
            fos.close();
            System.out.print( "Done.\n" );

        } catch (Exception e) { System.out.println( "\n" + e ); e.printStackTrace(System.out); }

        Msg.I("Audio encoding done.", this);
        return true;
    }

    public Object handleExit(ProjectEvent e) {
        Msg.D("Cleanup", 2, this);
        File tempVideo = new File(PLPToolbox.getConfDir() +
                    "/plp.lecturevideo." + plp.plpfile.getName());
        if(tempVideo.exists())
            tempVideo.delete();
        deleteTemporaryAudioFile();
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
            audioRecorderThread = new AudioRecorder(TEMPORARY_AUDIO_FILE);
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
            if(!superimpose) {
                audioRecorderThread = null;
            } else { // superimposing, do not play audio
                hasAudioRecord = false;
                audioRecorderThread = new AudioRecorder(TEMPORARY_AUDIO_FILE);
            }
            
            runnerThread = new Runner(events, plp,
                    audioRecorderThread, videoURL, controls);
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
            if(hasAudioRecord)
                controls.getAudioPlayer().doStop();
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

    public void deleteTemporaryAudioFile() {
        File temp = new File(TEMPORARY_AUDIO_FILE);
        if(temp.exists()) temp.delete();
        temp = new File(TEMPORARY_AUDIO_FILE + ".ogg");
        if(temp.exists()) temp.delete();
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
        controls.setVisible(true);
        controls.getAudioPlayer().doStop();
    }

    public String getTemporaryAudioFile() {
        return TEMPORARY_AUDIO_FILE;
    }

    public Object showThirdPartyLicense() {
        JOptionPane.showMessageDialog(plp.g_dev,
                ThirdPartyLicenseText.LICENSE_cortado,
                "Lecture Publisher uses Cortado!", JOptionPane.INFORMATION_MESSAGE);
        JOptionPane.showMessageDialog(plp.g_dev,
                ThirdPartyLicenseText.LICENSE_jheora,
                "Lecture Publisher also uses Jheora!", JOptionPane.INFORMATION_MESSAGE);
        JOptionPane.showMessageDialog(plp.g_dev,
                ThirdPartyLicenseText.LICENSE_jkate,
                "...JKate!", JOptionPane.INFORMATION_MESSAGE);
        JOptionPane.showMessageDialog(plp.g_dev,
                ThirdPartyLicenseText.LICENSE_jtiger,
                "...JTiger!", JOptionPane.INFORMATION_MESSAGE);
        JOptionPane.showMessageDialog(plp.g_dev,
                ThirdPartyLicenseText.LICENSE_vorbis_java,
                "...and finally vorbis-java!", JOptionPane.INFORMATION_MESSAGE);
        JOptionPane.showMessageDialog(plp.g_dev,
                "Go open-source.",
                "Message", JOptionPane.INFORMATION_MESSAGE);
        return null;
    }

    @Override
    public String toString() {
        return "LectureRecorder";
    }

    class AudioRecorder extends Thread {
        private AudioFormat audioFormat;
        private TargetDataLine targetDataLine;
        private AudioInputStream audioInputStream;
        //private OggSpeexWriter speexWriter;
        private File output;
        private boolean ready = false;
        private boolean done = false;

        public AudioRecorder(String path) {
            output = new File(path);
            if(output.exists()) output.delete();
            audioFormat = new AudioFormat(
                    AudioFormat.Encoding.PCM_SIGNED,
                    44100.0F, 16, 2, 4, 44100.0F, false);
            DataLine.Info info = new DataLine.Info(TargetDataLine.class, audioFormat);
            targetDataLine = null;

            try {
                targetDataLine = (TargetDataLine) AudioSystem.getLine(info);
                targetDataLine.open(audioFormat);
            } catch(LineUnavailableException e) {
                Msg.E("Unable to get a recording line",
                        Constants.PLP_GENERIC_ERROR, this);
                e.printStackTrace();
            } catch(Exception e) {
                Msg.W("General error during initialization.", this);
                if(Constants.debugLevel >= 2)
                    e.printStackTrace();
            }
            audioInputStream = new AudioInputStream(targetDataLine);
            ready = true;
        }

        @Override
        public void run() {
            targetDataLine.start();
            try {
                AudioSystem.write(audioInputStream, AudioFileFormat.Type.WAVE, output);
                //speexWriter.close();
            } catch(IOException e) {
                Msg.W("I/O Error during audio recording.", this);
            } catch(Exception e) {
                Msg.W("General error during audio recording.", this);
                if(Constants.debugLevel >= 2)
                    e.printStackTrace();
            }
            if(!encodeAudio()) {
                Msg.E("Failed to encode lecture audio with vorbis encoder",
                      Constants.PLP_GENERIC_ERROR, this);
            }
            done = true;
            hasAudioRecord = true;
        }

        public boolean isReady() {
            return ready;
        }

        public boolean isDone() {
            return done;
        }

        public void stopRecording() {
            targetDataLine.stop();
            targetDataLine.close();
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
                try {
                    in.close();
                } catch(IOException e) {
                    Msg.W("Unable to close temporary wave file.",
                          this);
                }
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
        private AudioRecorder audioRecorderThread;
        private String videoURL;
        private Controls controls;
        private boolean stop;

        public Runner(ArrayList<ProjectEvent> events, ProjectDriver plp,
                AudioRecorder audioRecorderThread,
                String videoURL, Controls controls) {
            this.events = events;
            this.plp = plp;
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
                if(videoURL != null && !novideo) {
                    controls.initVideo(videoURL);
                    controls.startVideo();
                    controls.pauseVideo();
                }
                if(hasAudioRecord) {
                    controls.initAudio();
                    controls.getAudioPlayer().start();
                    controls.getAudioPlayer().doPause();
                }
                controls.setVisible(false);
                for(int i = recordDelaySeconds; i > 0; i--) {
                    Msg.I("Replay will start in " + i + " second" +
                          (i != 1 ? "s" : "") + "...", this);
                    Thread.sleep(1000);
                }
                Msg.I("Replaying...", this);
                if(videoURL != null && !novideo)
                    controls.playVideo();
                if(hasAudioRecord)
                    controls.getAudioPlayer().doPlay();
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

    class PaintOverlay extends JComponent {
        private ProjectDriver plp;

        public PaintOverlay(ProjectDriver plp) {
            this.plp = plp;
        }

        public void attachToIDE() {
            plp.g_dev.addPaintSurfaceOverlay(this);
        }

        @Override
        public void paint(Graphics g) {
            g.setColor(Color.RED);
            g.drawLine(0, 0, this.getWidth(), this.getHeight());
        }
    }
}


