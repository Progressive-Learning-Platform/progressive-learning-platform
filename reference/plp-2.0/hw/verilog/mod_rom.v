/*
David Fritz

ROM module, which uses an inferred ram

10.31.2010
*/

module mod_rom(clk, ie, de, iaddr, daddr, drw, din, iout, dout);
	input clk;
	input ie,de;
	input [31:0] iaddr, daddr;
	input drw;
	input [31:0] din;
	output [31:0] iout, dout;

	/* the block ram */
	wire [31:0] idata, ddata;

	/* by spec, the iout and dout signals must go hiZ when we're not using them */
	assign iout = ie ? idata : 32'h00000000;
	assign dout = de ? ddata : 32'h00000000;

	inferred_ram #(512) (clk,clk,iaddr[9:2],daddr[9:2],idata,ddata);
endmodule
