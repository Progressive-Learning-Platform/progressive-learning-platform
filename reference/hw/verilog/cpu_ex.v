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

fritz

instruction execute phase

*/

module cpu_ex(rst, clk, cpu_stall, id_c_rfw, id_c_wbsource, id_c_drw,
		id_c_alucontrol, id_c_j, id_c_b, id_c_jjr, id_rfa,
		id_rfb, id_se, id_shamt, id_func, id_rf_waddr,
		id_pc, id_jaddr, id_c_rfbse, id_rs, id_rt,
		wb_wdata, wb_rfw, wb_waddr, p_c_rfw, p_c_wbsource,
		p_c_drw, p_alu_r, p_rfb, p_rf_waddr, p_jalra,
		p_rt, baddr, jaddr, c_b, c_j, int_flush, int_pc);
	input 		rst, clk, cpu_stall;
	input		id_c_rfw;
	input [1:0]	id_c_wbsource;
	input [1:0]	id_c_drw;
	input [5:0]	id_c_alucontrol;
	input 		id_c_j;
	input		id_c_b;
	input		id_c_jjr;
	input [31:0]	id_rfa;
	input [31:0]	id_rfb;
	input [31:0]	id_se;
	input [4:0]	id_shamt;
	input [5:0]	id_func;
	input [4:0]	id_rf_waddr;
	input [31:0]	id_pc;
	input [25:0]	id_jaddr;
	input 		id_c_rfbse;
	input [4:0]	id_rs;
	input [4:0]	id_rt;
	input [31:0]	wb_wdata;
	input 		wb_rfw;
	input [4:0]	wb_waddr;
	input 		int_flush;
	output reg	p_c_rfw;
	output reg [1:0] p_c_wbsource;
	output reg [1:0] p_c_drw;
	output reg [31:0] p_alu_r;
	output reg [31:0] p_rfb;
	output reg [4:0] p_rf_waddr;
	output reg [31:0] p_jalra;
	output reg [4:0] p_rt;
	output [31:0]	baddr;
	output [31:0]	jaddr;
	output c_b;
	output c_j;
	output [31:0]	int_pc;

	/* forward logic */
	wire [1:0] forwardX = (p_c_rfw & (p_rf_waddr == id_rs) & (p_rf_waddr != 0)) ? 2'b01 :
			      (wb_rfw & (wb_waddr == id_rs) & (wb_waddr != 0)) ? 2'b10 : 0;
	wire [1:0] forwardY = (p_c_rfw & (p_rf_waddr == id_rt) & (p_rf_waddr != 0)) ? 2'b01 :
			      (wb_rfw & (wb_waddr == id_rt) & (wb_waddr != 0)) ? 2'b10 : 0;

	wire [31:0] x = (forwardX == 2'b00) ? id_rfa :
			(forwardX == 2'b01) ? p_alu_r :
			(forwardX == 2'b10) ? wb_wdata : 0;
	wire [31:0] eff_y = (forwardY == 2'b00) ? id_rfb :
			(forwardY == 2'b01) ? p_alu_r :
			(forwardY == 2'b10) ? wb_wdata : 0;
	wire [31:0] y = (id_c_rfbse) ? id_se : eff_y;
	wire cmp_signed = (x[31] == y[31]) ? x < y : x[31];
	wire cmp_unsigned = x < y;

	/* multiply */
	wire [63:0] r_mul = {{32{x[31]}},x} * {{32{y[31]}},y};

	/* alu control */
	wire [5:0] alu_func = 
		(id_c_alucontrol == 6'h00) ? id_func :
		(id_c_alucontrol == 6'h08) ? 6'h21 :
		(id_c_alucontrol == 6'h09) ? 6'h21 :
		(id_c_alucontrol == 6'h0c) ? 6'h24 :
		(id_c_alucontrol == 6'h0d) ? 6'h25 :
		(id_c_alucontrol == 6'h0a) ? 6'h2a :
		(id_c_alucontrol == 6'h0b) ? 6'h2b :
		(id_c_alucontrol == 6'h23) ? 6'h21 :
		(id_c_alucontrol == 6'h2b) ? 6'h21 : 
		(id_c_alucontrol == 6'h0f) ? 6'h00 :
		(id_c_alucontrol == 6'h04) ? 6'h04 :    /* these two define the beq and bne logic */
		(id_c_alucontrol == 6'h05) ? 6'h05 : 0;

	wire [4:0] shamt = id_c_alucontrol == 6'h0f ? 5'h10 : id_shamt;

	/* the alu */
	wire [31:0] alu_r = 
		(alu_func == 6'h21) ? x + y :
		(alu_func == 6'h24) ? x & y :
		(alu_func == 6'h27) ? ~(x|y) :
		(alu_func == 6'h25) ? x | y :
		(alu_func == 6'h2a) ? cmp_signed :
		(alu_func == 6'h2b) ? cmp_unsigned :
		(alu_func == 6'h00) ? y << shamt :
		(alu_func == 6'h02) ? y >> shamt :
		(alu_func == 6'h01) ? x << y[4:0] :
		(alu_func == 6'h03) ? x >> y[4:0] :
		(alu_func == 6'h23) ? x - y :
		(alu_func == 6'h04) ? x != y :
		(alu_func == 6'h05) ? x == y :
		(alu_func == 6'h10) ? r_mul[31:0] :
		(alu_func == 6'h11) ? r_mul[63:32] : 0;


	/* branch and jump logic */
	wire [31:0] jalra = 8 + id_pc;
	wire [31:0] pc_4 = id_pc + 4;
	wire [31:0] jjal_jaddr = {pc_4[31:28], id_jaddr, 2'b0};
	assign c_j = id_c_j;
	assign c_b = id_c_b & (alu_r == 0);
	assign jaddr = id_c_jjr ? x : jjal_jaddr;
	assign baddr = {id_se[29:0],2'b0} + pc_4;

	assign int_pc = id_pc;

	always @(posedge clk) begin
		if (!cpu_stall) begin
		if (rst || int_flush) begin
			p_c_rfw <= 0;
			p_c_wbsource <= 0;
			p_c_drw <= 0;
			p_alu_r <= 0;
			p_rfb <= 0;
			p_rf_waddr <= 0;
			p_jalra <= 0;
			p_rt <= 0;
		end else begin
			p_c_rfw <= id_c_rfw;
			p_c_wbsource <= id_c_wbsource;
			p_c_drw <= id_c_drw;
			p_alu_r <= alu_r;
			p_rfb <= eff_y;
			p_rf_waddr <= id_rf_waddr;
			p_jalra <= jalra;
			p_rt <= id_rt;
		end
	
		/* debug statements, not synthesized by Xilinx */
		//$display("EX: PC: %x", id_pc);
		end
	end
endmodule
