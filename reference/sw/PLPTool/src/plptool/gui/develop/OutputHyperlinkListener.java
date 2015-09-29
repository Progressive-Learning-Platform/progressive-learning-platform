package plptool.gui.develop;

import static javax.swing.event.HyperlinkEvent.EventType.ACTIVATED;

import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;

import plptool.gui.ProjectDriver;

public class OutputHyperlinkListener implements HyperlinkListener
{
	private ProjectDriver plp;
	
	public OutputHyperlinkListener(ProjectDriver plp)
	{
		this.plp = plp;
	}
	
	@Override
	public void hyperlinkUpdate(HyperlinkEvent event)
	{
		if (event.getEventType().equals(ACTIVATED))
		{
			// XXX: find a way to remove the dependency on the format of the description
			String tokens[] = event.getDescription().split("::");
			int line = Integer.parseInt(tokens[1]);
			// TODO: this class does not need the plp instance - limit scope afap
			plp.g_dev.gotoLocation(tokens[0], line);
		}
	}
}
