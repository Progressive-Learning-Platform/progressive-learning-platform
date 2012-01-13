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
import java.awt.event.KeyEvent;
import java.awt.event.KeyAdapter;
import javax.swing.JFrame;
import javax.swing.JTextField;
import javax.swing.JOptionPane;
import javax.swing.JButton;
import javax.swing.event.DocumentListener;
import javax.swing.event.DocumentEvent;

import java.io.IOException;
import java.io.File;
import java.util.Locale;

import javax.sound.sampled.DataLine;
import javax.sound.sampled.TargetDataLine;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.AudioFileFormat;

public class LectureRecorder extends JFrame implements PLPGenericModule {
    private boolean init = false;
    private boolean record = false;
    private ProjectDriver plp = null;
    private ArrayList<ProjectEvent> events;

    private ArrayList<PLPAsmSource> snapshot_Asms;
    private int snapshot_OpenAsm;
    private DevDocListener editorDocListener;
    private LectureAudioRecorder audioRecorderThread;

    private JTextField in;
    private JButton ctrl;

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
                Msg.I("Taking snapshot of the project...", this);
                snapshot_Asms = new ArrayList<PLPAsmSource>();
                for(int i = 0; i < plp.getAsms().size(); i++) {
                    PLPAsmSource s = plp.getAsm(i);
                    snapshot_Asms.add(new PLPAsmSource(s.getAsmString(), s.getAsmFilePath(), i));
                    snapshot_OpenAsm = plp.getOpenAsm();
                }
                Msg.I("Recording project events.", this);
                editorDocListener = new DevDocListener(this);
                plp.g_dev.getEditor().getDocument().addDocumentListener(editorDocListener);
                record = true;
            } else if(param.equals("stop")) {
                if(record) {
                    plp.g_dev.getEditor().getDocument().removeDocumentListener(editorDocListener);
                    record = false;
                    Msg.I("Stopped recording.", this);
                } else
                    Msg.I("Not recording!", this);
            } else if(param.equals("replay") && init) {
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
                    (new LectureRunner(events, plp)).start();
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
            setSize(400, 100);
            setResizable(false);
            initComponents();
            init = true;
            return param;
	}

	if(param instanceof ProjectEvent && init) {
            ProjectEvent e = (ProjectEvent) param;
            int id = e.getIdentifier();
            Msg.D(e.getSystemTimestamp() + ":" + e.getIdentifier(), 2, this);
            if(record &&
               id != ProjectEvent.PROJECT_OPEN_ENTRY &&
               id != ProjectEvent.PROJECT_SAVE) {
                
                events.add(e);
            } else {
                try {
                    switch(id) {
                        case ProjectEvent.PROJECT_OPEN_ENTRY:
                            TarArchiveEntry tIn = (TarArchiveEntry) e.getParameters();
                            if(tIn.getName().equals("plp.lecturerecord")) {
                                Msg.I("Loading saved lecture record...", this);

                                return true;
                            }

                            break;

                        case ProjectEvent.PROJECT_SAVE:
                            Msg.I("Saving lecture record...", this);
                            TarArchiveOutputStream tOut = (TarArchiveOutputStream) e.getParameters();
                            TarArchiveEntry entry = new TarArchiveEntry("plp.lecturerecord");
                            String data = "";
                            ProjectEvent ev;

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
                }
            }
	}

	return null;
    }

    private void initComponents() {
        in = new JTextField();
        in.setSize(this.getWidth(), this.getHeight()/2);
        in.setLocation(0, 0);
        in.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent evt) {
                if(evt.getKeyCode() == KeyEvent.VK_ENTER) {
                    hook(in.getText());
                }
            }
        });

        ctrl = new JButton();
        ctrl.setSize(this.getWidth()-15, this.getHeight()/2-5);
        ctrl.setLocation(0, this.getHeight()/2+5);

        //this.add(ctrl);
        this.add(in);
    }

    @Override
    public String toString() {
        return "LectureRecorder";
    }
}

class LectureRunner extends Thread {
    private ProjectDriver plp;
    private ArrayList<ProjectEvent> events;
    private long startTime;

    public LectureRunner(ArrayList<ProjectEvent> events, ProjectDriver plp) {
        this.events = events;
        this.plp = plp;
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
            for(int i = 0; i < events.size(); i++) {
                e = events.get(i);
                diff = e.getSystemTimestamp() - curTime;
                Msg.D(i + "\t: " + diff + "ms", 2, this);
                Thread.sleep(diff);
                plp.replay(e);
                curTime = e.getSystemTimestamp();
            }
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

class LectureAudioRecorder extends Thread {
    private AudioFormat audioFormat;
    private TargetDataLine targetDataLine;
    private AudioInputStream audioInputStream;
    private File output;

    public LectureAudioRecorder() {
        output = new File(PLPToolbox.getConfDir() + "/lecture_temp_audio.wav");
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

    public void stopRecording() {
        targetDataLine.stop();
        targetDataLine.close();
    }

    @Override
    public String toString() {
        return "LectureAudioRecorder";
    }
}