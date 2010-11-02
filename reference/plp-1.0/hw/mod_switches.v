/* 
David Fritz

switches control module

2.23.2010
*/

/*
There are 4 switches on the Spartan3E board, and they are addressable as the last nibble of the returned word
*/

module mod_switches(de, dout, switches);
	input de;
	output [31:0] dout;
	input [3:0] switches;

	assign dout = de ? {28'h0000000,switches} : 32'h00000000;
endmodule
