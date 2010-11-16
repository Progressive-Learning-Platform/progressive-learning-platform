/*

fritz

instruction fetch phase

*/

module cpu_if(rst, clk, imem_addr, p_pc, pc_j, pc_b, b_addr, j_addr);
	input 		rst, clk;
	input 		pc_j;
	input 		pc_b;
	input 	[31:0] 	b_addr;
	input 	[31:0] 	j_addr;
	output reg [31:0] p_pc;
	output 	[31:0] 	imem_addr;

	reg [31:0] pc;
	wire [31:0] next_pc;

	always @(posedge clk) begin
		if (rst) begin
			p_pc <= 0;
			pc <= 0;
		end else begin
			p_pc <= pc;
			pc   <= next_pc;
		end
		$display("PC: %d",pc);
	end		

	assign next_pc = 
		(pc_b) ? b_addr :
		(pc_j & !pc_b) ? j_addr :
		pc + 4;

	assign imem_addr = pc;
endmodule
