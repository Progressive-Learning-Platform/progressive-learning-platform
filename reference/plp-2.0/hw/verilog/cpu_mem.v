/*

fritz

memory phase

*/

module cpu_mem(clk, ex_c_rfw, ex_c_wbsource, ex_c_drw, ex_alu_r, ex_rfb, ex_rf_waddr, ex_jalra, p_c_rfw, p_c_wbsource, p_alu_r, dmem_data, p_rf_waddr, p_jalra, dmem_addr, dmem_drw);
	input 		clk;
	input 		ex_c_rfw;
	input [1:0]	ex_c_wbsource;
	input 		ex_c_drw;
	input [31:0]	ex_alu_r;
	input [31:0]	ex_rfb;
	input [4:0]	ex_rf_waddr;
	input [31:0]	ex_jalra;
	output reg 	p_c_rfw;
	output reg [1:0] p_c_wbsource;
	output reg [31:0] p_alu_r;
	output [31:0] dmem_data;
	output reg [4:0] p_rf_waddr;
	output reg [31:0] p_jalra;
	output [31:0] dmem_addr;
	output dmem_drw;

	assign dmem_data = ex_rfb;
	assign dmem_addr = ex_alu_r;
	assign dmem_drw  = ex_c_drw;

	always @(posedge clk) begin
		p_c_rfw <= ex_c_rfw;
		p_c_wbsource <= ex_c_wbsource;
		p_alu_r <= ex_alu_r;
		p_rf_waddr <= ex_rf_waddr;
		p_jalra <= ex_jalra;
	end

endmodule
