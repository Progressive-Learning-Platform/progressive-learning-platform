/*
David Fritz

top level module for mips design

2.6.2010
*/

module top(clk,leds,rst,txd,rxd,switches,sseg_an,sseg_display,rgb,hs,vs);
	input clk;
	output [7:0] leds;
	input rst,rxd;
	output txd;
	input [7:0] switches;
	output [3:0] sseg_an;
	output [7:0] sseg_display;
	output [7:0] rgb;
	output hs, vs;
	
	wire [31:0] daddr, dout, din, iaddr, iin;
	wire drw;

	cpu cpu_t(rst, clk, daddr, dout, din, drw, iaddr, iin);
        arbiter arbiter_t(rst, clk, daddr, dout, din, drw, iaddr, iin, leds, txd, rxd, switches, sseg_an, sseg_display, rgb, hs, vs);
endmodule
	

