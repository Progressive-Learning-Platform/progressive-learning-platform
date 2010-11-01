/* 
David Fritz

cpuid module

2.23.2010

a simple cpuid module that is used to determine in software the board 
id and frequency.

*/

module mod_cpuid(de, daddr, dout);
	input de;
	input [31:0] daddr;
	output [31:0] dout;

	wire [31:0] eff_dout;
	
	parameter cpu_id = 32'hdeadbeef;
	parameter board_freq = 32'h017d7840;	/* 25 mhz */

	assign dout = de ? eff_dout : 32'h00000000;

	assign eff_dout = (daddr == 0) ? cpu_id :
			  (daddr == 4) ? board_freq : 0;
endmodule
