/*

fritz

instruction decode phase

*/

module cpu_id(clk, if_pc, if_inst, wb_rfw, wb_rf_waddr, wb_rf_wdata, p_rfa, p_rfb, p_rfbse, p_shamt, p_func, p_rf_waddr, p_baddr, p_jalra, p_c_b, c_j, p_c_rfw, p_c_wbsource, p_c_drw, p_c_alucontrol, jaddr);
	input 		clk;
	input	[31:0]	if_pc;
	input	[31:0]	if_inst;
	input 		wb_rfw;
	input   [4:0]	wb_rf_waddr;
	input   [31:0]  wb_rf_wdata;
	output reg [31:0] p_rfa;
	output reg [31:0] p_rfb;
	output reg [31:0] p_rfbse;
	output reg [4:0]  p_shamt;
	output reg [5:0]  p_func;
	output reg [4:0]  p_rf_waddr;
	output reg [31:0] p_baddr;
	output reg [31:0] p_jalra;
	output reg 	  p_c_b;
	output 		  c_j
	output reg 	  p_c_rfw;
	output reg [1:0]  p_c_wbsource;
	output reg 	  p_c_drw;
	output reg [4:0]  p_c_alucontrol;
	output [31:0]	  jaddr;

	reg [31:0] rf [31:1];

	wire [5:0] opcode = if_inst[31:26];
	wire [4:0] rf_addr_a = if_inst[25:21];
	wire [4:0] rf_addr_b = if_inst[20:16];
	wire [4:0] rf_waddr = if_inst[15:11];
	wire [15:0] imm = if_inst[15:0];
	wire [4:0] shamt = if_inst[10:6];
	wire [5:0] func = if_inst[5:0];i

	/* internal logic */
	wire [31:0] signext_imm = {{16{imm[15]}},imm};
        wire [31:0] zeroext_imm = {{16{1'b0}},imm};
	wire [31:0] se = c_se ? signext_imm : zeroext_imm;
	wire [31:0] baddr = se + if_pc;
	wire [31:0] jalra = 4 + if_pc;

	/* control logic */
	wire c_b = ???
	wire c_rfw = ???
	wire c_wbsource = ???
	wire c_drw = ???
	wire c_alucontrol = ???
	wire c_se = ???
	wire c_rfbse = ???
	wire c_jjr = ???

	assign jaddr = c_jjr ? rf[rf_addr_a] : rf_if_inst[25:0];
	assign c_j = ???

	always @(posedge clk) begin
		p_rfa <= rf[rf_addr_a];
		p_rfb <= rf[rf_addr_b];
		p_rfbse <= se
		p_shamt <= shamt;
		p_func <= func;
		p_rf_waddr <= rf_waddr;
		p_baddr <= baddr;
		p_jalra <= jalra;
		p_c_b <= c_b;
		p_c_rfw <= c_rfw;
		p_c_wbsource <= c_wbsource;
		p_c_drw <= c_drw;
		p_c_alucontrol = c_alucontrol;

		/* regfile */
		if (wb_rfw && wb_rf_waddr != 5'd0) begin
			rf[wb_rf_waddr] <= wb_rf_wdata;
		end
	end
endmodule
