/*
    Test fixture for Progressive Learning Platform
    Copyright 2012 Matthew Gaalswyk

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

`timescale 1ns / 1ps
`define CPU_T testfixture.uut.cpu_t
`define CPU_IF CPU_T.stage_if
`define ROM testfixture.uut.arbiter_t.rom_t.rom.RAM
`define RAM testfixture.uut.arbiter_t.ram_t.data_array.RAM
`define PC CPU_T.pc.pc

`include "defines.v"
module testfixture;
`include "parameters.v"
reg        clk;
wire [7:0] leds;
reg        rst;
wire       txd;
reg        rxd;
reg  [7:0] switches;

top uut(clk,leds,rst,txd,rxd,switches);

always #1 clk = ~clk;

initial begin
  // line delimited 32 bit wide hex, 512w max
  $readmemh("../bootrom/plp.v.hex",`ROM);
  //$readmemh("../programs/multtest.hex",`RAM);
  rst = 0; 
  clk = 0;
  #50;
  rst = 1;
  #50;
  rst = 0;
  #400
  $display("500 time units done");
  $stop;
 
end


// XXX: autogenerate?
//
// ASCII decoding
//
// TODO: turn into a task so we can have this info for any stage
reg  [39:0] _ascii_instruction; // 40 bits, 5 bytes/chars
reg  [23:0] _ascii_alu_op;
wire [5:0] opcode = `CPU_T.iin[31:26];
wire [4:0] shamt  = `CPU_T.iin[10: 6];
wire [5:0] func   = `CPU_T.iin[ 5: 0];
wire [4:0] rs     = `CPU_T.iin[25:21];
wire [4:0] rt     = `CPU_T.iin[20:16];
always @(opcode or func or shamt or rs or rt) begin
  // ascii alu operation decoding
  case({func})
    F_LSHIFT:
      _ascii_alu_op = "<< ";
    F_LSHIFTV:
      _ascii_alu_op = "<< ";
    F_RSHIFT:
      _ascii_alu_op = ">> ";
    F_RSHIFTV:
      _ascii_alu_op = ">> ";
    F_MULLO:
      _ascii_alu_op = "*LO";
    F_MULHI:
      _ascii_alu_op = "*HI";
    F_ADD:
      _ascii_alu_op = " + ";
    F_SUB:
      _ascii_alu_op = " - ";
    F_AND:
      _ascii_alu_op = " & ";
    F_OR:
      _ascii_alu_op = " | ";
    F_XOR:
      _ascii_alu_op = " ^ ";
    F_NOR:
      _ascii_alu_op = "~| ";
    F_CMP_S:
      _ascii_alu_op = "<+-";
    F_CMP_U:
      _ascii_alu_op = "<++";
    F_NEQ:
      _ascii_alu_op = "== ";
    F_EQ:
      _ascii_alu_op = "!= ";
    default:
      _ascii_alu_op = "%Er";
  endcase

  // ascii instruction decoding
  case({opcode})
    R_TYPE:
    case({func})
    ADDU :
      if(rt == SPR_Z)
      begin
             _ascii_instruction = "move "; // pseudo-op
      end else begin
             _ascii_instruction = "addu ";
      end
    AND  :   _ascii_instruction = "and  ";
    JR   :   _ascii_instruction = "jr   ";
    JALR :   _ascii_instruction = "jalr ";
    MULHI:   _ascii_instruction = "mulhi";
    MULLO:   _ascii_instruction = "mullo";
    NOR  :   _ascii_instruction = "nor  ";
    OR   :   _ascii_instruction = "or   ";
    XOR  :   _ascii_instruction = "xor  ";
    SLL  :
      case({shamt})
        0:   _ascii_instruction = "nop  "; // pseudo-op
        default:
             _ascii_instruction = "sll  ";
      endcase
    SLLV :   _ascii_instruction = "sllv ";
    SLT  :   _ascii_instruction = "slt  ";
    SLTU :   _ascii_instruction = "sltu ";
    SRL  :
      case({shamt})
        0:   _ascii_instruction = "nop  "; // pseudo-op
        default:
             _ascii_instruction = "srl  ";
      endcase
    SRLV :   _ascii_instruction = "srlv ";
    SUBU :   _ascii_instruction = "subu ";
    default:
      begin
             _ascii_instruction = "%Err ";
      end
    endcase // end func/R_TYPE case
    ADDIU:   _ascii_instruction = "addiu";
    ANDI :   _ascii_instruction = "andi ";
    BEQ  :
      if(rs == SPR_Z && rt == SPR_Z)
      begin
             _ascii_instruction = "b    "; // pseudo-op
      end else begin
             _ascii_instruction = "beq  ";
      end
    BNE  :   _ascii_instruction = "bne  ";
    J    :   _ascii_instruction = "j    ";
    JAL  :   _ascii_instruction = "jal  ";
    LUI  :   _ascii_instruction = "lui  ";
    LW   :   _ascii_instruction = "lw   ";
    ORI  :   _ascii_instruction = "ori  ";
    XORI :   _ascii_instruction = "xori ";
    SLTI :   _ascii_instruction = "slti ";
    SLTIU:   _ascii_instruction = "sltiu";
    SW   :   _ascii_instruction = "sw   ";
    default:
      begin
             _ascii_instruction = "%Err ";
      end
  endcase
end

endmodule
