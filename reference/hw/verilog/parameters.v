/*
    Verilog parameters for Progressive Learning Platform
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

`include "defines.v"

// Miscellaneous
localparam SPR_JRA   = 6'd31; // Special purpose register
localparam SPR_Z     = 6'd0;
localparam ZERO_DATA = 32'd0;


// c_dmem_rw control signal constants, 0 or 1 hot encoding
localparam DMEM_READ  = 2'b10;
localparam DMEM_WRITE = 2'b01;
localparam DMEM_NOP   = 2'b00;


//
// MUX CONSTANTS
//
// JJR Mux
localparam MUX_JADDR = 1'd0;
localparam MUX_JRA   = 1'd1;
// WB_DEST Mux
localparam MUX_RD = 2'd0;
localparam MUX_RT = 2'd1;
localparam MUX_RA = 2'd2;
// WB_SRC Mux
localparam MUX_JALRA = 2'd2;
localparam MUX_DWORD = 2'd1;
localparam MUX_ALU_R = 2'd0;
// ALUY_SRC Mux
localparam MUX_IMM = 1'd1;
localparam MUX_RFB = 1'd0;
// Immediate zero/sign extend mux
localparam MUX_ZE = 1'd0;
localparam MUX_SE = 1'd1;


//
// OPCODES - keep sorted in hex order
//
localparam R_TYPE = 6'h00;

localparam J      = 6'h02;
localparam JAL    = 6'h03;
localparam BEQ    = 6'h04;
localparam BNE    = 6'h05;

localparam ADDIU  = 6'h09;
localparam SLTI   = 6'h0a;
localparam SLTIU  = 6'h0b;
localparam ANDI   = 6'h0c;
localparam ORI    = 6'h0d;
localparam XORI   = 6'h0e;

localparam LUI    = 6'h0f;

localparam LW     = 6'h23;

localparam SW     = 6'h2b;


//
// FUNCTIONS - keep sorted in hex order
//
localparam SLL    = 6'h00;

localparam SRL    = 6'h02;

localparam SLLV   = 6'h04;

localparam SRLV   = 6'h06;

localparam JR     = 6'h08;
localparam JALR   = 6'h09;

localparam MULLO  = 6'h10;
localparam MULHI  = 6'h11;

localparam ADDU   = 6'h21;

localparam SUBU   = 6'h23;
localparam AND    = 6'h24;
localparam OR     = 6'h25;
localparam XOR    = 6'h26;
localparam NOR    = 6'h27;

localparam SLT    = 6'h2a;
localparam SLTU   = 6'h2b;


//
// ALU FUNCTIONS - keep sorted in hex order
//
localparam F_LSHIFT  = 6'h00;

localparam F_RSHIFT  = 6'h02;

localparam F_LSHIFTV = 6'h04;

localparam F_RSHIFTV = 6'h06;

localparam F_MULLO   = 6'h10; // 32x32 signed mult, return low 32
localparam F_MULHI   = 6'h11; // 32x32 signed mult, return high 32

localparam F_ADD     = 6'h21;

localparam F_SUB     = 6'h23;
localparam F_AND     = 6'h24;
localparam F_OR      = 6'h25;
localparam F_XOR     = 6'h26;
localparam F_NOR     = 6'h27;

localparam F_CMP_S   = 6'h2a; // x < y signed
localparam F_CMP_U   = 6'h2b; // x < y unsigned

localparam F_NEQ     = 6'h3a; // XXX: make sure these fx codes are ok
localparam F_EQ      = 6'h3b; // XXX: make sure these fx codes are ok