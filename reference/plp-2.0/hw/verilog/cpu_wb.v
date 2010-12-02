/*

fritz

writeback phase

*/

module cpu_wb(mem_c_rfw, mem_c_wbsource, mem_alu_r, mem_dmem_in, mem_rf_waddr, mem_jalra, rfw, wdata, rf_waddr);
	input mem_c_rfw;
	input [1:0] mem_c_wbsource;
	input [31:0] mem_alu_r;
	input [31:0] mem_dmem_in;
	input [4:0] mem_rf_waddr;
	input [31:0] mem_jalra;
	output rfw;
	output [31:0] wdata;
	output [4:0] rf_waddr;

	assign rfw = mem_c_rfw;
	assign wdata = 
		(mem_c_wbsource == 2'b00) ? mem_alu_r :
		(mem_c_wbsource == 2'b01) ? mem_dmem_in :
		(mem_c_wbsource == 2'b10) ? mem_jalra :
		32'd0;
	assign rf_waddr = mem_rf_waddr;
endmodule
