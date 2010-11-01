/*
David Fritz
1.26.2010

MIPS ALU
*/

/*
function table (all values in hex):

20: add, addi
21: addu, addiu, lbu, lhu, lui, lw, sb, sh, sw
24: and, andi
08: jr
27: nor
25: or, ori
2a: slt. slti
2b: sltu, sltiu
00: sll, j, jal
02: srl
22: sub, beq, bne (branch instruction result comes from the zero bit, since internally it's a signed subtract)
23: subu

*/

module alu(x,y,funct,shamt,r,overflow,zero);
	input  [31:0] x,y;
	input  [5:0] funct;
	input  [4:0] shamt;
	output [31:0] r;
	output overflow;
	output zero;

	wire [32:0] rbs; //result before shift
	assign r = rbs[31:0];

	//the overflow bit, which is asserted only on add/subtract
	assign overflow = (funct == 6'h20 || funct == 6'h22) ? rbs[32] : 0;

	//zero (nor the output bits)
	assign zero = r == 0 ? 1 : 0;
	
	wire signed_compare;
	wire unsigned_compare;
	assign signed_compare = (x[31] == y[31]) ? (x < y) : (x[31]);
	assign unsigned_compare = (x < y) ? 1 : 0;

	assign rbs = (funct == 6'h20) ? x + y :
					 (funct == 6'h21) ? x + y :
					 (funct == 6'h24) ? x & y :
					 (funct == 6'h08) ? x :
					 (funct == 6'h27) ? ~(x|y):
					 (funct == 6'h25) ? x | y :
					 (funct == 6'h2a) ? signed_compare :
					 (funct == 6'h2b) ? unsigned_compare :
					 (funct == 6'h00) ? y << shamt :
					 (funct == 6'h02) ? y >> shamt :
					 (funct == 6'h22) ? x - y :
					 (funct == 6'h23) ? x - y : 0;
endmodule
