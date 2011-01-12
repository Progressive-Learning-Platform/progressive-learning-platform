/* 
David Fritz

led control module

2.5.2010
*/

/*
There are 8 leds on the Spartan3E board, and they are addressable as the last byte of the returned word
*/

module mod_leds(clk, de, daddr, drw, din, dout, leds, rst);
	input clk,rst;
	input de;
	input [31:0] daddr;
	input drw;
	input [31:0] din;
	output [31:0] dout;

	output reg [7:0] leds;

	assign dout = de ? {24'h000000,leds} : 32'h00000000;

	/* all data bus activity is negative edge triggered */
	always @(negedge clk) begin
		if (drw && de && !rst)
			leds = din[7:0];
		else if (rst)
			leds = 8'hff;
	end
endmodule
