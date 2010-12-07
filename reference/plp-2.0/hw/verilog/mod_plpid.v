/* 
David Fritz

plpid module

a simple cpuid module that is used to determine in software the board 
id and frequency.

*/
module mod_plpid(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout);
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

	parameter cpu_id = 32'hdeadbeef;
	parameter board_freq = 32'h02faf080;	/* 50 mhz */

	assign ddata = (daddr == 0) ? cpu_id :
		       (daddr == 4) ? board_freq : 0;
endmodule
