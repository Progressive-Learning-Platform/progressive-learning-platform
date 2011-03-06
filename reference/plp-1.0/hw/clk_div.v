module clk_div(gclk,clk_half);
	input gclk;
	output reg clk_half;
	
	always @(posedge gclk) begin
		clk_half = ~clk_half;
	end
	
endmodule
