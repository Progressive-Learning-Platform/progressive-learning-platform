/*
    Copyright 2010 David Fritz, Brian Gordon, Wira Mulia

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */



/*

David Fritz

pipelined mips machine

*/

module cpu(rst, clk, cpu_stall, daddr, dout, din, drw, iaddr, iin, int, int_ack);
	input clk, rst, cpu_stall;
	output [31:0] daddr;
	output [31:0] dout;
	input [31:0] din;
	output [1:0] drw;
	output [31:0] iaddr;
	input [31:0] iin;
	input int;
	output int_ack;

	wire 	[31:0] 	ifid_pc;
	wire	[31:0]	ifid_inst;

	wire	[31:0]	idex_rfa;
	wire	[31:0]	idex_rfb;
	wire	[31:0]	idex_se;
	wire	[4:0]	idex_shamt;
	wire	[5:0]	idex_func;
	wire	[4:0]	idex_rf_waddr;
	wire		idex_c_rfw;
	wire	[1:0]	idex_c_wbsource;
	wire	[1:0]	idex_c_drw;
	wire	[5:0]	idex_c_alucontrol;
	wire		idex_c_j;
	wire		idex_c_b;
	wire		idex_c_jjr;
	wire	[25:0]	idex_jaddr;
	wire	[31:0]	idex_pc;
	wire		idex_c_rfbse;
	wire	[4:0]	idex_rs;
	wire	[4:0]	idex_rt;
	wire		idif_stall;

	wire		exmem_c_rfw;
	wire	[1:0]	exmem_c_wbsource;
	wire	[1:0]	exmem_c_drw;
	wire	[31:0]	exmem_alu_r;
	wire	[31:0]	exmem_rfb;
	wire	[4:0]	exmem_rf_waddr;
	wire	[31:0]	exmem_jalra;
	wire	[4:0]	exmem_rt;
	wire	[31:0]	exif_baddr;
	wire	[31:0]	exif_jaddr;
	wire		exif_b;
	wire		exif_j;

	wire		memwb_c_rfw;
	wire	[1:0]	memwb_c_wbsource;
	wire	[31:0]	memwb_alu_r;
	wire	[4:0]	memwb_rf_waddr;
	wire	[31:0]	memwb_jalra;
	wire	[31:0]	memwb_dout;

	wire		wbid_rfw;
	wire	[31:0]	wbid_wdata;
	wire	[4:0]	wbid_waddr;	

	wire	[31:0]	int_pc;
	wire		int_flush;

	cpu_if  stage_if (rst, clk, cpu_stall, iaddr, ifid_pc, exif_j,
			exif_b, exif_baddr, exif_jaddr, iin, ifid_inst, idif_stall, int, int_ack, int_flush, int_pc);
	cpu_id  stage_id (rst, clk, cpu_stall, ifid_pc, ifid_inst, wbid_rfw,
			wbid_waddr, wbid_wdata, idex_rfa, idex_rfb, idex_se,
			idex_shamt, idex_func, idex_rf_waddr, idex_c_rfw, idex_c_wbsource,
			idex_c_drw, idex_c_alucontrol, idex_c_j, idex_c_b, idex_c_jjr,
			idex_jaddr, idex_pc, idex_c_rfbse, idex_rs, idex_rt,
			idif_stall, int_flush);
	cpu_ex  stage_ex (rst, clk, cpu_stall, idex_c_rfw, idex_c_wbsource, idex_c_drw,
			idex_c_alucontrol, idex_c_j, idex_c_b, idex_c_jjr, idex_rfa,
			idex_rfb, idex_se, idex_shamt, idex_func, idex_rf_waddr,
			idex_pc, idex_jaddr, idex_c_rfbse, idex_rs, idex_rt, 
			wbid_wdata, wbid_rfw, wbid_waddr, exmem_c_rfw, exmem_c_wbsource, 
			exmem_c_drw, exmem_alu_r, exmem_rfb, exmem_rf_waddr, exmem_jalra, 
			exmem_rt, exif_baddr, exif_jaddr, exif_b, exif_j, int_flush, int_pc);
	cpu_mem stage_mem (rst, clk, cpu_stall, exmem_c_rfw, exmem_c_wbsource, exmem_c_drw,
			exmem_alu_r, exmem_rfb, exmem_rf_waddr, exmem_jalra, exmem_rt,
			wbid_wdata, memwb_c_rfw, memwb_c_wbsource, memwb_alu_r, dout, 
			memwb_rf_waddr, memwb_jalra, daddr, drw, din, 
			memwb_dout);
	cpu_wb  stage_wb (memwb_c_rfw, memwb_c_wbsource, memwb_alu_r, memwb_dout, memwb_rf_waddr,
			memwb_jalra, wbid_rfw, wbid_wdata, wbid_waddr);
endmodule 
