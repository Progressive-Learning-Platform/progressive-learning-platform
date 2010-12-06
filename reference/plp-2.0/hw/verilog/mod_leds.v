/* 
David Fritz

led control module

2.5.2010
*/

/*
There are 8 leds on the Spartan3E board, and they are addressable as the last byte of the returned word
*/
module mod_leds(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout, leds);
        input rst;
        input clk;
        input ie,de;
        input [31:0] iaddr, daddr;
        input drw;
        input [31:0] din;
        output [31:0] iout, dout;
	output reg [7:0] leds;

        /* by spec, the iout and dout signals must go hiZ when we're not using them */
        wire [31:0] idata, ddata;
        assign iout = ie ? idata : 32'hzzzzzzzz;
        assign dout = de ? ddata : 32'hzzzzzzzz;

	assign idata = 32'h00000000;
	assign ddata = {24'h000000,leds};

	/* all data bus activity is negative edge triggered */
	always @(negedge clk) begin
		if (drw && de && !rst) begin
			leds = din[7:0];
			$display("MOD_LEDS: %x", din[7:0]);
		end else if (rst) begin
			leds = 8'hff;
		end
	end
endmodule
