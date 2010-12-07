/* 
David Fritz

switches control module

2.23.2010
*/

/*
There are 4 switches on the Spartan3E board, and they are addressable as the last nibble of the returned word
*/
module mod_switches(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout, switches);
        input rst;
        input clk;
        input ie,de;
        input [31:0] iaddr, daddr;
        input drw;
        input [31:0] din;
        output [31:0] iout, dout;
	input [7:0] switches;

        /* by spec, the iout and dout signals must go hiZ when we're not using them */
        wire [31:0] idata, ddata;
        assign iout = ie ? idata : 32'hzzzzzzzz;
        assign dout = de ? ddata : 32'hzzzzzzzz;

	assign idata = 32'h00000000;
	assign ddata = {24'h0000000,switches};
endmodule
