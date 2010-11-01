/* 
David Fritz

timer module

2.23.2010

a simple 32-bit timer. Timer increments on every edge. 

*/

module mod_timer(clk, de, drw, din, dout, rst);
	input clk,rst;
	input de;
	input drw;
	input [31:0] din;
	output [31:0] dout;

	reg [31:0] timer;

	assign dout = de ? timer : 32'h00000000;

	/* all data bus activity is negative edge triggered */
	always @(negedge clk) begin
		timer = timer + 1;

		if (drw && de && !rst) timer = din;
		else if (rst) timer = 0;
	end
endmodule
