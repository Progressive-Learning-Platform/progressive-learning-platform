module mod_gpi(de, dout, gpi);
	input de;
	output [31:0] dout;
	input [7:0] gpi;

	assign dout = (de) ? {24'b0,gpi} : 0;

endmodule
