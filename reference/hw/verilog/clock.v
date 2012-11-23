module clock(clk, c);
	input clk;
	output c;

	reg count = 0;

	assign c = count;

	always @(posedge clk)
		count = ~count;
endmodule
