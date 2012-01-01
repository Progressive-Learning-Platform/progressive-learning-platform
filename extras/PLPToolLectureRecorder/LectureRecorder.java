import plptool.PLPGenericModule;
import plptool.Msg;
import plptool.gui.ProjectDriver;
import plptool.gui.ProjectEvent;

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

    public Object hook(Object param) {
        if(param instanceof String) {
            if(param.equals("init")) {
		setTitle("PLPTool Lecture Recorder Module");
		setSize(400, 100);
                setResizable(false);
                initComponents();
            } else if(param.equals("show") && init) {
		this.setVisible(true);
            } else if(param.equals("hello")) {
		Msg.I("ready!", this);
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
            init = true;
            return param;
	}

	if(param instanceof ProjectEvent && init && record) {
            ProjectEvent e = (ProjectEvent) param;
            Msg.I(e.getSystemTimestamp() + ":" + e.getIdentifier(), this);
            events.add(e);
	}

	return null;
    }

    private void initComponents() {
        final JTextField in = new JTextField();
        in.setSize(this.getWidth(), this.getHeight());
        in.addKeyListener(new KeyAdapter() {
            @Override
            public void keyTyped(KeyEvent evt) {
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