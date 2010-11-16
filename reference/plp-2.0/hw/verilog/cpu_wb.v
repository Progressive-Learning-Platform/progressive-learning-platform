/*

fritz

writeback phase

*/

module cpu_wb(mem_rfw, mem_wbsource, mem_alu_r, dmem_data, mem_rf_waddr, mem_jalra, rfw, wbdata, rf_waddr);
	input mem_rfw;
	input [1:0] mem_wbsource;
	input [31:0] mem_alu_r;
	input [31:0] dmem_data;
	input [4:0] mem_rf_waddr;
	input [31:0] mem_jalra;
	output rfw;
	output [31:0] wbdata;
	output [4:0] rf_waddr;

	assign rfw = mem_rfw;
	assign wbdata = 
		(mem_wbsource == 2'b00) ? mem_alu_r :
		(mem_wbsource == 2'b01) ? dmem_data :
		(mem_wbsource == 2'b10) ? mem_jalra :
		32'd0;
	assign rf_waddr = mem_rf_waddr;
endmodule
