package edu.asu.plp.tool.backend.plpisa.sim;

public class ProgramCounter
{
	private long data;
	private long nextData;
	
	public ProgramCounter(long data)
	{
		this.nextData = data;
	}
	
	public void write(long data)
	{
		this.nextData = data;
	}
	
	public long input()
	{
		return nextData;
	}
	
	public long evaluate()
	{
		return data;
	}
	
	public void clock()
	{
		data = nextData;
	}
	
	public void reset(long data)
	{
		this.data = -1;
		this.nextData = data;
	}
}
