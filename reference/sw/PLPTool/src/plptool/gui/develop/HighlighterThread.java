package plptool.gui.develop;

import javax.swing.SwingUtilities;

import plptool.Config;
import plptool.Msg;
import plptool.gui.frames.Develop;

public class HighlighterThread extends Thread {
    private Develop g_dev;
    private boolean stop;
    private boolean scheduleHighlight;

    public HighlighterThread(Develop g_dev) {
        this.g_dev = g_dev;
        stop = false;
        scheduleHighlight = false;
    }

    @Override
    public void run() {
        while(!stop) {
            if(isScheduled()) {
                SwingUtilities.invokeLater(new Runnable() {
                    @Override
                    public void run() {
                        Msg.D("HighlighterThread: running schedule", 10, null);
                        if(Config.devSyntaxHighlighting) {
                            g_dev.syntaxHighlight();
                        }
                        Msg.D("HighlighterThread: done", 10, null);
                    }
                    });
                setScheduleHighlight(false);
            }
            try {
                Thread.sleep(Config.devHighlighterThreadRefreshMsecs);
            } catch(Exception e) {
                
            }
        }
    }

    public synchronized void setScheduleHighlight(boolean val) {
        scheduleHighlight = val;
    }

    public synchronized boolean isScheduled() {
        return scheduleHighlight;
    }

    public void stopThread() {
        scheduleHighlight = false;
        stop = true;
    }
}
