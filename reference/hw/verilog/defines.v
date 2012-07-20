/*
    Verilog defines for Progressive Learning Platform
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

`ifndef _PLP_DEFINES
`define _PLP_DEFINES

// data bus width
`define W_DATA 32
// instruction size
`define W_INST 32
// memory address width (same for data/inst)
`define W_ADDR 32
// register file size, including 0 register
`define RF_SIZE 32
// register file address width
`define W_RFADDR 5
// width of jump address in instruction
`define W_JADDR 26

`endif