/*
fritz

10.31.2010

generic block rom. taken from the xilinx xst style guide for generating inferred rams.
*/

module inferred_rom(clka, clkb, ena, enb, addra, addrb, doa, dob);
        input clka, clkb;
        input ena, enb;
        input [8:0] addra, addrb;
        output reg [31:0] doa, dob;
        reg [31:0] RAM [511:0];

        always @(posedge clka) begin
                if (ena) begin
                        doa <= RAM[addra];
                end
        end

        always @(posedge clkb) begin
                if (enb) begin
                        dob <= RAM[addrb];
                end
        end

	initial begin
		RAM[0]   = 32'h00000000;
		RAM[511] = 32'h00000000;
	end
endmodule

