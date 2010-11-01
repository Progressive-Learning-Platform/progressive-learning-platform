/*
David Fritz

2.5.2010

Single cycle MIPS machine
*/

module cpu(clk, daddr, dout, din, drw, iaddr, iin, rst);
	input clk, rst;
	output [31:0] daddr;
	output [31:0] dout;
	input [31:0] din;
	output drw;
	output [31:0] iaddr;
	input [31:0] iin;

	reg [31:0] pc;
	wire [31:0] ireg;

	/* datapath wires */
	wire [31:0] alu_x, alu_y, alu_r, rf_wdata, rf_rdata1, rf_rdata2;
	wire [5:0] alu_funct;
	wire [4:0] alu_shamt, rf_raddr1, rf_raddr2, rf_waddr;
	wire alu_overflow, alu_zero, rf_w;

	/* instruction decode stuff (the actual control logic)*/
	wire [5:0] opcode;
	wire [4:0] rs,rt,rd;
	wire [4:0] shamt;
	wire [5:0] funct;
	wire [15:0] imm;
	wire [25:0] jaddr;
	wire [1:0] itype;
	wire [31:0] eff_imm;
	wire [31:0] signext_imm;
	wire [31:0] zeroext_imm;
	wire [31:0] eff_b_addr;
	wire [31:0] eff_j_addr;
	wire [31:0] pc_4;
	wire [5:0] eff_funct;
	wire eff_w;
	wire [31:0] new_pc;

	assign opcode = ireg[31:26];
	assign rs = ireg[25:21];
	assign rt = ireg[20:16];
	assign rd = ireg[15:11];
	assign shamt = ireg[10:6];
	assign funct = ireg[5:0];
	assign imm = ireg[15:0];
	assign jaddr = ireg[25:0];

	assign signext_imm = {{16{imm[15]}},imm};
	assign zeroext_imm = {{16{1'b0}},imm};

	assign eff_imm = 
		(opcode == 6'h08) ? signext_imm :
		(opcode == 6'h09) ? signext_imm :
		(opcode == 6'h0c) ? zeroext_imm :
		(opcode == 6'h04) ? rf_rdata2 :
		(opcode == 6'h05) ? rf_rdata2 :
		(opcode == 6'h0f) ? {imm,16'b0} :
		(opcode == 6'h23) ? signext_imm :
		(opcode == 6'h0d) ? zeroext_imm :
		(opcode == 6'h0a) ? signext_imm :
		(opcode == 6'h0b) ? signext_imm :
		(opcode == 6'h2b) ? signext_imm : 0;

	assign itype = 
		(opcode == 6'h00) ? 0 :
		(opcode == 6'h08) ? 1 :
		(opcode == 6'h09) ? 1 : 
		(opcode == 6'h0c) ? 1 :
		(opcode == 6'h04) ? 1 :
		(opcode == 6'h05) ? 1 :
		(opcode == 6'h0f) ? 1 :
		(opcode == 6'h23) ? 1 :
		(opcode == 6'h0d) ? 1 :
		(opcode == 6'h0a) ? 1 :
		(opcode == 6'h0b) ? 1 :
		(opcode == 6'h2b) ? 1 :
		(opcode == 6'h02) ? 2 :
		(opcode == 6'h03) ? 2 : 0; 
	
	assign eff_b_addr = {{14{imm[15]}},imm,2'b0} + pc;
	assign eff_j_addr = {pc[31:28],jaddr,2'b0};
	assign pc_4 = pc + 4;

	assign eff_funct = 
                (opcode == 6'h08) ? 6'h20 :
                (opcode == 6'h09) ? 6'h21 :
                (opcode == 6'h0c) ? 6'h24 :
                (opcode == 6'h04) ? 6'h22 :
                (opcode == 6'h05) ? 6'h22 :
                (opcode == 6'h0f) ? 6'h21 :
                (opcode == 6'h23) ? 6'h21 :
                (opcode == 6'h0d) ? 6'h25 :
                (opcode == 6'h0a) ? 6'h2a :
                (opcode == 6'h0b) ? 6'h2b :
                (opcode == 6'h2b) ? 6'h21 : funct;

	assign eff_w = 
		(opcode == 6'h04) ? 0 :
		(opcode == 6'h05) ? 0 :
		(opcode == 6'h02) ? 0 :
		(opcode == 6'h2b) ? 0 : 1;

	assign new_pc = 
			(rst) ? 32'h00000000 :
			(opcode == 6'h04 && alu_zero) ? eff_b_addr :
			(opcode == 6'h05 && !alu_zero) ? eff_b_addr :
			(opcode == 6'h02) ? eff_j_addr :
			(opcode == 6'h03) ? eff_j_addr :
			(opcode == 6'h00 && funct == 6'h08) ? rf_rdata1 : pc_4 ;		

	/* end instruction decode */

	/* the datapath */
	alu alu_t(alu_x,alu_y,alu_funct,alu_shamt,alu_r,alu_overflow,alu_zero);
	registerfile regfile_t(rf_raddr1,rf_raddr2,rf_waddr,rf_wdata,rf_w,rf_rdata1,rf_rdata2,clk);

	assign alu_x = rf_rdata1; /* first input to the alu is always rs */
	assign alu_y = 
		(itype == 0) ? rf_rdata2 :
		(itype == 1) ? eff_imm : 0;
	assign alu_funct = eff_funct;
	assign alu_shamt = (opcode == 0) ? shamt : 0;
	assign rf_raddr1 = rs;
	assign rf_raddr2 = rt;
	assign rf_waddr  = 
		(itype == 0) ? rd :
		(itype == 1) ? rt :
		(itype == 2) ? 5'h1f : 0;
	assign rf_wdata = 
		(opcode == 6'h03) ? pc_4 :
		(opcode == 6'h23) ? din : alu_r;
	assign rf_w = eff_w;	

	/* statewise things */
	always @(posedge clk) begin
		pc <= new_pc;
	end
		
	/* bus goodies */
	assign iaddr = pc;
	assign drw = (opcode == 6'h2b) ? 1 : 0;
	assign daddr = alu_r; //= (opcode == 6'h23 || opcode == 6'h2b) ? alu_r : 32'b0;
	assign dout = rf_rdata2;
	assign ireg = iin;
	
endmodule
