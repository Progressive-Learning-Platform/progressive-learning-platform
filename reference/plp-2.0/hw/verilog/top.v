/*
David Fritz

top level module for mips design

2.6.2010
*/

module top(clk,leds,rst,txd,rxd,switches,rgb,hs,vs,gpi);
	input clk;
	output [7:0] leds;
	input rst,rxd;
	output txd;
	input [3:0] switches;
	output [2:0] rgb;
	output hs,vs;
	input [7:0] gpi;
	
	wire [31:0] daddr, dout, din, iaddr, iin;
	wire drw;

	/* clock signals */
	wire clk_half;
	clk_div cdiv(clk,clk_half);

	cpu cpu_t(clk_half, daddr, dout, din, drw, iaddr, iin, rst);
        fsb fsb_t(clk_half, daddr, dout, din, drw, iaddr, iin, leds, rst, txd, rxd, switches);
endmodule
	

