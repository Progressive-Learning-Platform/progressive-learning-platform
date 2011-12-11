import plptool.PLPGenericModule;
import plptool.Msg;
import plptool.gui.ProjectDriver;
import plptool.gui.ProjectEvent;

import java.util.ArrayList;
import javax.swing.JFrame;

public class LectureRecorder extends JFrame implements PLPGenericModule {
	private boolean init = false;
	private ProjectDriver plp = null;
	private ArrayList<ProjectEvent> events;

	public Object hook(Object param) {
		if(param instanceof String) {
			if(param.equals("init")) {
				setTitle("PLPTool Lecture Recorder Module");
				setSize(400, 300);
                                setResizable(false);

			} else if(param.equals("show") && init) {
				this.setVisible(true);
			} else if(param.equals("hello")) {
				System.out.println("LectureRecorder: ready!");
			} else if(param.equals("replay") && init) {
				(new LectureRunner(events, plp)).start();
			} else if(param.equals("clear") && init) {
				System.out.println("Replay events cleared");
				events = new ArrayList<ProjectEvent>();
			}

		}

		if(param instanceof ProjectDriver) {
			this.plp = (ProjectDriver) param;
			events = new ArrayList<ProjectEvent>();
			System.out.println("LectureRecorder: " + plp + " attached.");			
			init = true;
			return param;
		}

		if(param instanceof ProjectEvent && init) {
			ProjectEvent e = (ProjectEvent) param;
			System.out.println("LectureRecorder: " + e.getSystemTimestamp() + ":" + e.getIdentifier());
			events.add(e);
		}

		return null;
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
				System.out.println(i + "\t: " + diff + "ms");
				Thread.sleep(diff);
				plp.replay(e);
				curTime = e.getSystemTimestamp();
			}
		} catch(Exception e) {

		}
	}
}