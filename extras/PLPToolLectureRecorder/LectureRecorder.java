import plptool.PLPGenericModule;
import plptool.Msg;
import plptool.gui.ProjectDriver;
import plptool.gui.ProjectEvent;
import plptool.Constants;
import plptool.PLPAsmSource;
import plptool.PLPToolbox;

import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;
import org.apache.commons.compress.archivers.tar.TarArchiveOutputStream;
import org.apache.commons.compress.archivers.tar.TarArchiveEntry;

import java.util.ArrayList;
import java.awt.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyAdapter;
import javax.swing.JFrame;
import javax.swing.JMenuItem;
import javax.swing.JTextField;
import javax.swing.JOptionPane;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.event.DocumentListener;
import javax.swing.event.DocumentEvent;

import java.io.IOException;
import java.io.File;

import javax.sound.sampled.DataLine;
import javax.sound.sampled.TargetDataLine;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.UnsupportedAudioFileException;
import javax.sound.sampled.SourceDataLine;

public class LectureRecorder extends JFrame implements PLPGenericModule {
    private boolean init = false;
    private boolean record = false;
    private boolean audio = true;
    private boolean superimpose = false;
    private ProjectDriver plp = null;
    private ArrayList<ProjectEvent> events;

    private ArrayList<PLPAsmSource> snapshot_Asms;
    private int snapshot_OpenAsm;
    private DevDocListener editorDocListener;
    private AudioRecorder audioRecorderThread;
    private AudioPlayer audioPlayerThread;

    private JTextField in;
    private JButton ctrlRecordAll;
    private JButton ctrlStopAll;
    private JButton ctrlReplayAll;
    private JButton ctrlRecordEventsWithoutAudio;
    private JButton ctrlSuperimpose;

    private JMenuItem menuDevShowFrame;

    private int recordDelaySeconds = 2;

    public String getVersion() { return "4.0-beta"; }

    public Object hook(Object param) {
        int ret;

        if(param instanceof String) {
            if(param.equals("init")) {

            } else if(param.equals("show") && init) {
		this.setVisible(true);

            } else if(param.equals("help")) {
		return "This is BETA\nPlease read the manual!";

            } else if(param.equals("record")) {
                if(plp.plpfile == null) {
                    Msg.I("No project is open.", this);
                    return null;
                }
                Msg.I("Taking snapshot of the project...", this);
                snapshot_Asms = new ArrayList<PLPAsmSource>();
                for(int i = 0; i < plp.getAsms().size(); i++) {
                    PLPAsmSource s = plp.getAsm(i);
                    snapshot_Asms.add(new PLPAsmSource(s.getAsmString(), s.getAsmFilePath(), i));
                    snapshot_OpenAsm = plp.getOpenAsm();
                }
                
                events.clear();
                Msg.I("Recording project events.", this);
                events.add(new ProjectEvent(ProjectEvent.GENERIC, -1)); // start marker
                if(audio) {
                    audioRecorderThread = new AudioRecorder(PLPToolbox.getConfDir() + "/lecture_temp_audio.wav");
                    if(audioRecorderThread.isReady()) audioRecorderThread.start();
                }
                editorDocListener = new DevDocListener(this);
                plp.g_dev.getEditor().getDocument().addDocumentListener(editorDocListener);
                record = true;

            } else if(param.equals("stop")) {
                if(record) {
                    if(audio) audioRecorderThread.stopRecording();
                    plp.g_dev.getEditor().getDocument().removeDocumentListener(editorDocListener);
                    record = false;
                    Msg.I("Stopped recording.", this);
                } else
                    Msg.I("Not recording!", this);
                
            } else if(param.equals("replay") && init) {
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
                        audioPlayerThread = new AudioPlayer(PLPToolbox.getConfDir() + "/lecture_temp_audio.wav");
                    } else {
                        audioPlayerThread = null;
                        audioRecorderThread = new AudioRecorder(PLPToolbox.getConfDir() + "/lecture_temp_audio.wav");
                    }
                    (new Runner(events, plp, audioPlayerThread, audioRecorderThread)).start();
                }
            } else if(param.equals("clear") && init) {
		Msg.I("Replay events cleared", this);
		events = new ArrayList<ProjectEvent>();
            }
        }

	if(param instanceof ProjectDriver) {
            this.plp = (ProjectDriver) param;
            events = new ArrayList<ProjectEvent>();
            Msg.I(plp + " attached.", this);
            setTitle("PLPTool Lecture Recorder Module");
            setSize(300, 200);
            setResizable(false);
            initComponents();
            init = true;

            File temp = new File(PLPToolbox.getConfDir() + "/lecture_temp_audio.wav");
            if(temp.exists()) temp.delete();

            menuDevShowFrame = new JMenuItem();
            menuDevShowFrame.setText("Show Lecture Recorder");
            menuDevShowFrame.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    hook("show");
                }
            });
            plp.g_dev.addToolsItem(menuDevShowFrame);
            
            return param;
	}

	if(param instanceof ProjectEvent && init) {
            ProjectEvent e = (ProjectEvent) param;
            int id = e.getIdentifier();
            Msg.D(e.getSystemTimestamp() + ":" + e.getIdentifier(), 2, this);
            if(record &&
               id != ProjectEvent.PROJECT_OPEN_ENTRY &&
               id != ProjectEvent.PROJECT_SAVE &&
               id != ProjectEvent.EDITOR_TEXT_SET ) {
                events.add(e);
            } else {
                try {
                    switch(id) {
                        case ProjectEvent.EDITOR_TEXT_SET:
                            plp.g_dev.getEditor().getDocument().addDocumentListener(editorDocListener);

                            break;

                        case ProjectEvent.PROJECT_OPEN_ENTRY:
                            String entryName = (String) ((Object[])e.getParameters())[0];
                            byte[] image = (byte[]) ((Object[])e.getParameters())[1];
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
                                            Object[] eParamsInsert = {Integer.parseInt(tokens[3]), tokens[4]};
                                            ev.setParameters(eParamsInsert);
                                            break;
                                        case ProjectEvent.EDITOR_REMOVE:
                                            Object[] eParamsRemove = {Integer.parseInt(tokens[3]), Integer.parseInt(tokens[4])};
                                            ev.setParameters(eParamsRemove);
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

                            }

                            break;

                        case ProjectEvent.PROJECT_SAVE:
                            if(events == null || events.isEmpty())
                                return null;
                            TarArchiveOutputStream tOut = (TarArchiveOutputStream) e.getParameters();
                            TarArchiveEntry entry;
                            String data = "";
                            Msg.I("Saving lecture snapshot...", this);

                            entry = new TarArchiveEntry("plp.lecturerecord_openasm");
                            data = "" + snapshot_OpenAsm;
                            entry.setSize(data.length());
                            tOut.putArchiveEntry(entry);
                            tOut.write(data.getBytes());
                            tOut.flush();
                            tOut.closeArchiveEntry();
                            
                            Msg.I("Saving lecture record...", this);
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
                                        data += (String)((Object[]) ev.getParameters())[1];
                                        break;
                                    case ProjectEvent.EDITOR_REMOVE:
                                        data += (Integer)((Object[]) ev.getParameters())[0] + "::";
                                        data += (Integer)((Object[]) ev.getParameters())[1];
                                        break;
                                }

                                data += "\n";
                            }

                            entry.setSize(data.length());
                            tOut.putArchiveEntry(entry);
                            tOut.write(data.getBytes());
                            tOut.flush();
                            tOut.closeArchiveEntry();
                            break;
                    }
                } catch(Exception ex) {
                    Msg.E("Whoops!", Constants.PLP_GENERIC_ERROR, this);
                    if(Constants.debugLevel >= 2)
                        ex.printStackTrace();
                }
            }
	}

	return null;
    }

    private void initComponents() {
        Container pane = this.getContentPane();
        JPanel row0 = new JPanel();
        JPanel row1 = new JPanel();
        JPanel row2 = new JPanel();
        pane.setLayout(new GridBagLayout());
        GridBagConstraints c = new GridBagConstraints();
        c.fill = GridBagConstraints.HORIZONTAL;
        c.gridx = 0;
        c.gridy = 0;
        pane.add(row0, c);
        c.gridx = 0;
        c.gridy = 1;
        pane.add(row1, c);
        c.gridx = 0;
        c.gridy = 2;
        pane.add(row2, c);

        row0.setLayout(new GridBagLayout());
        row1.setLayout(new GridBagLayout());

        in = new JTextField();
        c.weightx = 3;
        c.gridx = 0;
        c.gridy = 0;
        row0.add(in, c);

        in.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent evt) {
                if(evt.getKeyCode() == KeyEvent.VK_ENTER) {
                    hook(in.getText());
                }
            }
        });

        ctrlRecordAll = new JButton();
        ctrlRecordAll.setText("Record ALL");
        c.gridx = 0;
        c.gridy = 0;
        ctrlRecordAll.addActionListener(new java.awt.event.ActionListener() {
           public void actionPerformed(java.awt.event.ActionEvent evt) {
               audio = true;
               hook("record");
           }
        });
        row1.add(ctrlRecordAll, c);

        ctrlStopAll = new JButton();
        ctrlStopAll.setText("Stop ALL");
        c.gridx = 1;
        c.gridy = 0;
        ctrlStopAll.addActionListener(new java.awt.event.ActionListener() {
           public void actionPerformed(java.awt.event.ActionEvent evt) {
               hook("stop");
           }
        });
        row1.add(ctrlStopAll, c);

        ctrlReplayAll = new JButton();
        ctrlReplayAll.setText("Replay ALL");
        c.gridx = 2;
        c.gridy = 0;
        ctrlReplayAll.addActionListener(new java.awt.event.ActionListener() {
           public void actionPerformed(java.awt.event.ActionEvent evt) {
               superimpose = false;
               hook("replay");
           }
        });
        row1.add(ctrlReplayAll, c);

        ctrlRecordEventsWithoutAudio = new JButton();
        ctrlRecordEventsWithoutAudio.setText("Rec NOAUDIO");
        c.gridx = 0;
        c.gridy = 0;
        ctrlRecordEventsWithoutAudio.addActionListener(new java.awt.event.ActionListener() {
           public void actionPerformed(java.awt.event.ActionEvent evt) {
               audio = false;
               hook("record");
           }
        });
        row2.add(ctrlRecordEventsWithoutAudio, c);

        ctrlSuperimpose = new JButton();
        ctrlSuperimpose.setText("Superimpose");
        c.gridx = 1;
        c.gridy = 0;
        ctrlSuperimpose.addActionListener(new java.awt.event.ActionListener() {
           public void actionPerformed(java.awt.event.ActionEvent evt) {
               superimpose = true;
               hook("replay");
           }
        });
        row2.add(ctrlSuperimpose, c);
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

            AudioFileFormat.Type targetType = AudioFileFormat.Type.WAVE;
            audioInputStream = new AudioInputStream(targetDataLine);
            ready = true;
        }

        @Override
        public void run() {
            targetDataLine.start();
            try {
                AudioSystem.write(audioInputStream, AudioFileFormat.Type.WAVE, output);
            } catch(IOException e) {
                Msg.W("I/O Error during audio recording.", this);
            }
        }

        public boolean isReady() {
            return ready;
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
            SourceDataLine line = null;
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

            line.start();

            int nBytesRead = 0;
            byte[] abData = new byte[EXTERNAL_BUFFER_SIZE];

            try {
                while (nBytesRead != -1) {
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

        public Runner(ArrayList<ProjectEvent> events, ProjectDriver plp,
                AudioPlayer audioPlayerThread, AudioRecorder audioRecorderThread) {
            this.events = events;
            this.plp = plp;
            this.audioPlayerThread = audioPlayerThread;
            this.audioRecorderThread = audioRecorderThread;
            if(events.size() >= 1)
                startTime = events.get(0).getSystemTimestamp();
        }

        @Override
        public void run() {
            try {
                ProjectEvent e;
                long curTime = startTime;
                long diff;
                Msg.I("Replay will start in 2 seconds...", this);
                Thread.sleep(1000);
                Msg.I("Replay will start in 1 second...", this);
                Thread.sleep(1000);
                Msg.I("Replaying...", this);
                if(audioPlayerThread != null) audioPlayerThread.start();
                if(audioRecorderThread != null && audioRecorderThread.isReady())
                    audioRecorderThread.start();
                for(int i = 0; i < events.size(); i++) {
                    e = events.get(i);
                    diff = e.getSystemTimestamp() - curTime;
                    Msg.D(i + "\t: " + diff + "ms", 2, this);
                    Thread.sleep(diff);
                    plp.replay(e);
                    curTime = e.getSystemTimestamp();
                }
                if(audioRecorderThread != null)
                    audioRecorderThread.stopRecording();
                Msg.I("Replay done.", this);
            } catch(Exception e) {
            }
        }

        @Override
        public String toString() {
            return "LectureRunner";
        }
    }

    class DevDocListener implements DocumentListener {
        private LectureRecorder l;

        public DevDocListener(LectureRecorder l) {
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


