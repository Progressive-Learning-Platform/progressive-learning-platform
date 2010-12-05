/*

fritz

instruction fetch phase

*/

module cpu_if(rst, clk, imem_addr, p_pc, pc_j,
		pc_b, b_addr, j_addr, iin, p_inst, stall);
	input 		rst, clk;
	input 		pc_j;
	input 		pc_b;
	input 	[31:0] 	b_addr;
	input 	[31:0] 	j_addr;
	input 		stall;
	output reg [31:0] p_pc;
	output 	[31:0] 	imem_addr;
	input	[31:0]	iin;
	output reg [31:0] p_inst;

	reg [31:0] pc;
	wire [31:0] next_pc;

	/* flush logic (for branches and jumps) */
	wire flush = pc_b | pc_j;

	always @(posedge clk) begin
		if (rst) begin
			p_pc <= 0;
			pc <= 0;
			p_inst <= 0;
		end else if (stall && !flush && !rst) begin
			p_pc <= p_pc;
			pc <= pc;
			p_inst <= p_inst;
		end else if (flush && !rst && !stall) begin
			p_pc <= 0;
			p_inst <= 0;
			pc <= next_pc;
		end else begin
			p_pc <= pc;
			pc   <= next_pc;
			p_inst <= iin;
		end
		
		/* debug code, not synthesized by Xilinx */
		//$display("IF: PC: %x INST: %x", p_pc, p_inst);
	end		

	assign next_pc = 
		(pc_b) ? b_addr :
		(pc_j & !pc_b) ? j_addr :
		pc + 4;

	assign imem_addr = pc;
endmodule
