import plptool.PLPGenericModule;
import plptool.Msg;
import plptool.gui.ProjectDriver;
import plptool.gui.ProjectEvent;
import plptool.Constants;

import org.apache.commons.compress.archivers.tar.TarArchiveOutputStream;
import org.apache.commons.compress.archivers.tar.TarArchiveEntry;

import java.util.ArrayList;
import java.awt.event.KeyEvent;
import java.awt.event.KeyAdapter;
import javax.swing.JFrame;
import javax.swing.JTextField;

public class LectureRecorder extends JFrame implements PLPGenericModule {
    private boolean init = false;
    private boolean record = false;
    private ProjectDriver plp = null;
    private ArrayList<ProjectEvent> events;

    private JTextField in;

    public String getVersion() { return "4.0-beta"; }

    public Object hook(Object param) {
        if(param instanceof String) {
            if(param.equals("init")) {

            } else if(param.equals("show") && init) {
		this.setVisible(true);
            } else if(param.equals("help")) {
		return "This is BETA\nPlease read the manual!";
            } else if(param.equals("record")) {
                Msg.I("Recording project events.", this);
                record = true;
            } else if(param.equals("stop")) {
                if(record) {
                    Msg.I("Stopped recording.", this);
                    record = false;
                } else
                    Msg.I("Not recording!", this);
            } else if(param.equals("replay") && init) {
		(new LectureRunner(events, plp)).start();
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
                            Msg.I("Loading saved lecture record...", this);
                            TarArchiveEntry tIn = (TarArchiveEntry) e.getParameters();
                            if(tIn.getName().equals("plp.lecturerecord")) {

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
        in.setSize(this.getWidth(), this.getHeight());
        in.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent evt) {
                if(evt.getKeyCode() == KeyEvent.VK_ENTER) {
                    hook(in.getText());
                }
            }
        });
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
            for(int i = 0; i < events.size(); i++) {
                e = events.get(i);
                diff = e.getSystemTimestamp() - curTime;
                Msg.D(i + "\t: " + diff + "ms", 2, this);
                Thread.sleep(diff);
                plp.replay(e);
                curTime = e.getSystemTimestamp();
            }
        } catch(Exception e) {
        }
    }
}