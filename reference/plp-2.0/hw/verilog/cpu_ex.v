/*

fritz

instruction execute phase

*/

module cpu_ex(clk, id_c_rfw, id_c_wbsource, id_c_drw, id_c_alucontrol, id_rfa, id_rfb, id_rfbse, id_shamt, id_func, id_rf_waddr, id_jalra, p_c_rfw, p_c_wbsource, p_c_drw, p_alu_r, p_rfb, p_rf_waddr, p_jalra);
	input 		clk;
	input		id_c_rfw;
	input		id_c_wbsource;
	input		id_c_drw;
	input [5:0]	id_c_alucontrol;
	input [31:0]	id_rfa;
	input [31:0]	id_rfb;
	input [31:0]	id_rfbse;
	input [4:0]	id_shamt;
	input [5:0]	id_func;
	input [4:0]	id_rf_waddr;
	input [31:0]	id_jalra;
	output reg	p_c_rfw;
	output reg [1:0] p_c_wbsource;
	output reg	p_c_drw;
	output reg [31:0] p_alu_r;
	output reg [31:0] p_rfb;
	output reg [4:0] p_rf_waddr;
	output reg [31:0] p_jalra;

	wire [31:0] x = id_rfa;
	wire [31:0] y = id_rfbse;
	wire cmp_signed = (x[31] == y[31]) ? x < y : x[31];
	wire cmp_unsigned = x < y;

	/* the alu */
	wire [31:0] alu_r = 
		(alu_func == 6'h21) ? x + y :
		(alu_func == 6'h24) ? x & y :
		(alu_func == 6'h27) ? ~(x|y) :
		(alu_func == 6'h25) ? x | y :
		(alu_func == 6'h2a) ? cmp_signed :
		(alu_func == 6'h2b) ? cmp_unsigned :
		(alu_func == 6'h00) ? x << id_shamt :
		(alu_func == 6'h02) ? x >> id_shamt :
		(alu_func == 6'h23) ? x - y : 0;

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
		(id_c_alucontrol == 6'h2b) ? 6'h21 : 0;

	always @(posedge clk) begin
		p_c_rfw <= id_c_rfw;
		p_c_wbsource <= id_c_wbsource;
		p_c_drw <= p_c_drw;

		p_alu_r <= alu_r;
		p_rfb <= id_rfb;
		p_rf_waddr <= id_rf_waddr;
		p_jalra <= id_jalra;
	end
endmodule
