#!/usr/bin/env python

#fritz
#11.4.2010
#plpromgen
#
#a program to generate the rom module for the plp system.
#
#usage: plpromgen.py plp.hex
#
#the plp.hex file is included in every compiled plp project,
#and can be extracted using `tar -xf file.plp plp.hex`
#
#the generated output is a verilog module consisting of an
#inferred block ram with the ram initialized to the contents
#of the input file.

import sys

pre = '''
module inferred_rom(clka, clkb, ena, enb, addra, addrb, doa, dob);
        input clka, clkb;
        input ena, enb;
        input [8:0] addra, addrb;
        output reg [31:0] doa, dob;
        reg [31:0] RAM [511:0];

        always @(negedge clka) begin
                if (ena) begin
                        doa <= RAM[addra];
                end
        end

        always @(negedge clkb) begin
                if (enb) begin
                        dob <= RAM[addrb];
                end
        end

        initial begin
'''

post = '''
	end
endmodule
'''

infile  = open(sys.argv[1], 'r')
outfile = open('inferred_rom.v','w')
outfile.write(pre)

count = 0
for line in infile:
	outfile.write('\t\tRAM[' + str(count) + '] = ' + line.rstrip() + ';\n')
	count = count + 1

for x in range(count,512):
	outfile.write('\t\tRAM[' + str(x) + '] = 32\'h00000000;\n');

outfile.write(post)

infile.close()
outfile.close()
