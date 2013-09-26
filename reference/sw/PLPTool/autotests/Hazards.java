import plptool.*;
import plptool.testsuite.*;
import plptool.gui.ProjectDriver;
import plptool.gui.PLPToolApp;

public class Hazards implements Tester {
	public void configure(java.awt.Robot r) { }
	
	public void run(ProjectDriver plp) {
		plp.open("../examples/chutes_and_hazards.plp", false);
		
		String t = PLPToolApp.getAttributes().get("testmessage");
		if(t != null)
			AutoTest.p("Message from command line option: " + t);
		
		if(plp.assemble() != Constants.PLP_OK)		fail();
		if(!plp.isAssembled()) 						fail();
		if(plp.simulate() != Constants.PLP_OK) 		fail();
		
		for(int i = 0; i < 5000; i++)
			plp.sim.step();
		long out = (Long) plp.sim.bus.read(0xf0200000L);
		if(out == 0xffL)
			pass();
		else
			fail();
	}
	
	private void fail() { AutoTest.p("FAIL"); System.exit(-1); }
	private void pass() { AutoTest.p("PASS"); System.exit(0); }
}