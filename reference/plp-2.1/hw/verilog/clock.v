module clock(clk, cpu_stall, arbiter_c, cpu_c);
	input clk, cpu_stall;
	output arbiter_c, cpu_c;

	reg count = 0;

	assign arbiter_c = count;
	assign cpu_c = count & !cpu_stall;

	always @(posedge clk)
		count = ~count;
endmodule
