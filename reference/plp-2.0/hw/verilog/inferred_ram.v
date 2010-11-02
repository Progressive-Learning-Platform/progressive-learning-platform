/*
fritz

10.31.2010

generic block ram. taken from the xilinx xst style guide for generating inferred rams.
*/

module inferred_ram(clka, clkb, ena, enb, wea, addra, addrb, dia, doa, dob);
        parameter SIZE = 4096;

        input clka, clkb;
        input wea;
        input ena, enb;
        input [$clog2(SIZE)-1:0] addra, addrb;
        input [31:0] dia;
        output reg [31:0] doa, dob;
        reg [31:0] RAM [(SIZE<<2)-1:0];

        always @(posedge clka) begin
                if (ena) begin
                        if (wea)
                                RAM[addra] <= dia;
                        doa <= RAM[addra];
                end
        end

        always @(posedge clkb) begin
                if (enb) begin
                        dob <= RAM[addrb];
                end
        end
endmodule

