/*
David Fritz

top level module for mips design

2.6.2010
*/

module top(clk,leds,rst,txd,rxd,switches);
	input clk;
	output [7:0] leds;
	input rst,rxd;
	output txd;
	input [7:0] switches;
	
	wire [31:0] daddr, dout, din, iaddr, iin;
	wire drw;

	cpu cpu_t(rst, clk, daddr, dout, din, drw, iaddr, iin);
        arbiter arbiter_t(rst, clk, daddr, dout, din, drw, iaddr, iin, leds, txd, rxd, switches);
endmodule
	

