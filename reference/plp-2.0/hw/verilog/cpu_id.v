/*

fritz

instruction decode phase

*/

module cpu_id(rst, clk, if_pc, if_inst, wb_rfw, wb_rf_waddr, wb_rf_wdata, p_rfa, p_rfb, p_rfbse, p_shamt, p_func, p_rf_waddr, baddr, p_jalra, c_b, c_j, p_c_rfw, p_c_wbsource, p_c_drw, p_c_alucontrol, jaddr);
	input 		rst, clk;
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
	output     [31:0] baddr;
	output reg [31:0] p_jalra;
	output  	  c_b;
	output 		  c_j;
	output reg 	  p_c_rfw;
	output reg [1:0]  p_c_wbsource;
	output reg 	  p_c_drw;
	output reg [5:0]  p_c_alucontrol;
	output [31:0]	  jaddr;

	reg [31:0] rf [31:1];

	wire [5:0] opcode = if_inst[31:26];
	wire [4:0] rf_rs = if_inst[25:21];
	wire [4:0] rf_rt = if_inst[20:16];
	wire [4:0] rf_rd = if_inst[15:11];
	wire [15:0] imm = if_inst[15:0];
	wire [4:0] shamt = if_inst[10:6];
	wire [5:0] func = if_inst[5:0];
	wire [31:0] rfa = rf_rs == 0 ? 0 : rf[rf_rs];
	wire [31:0] rfb = rf_rt == 0 ? 0 : rf[rf_rt];


	/* control logic */
	wire c_rfw = ( 
		opcode != 6'h04 && 
		opcode != 6'h05 && 
		opcode != 6'h2b && 
		opcode != 6'h02); /* secret bug, jump register asserts write enable but the assembler sets rd = 0 */
	wire [1:0] c_wbsource = 
		(opcode == 6'h23) ? 2'h1 :
		(opcode == 6'h03) ? 2'h2 :
		(opcode == 6'h00 && func == 6'h09) ? 2'h2 : 0; 
	wire c_drw = opcode == 6'h2b ? 1 : 0;
	wire [5:0] c_alucontrol = opcode;
	wire c_se = (opcode == 6'h0c || opcode == 6'h0d) ? 0 : 1;
	wire c_rfbse = opcode == 6'h00 ? 0 : 1;
	wire c_jjr = 
		opcode == 6'h02 ? 0 :
		opcode == 6'h03 ? 0 : 1;
	wire [1:0] c_rd_rt_31 = 
		(opcode == 6'h03) ? 2'b10 : /* jal */
		(opcode == 6'h00) ? 2'b00 : 2'b01;

	/* internal logic */
	wire [31:0] signext_imm = {{16{imm[15]}},imm};
        wire [31:0] zeroext_imm = {{16{1'b0}},imm};
	wire [31:0] se = c_se ? signext_imm : zeroext_imm;
	wire [31:0] jalra = 8 + if_pc;
	wire [4:0] rd_rt_31 = 
		(c_rd_rt_31 == 2'b00) ? rf_rd :
		(c_rd_rt_31 == 2'b01) ? rf_rt :
		(c_rd_rt_31 == 2'b10) ? 5'b11111 : rf_rd;
	wire [31:0] rfbse = c_rfbse ? se : rf_rt;
	wire [31:0] pc_4 = if_pc + 4; /* used only because I can't figure out how to get the next statement to inline this */
	wire [31:0] jjal_jaddr = {pc_4[31:28], if_inst[25:0], 2'b0};

	assign jaddr = c_jjr ? rfa : jjal_jaddr;
	assign c_j = 
		(opcode == 6'h02) || 
		(opcode == 6'h03) || 
		(opcode == 6'h00 && func == 6'h08) || 
		(opcode == 6'h00 && func == 6'h09);
	assign baddr = {{14{imm[15]}},imm,2'b0} + if_pc + 4;

	assign c_b = 
		(opcode == 6'h04) ? (rfa == rfb) :
		(opcode == 6'h05) ? (rfa != rfb) : 0;

	always @(posedge clk) begin
		if (rst) begin		
			p_rfa <= 0;
			p_rfb <= 0;
			p_rfbse <= 0;
			p_shamt <= 0;
			p_func <= 0;
			p_rf_waddr <= 0;
			p_jalra <= 0;
			p_c_rfw <= 0;
			p_c_wbsource <= 0;
			p_c_drw <= 0;
			p_c_alucontrol = 0;
		end else begin
			p_rfa <= rfa;
			p_rfb <= rfb;
			p_rfbse <= rfbse;
			p_shamt <= shamt;
			p_func <= func;
			p_rf_waddr <= rd_rt_31;
			p_jalra <= jalra;
			p_c_rfw <= c_rfw;
			p_c_wbsource <= c_wbsource;
			p_c_drw <= c_drw;
			p_c_alucontrol = c_alucontrol;
		end
		
		/* regfile */
		if (wb_rfw && wb_rf_waddr != 5'd0) begin
			rf[wb_rf_waddr] <= wb_rf_wdata;
			$display("%d - reg %d: %x", if_pc, wb_rf_waddr, wb_rf_wdata);
		end
	end
endmodule
