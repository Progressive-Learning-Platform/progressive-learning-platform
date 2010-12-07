/*
David Fritz

RAM module, which uses an inferred block ram

*/

module mod_ram(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout);
        input rst;
        input clk;
        input ie,de;
        input [31:0] iaddr, daddr;
        input drw;
        input [31:0] din;
        output [31:0] iout, dout;

        /* by spec, the iout and dout signals must go hiZ when we're not using them */
        wire [31:0] idata, ddata;
        assign iout = ie ? idata : 32'hzzzzzzzz;
        assign dout = de ? ddata : 32'hzzzzzzzz;

        inferred_ram ram(~clk,~clk,1'b1,1'b1,(drw & de),daddr[12:2],iaddr[12:2],din,ddata,idata);

endmodule
