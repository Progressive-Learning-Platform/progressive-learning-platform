/* 
David Fritz

timer module

a simple 32-bit timer. Timer increments on every edge. 

*/
module mod_timer(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout);
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

	reg [31:0] timer;

	assign ddata = timer;

	/* all data bus activity is negative edge triggered */
	always @(negedge clk) begin
		timer = timer + 1;

		if (drw && de && !rst) timer = din;
		else if (rst) timer = 0;
	end
endmodule
