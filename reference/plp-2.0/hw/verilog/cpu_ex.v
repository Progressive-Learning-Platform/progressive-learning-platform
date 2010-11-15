/*

fritz

instruction execute phase

*/

module cpu_ex(clk, id_c_b, id_c_rfw, id_c_wbsource, id_c_drw, id_c_alucontrol, id_rfa, id_rfb, id_rfbse, id_shamt, id_func, id_rf_waddr, id_baddr, id_jalra, p_c_rfw, p_c_wbsource, p_c_drw, p_alu_r, p_rfb, p_rf_waddr, p_jalra, c_b, baddr);
	input 		clk;
	input		id_c_b;
	input		id_c_rfw;
	input		id_c_wbsource;
	input		id_c_drw;
	input [4:0]	id_c_alucontrol;
	input [31:0]	id_rfa;
	input [31:0]	id_rfb;
	input [31:0]	id_rfbse;
	input [4:0]	id_shamt;
	input [5:0]	id_func;
	input [4:0]	id_rf_waddr;
	input [31:0]	id_baddr;
	input [31:0]	id_jalra;
	output reg	p_c_rfw;
	output reg [1:0] p_c_wbsource;
	output reg	p_c_drw;
	output reg [31:0] p_alu_r;
	output reg [31:0] p_rfb;
	output reg [4:0] p_rf_waddr;
	output reg [31:0] p_jalra;
	output c_b;
	output [31:0] baddr;

	wire zero = (alu_r == 32'd0);

	assign c_b = zero & id_c_b;
	assign baddr = id_baddr;

	/* the alu */
	wire [31:0] alu_r = ???

	/* alu control */
	wire [4:0] alu_func = ???

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
