/*
David Fritz

ROM module, which uses an inferred ram
*/

module mod_rom(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout);
	input rst;
	input clk;
	input ie,de;
	input [31:0] iaddr, daddr;
	input drw;
	input [31:0] din;
	output [31:0] iout, dout;

	/* by spec, the iout and dout signals must go hiZ when we're not using them */
	wire [31:0] idata, ddata;
	assign iout = ie ? idata : 32'hzzzzzzzz;
	assign dout = de ? ddata : 32'hzzzzzzzz;

        inferred_rom rom(~clk,~clk,1'b1,1'b1,iaddr[10:2],daddr[10:2],idata,ddata);
endmodule
