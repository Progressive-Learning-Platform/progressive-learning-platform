/*
David Fritz

RAM module, which uses an inferred block ram

*/

module mod_ram(clk, ie, de, iaddr, daddr, drw, din, iout, dout);
	input clk;
	input ie,de;
	input [31:0] iaddr, daddr;
	input drw;
	input [31:0] din;
	output [31:0] iout, dout;

	/* the block ram */
	wire [31:0] idata, ddata;

	/* by spec, the iout and dout signals must go low when unused */
	assign iout = ie ? idata : 32'h00000000;
	assign dout = de ? ddata : 32'h00000000;

        inferred_ram ram(clk,~clk,1'b1,1'b1,(drw & de),iaddr[12:2],daddr[12:2],din,idata,ddata);
endmodule
