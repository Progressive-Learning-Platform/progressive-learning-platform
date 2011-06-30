/*
    Copyright 2010 David Fritz, Brian Gordon, Wira Mulia

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


/*
fritz

generic 8kb block ram. taken from the xilinx xst style guide for generating inferred rams.
*/

module inferred_ram(clka, clkb, ena, enb, wea, addra, addrb, dia, doa, dob);
        input clka, clkb;
        input wea;
        input ena, enb;
        input [10:0] addra, addrb;
        input [31:0] dia;
        output reg [31:0] doa, dob;
        reg [31:0] RAM [2047:0];

        always @(posedge clka) begin
                if (ena) begin
                        if (wea) begin
                                RAM[addra] <= dia;
				$display("RAM: %x written to %x", dia, addra);
			end
                        doa <= RAM[addra];
                end
        end

        always @(posedge clkb) begin
                if (enb) begin
                        dob <= RAM[addrb];
                end
        end
endmodule

