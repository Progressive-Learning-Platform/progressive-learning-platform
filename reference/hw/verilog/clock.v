/*
    Clock divider module for Progressive Learning Platform
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
 
// divides input clock by 2
// TODO: Use BUFG?

module clock(
  input      i_clk,
  output reg o_clk = 0
);

  always @(posedge i_clk)
    o_clk = ~o_clk;
endmodule
