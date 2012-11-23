module debounce(clk, rst, rst_debounced);
	input clk;
	input rst;
	output rst_debounced;

	parameter FREQ = 1250000;

	reg [31:0] count;

	assign rst_debounced = count == FREQ;

	always @(posedge clk) begin
		if (rst && count == 0) begin
			count <= 1;
		end else if (count != 0 && count != FREQ) begin
			count <= count + 1;
		end else if (count == FREQ) begin
			count <= 0;
		end
	end
endmodule
