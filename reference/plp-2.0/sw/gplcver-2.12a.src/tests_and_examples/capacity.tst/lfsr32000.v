// DA Solutions LFSR model used for sizing tests
// Created by D. J. Wharton, 1-Sep-94
// Model equivalent to 3,200,000 gates 
// Equivalent model in VHDL

`timescale 1ns / 1ns

module lfsr (d, clk, reset, q);

  input d, clk, reset;
  output q;

  EN  	en_1 (d, q7, xout);

  FD2 	fd2_1 (xout, clk, reset, q1, q1b),
        fd2_2 (q1, clk, reset, q2, q2b),
        fd2_3 (q2, clk, reset, q3, q3b),
        fd2_4 (q3, clk, reset, q4, q4b),
        fd2_5 (q4, clk, reset, q5, q5b),
        fd2_6 (q5, clk, reset, q6, q6b),
        fd2_7 (q6, clk, reset, q7, q7b),
        fd2_8 (q7, clk, reset, q8, q8b),
        fd2_9 (q8, clk, reset, q9, q9b),
        fd2_10 (q9, clk, reset, q, q10b);  

endmodule


module lfsr10 (d, clk, reset, q);

  input d, clk, reset;
  output q;

  lfsr	 lfsr_1 (d,  clk, reset, d1),
         lfsr_2 (d1, clk, reset, d2),
         lfsr_3 (d2, clk, reset, d3),
         lfsr_4 (d3, clk, reset, d4),
         lfsr_5 (d4, clk, reset, d5),
         lfsr_6 (d5, clk, reset, d6),
         lfsr_7 (d6, clk, reset, d7),
         lfsr_8 (d7, clk, reset, d8),
         lfsr_9 (d8, clk, reset, d9),
         lfsr_10(d9, clk, reset, q ); 

endmodule  


module lfsr100 (d, clk, reset, q);

  input d, clk, reset;
  output q;

  lfsr10 lfsr10_1 (d , clk, reset, d1),
         lfsr10_2 (d1, clk, reset, d2),
         lfsr10_3 (d2, clk, reset, d3),
         lfsr10_4 (d3, clk, reset, d4),
         lfsr10_5 (d4, clk, reset, d5),
         lfsr10_6 (d5, clk, reset, d6),
         lfsr10_7 (d6, clk, reset, d7),
         lfsr10_8 (d7, clk, reset, d8),
         lfsr10_9 (d8, clk, reset, d9),
         lfsr10_10(d9, clk, reset, q ); 


endmodule  


module lfsr1000 (d, clk, reset, q);

  input d, clk, reset;
  output q;

  lfsr100 lfsr100_1(d , clk, reset, d1),  
         lfsr100_2 (d1, clk, reset, d2),
         lfsr100_3 (d2, clk, reset, d3),
         lfsr100_4 (d3, clk, reset, d4),
         lfsr100_5 (d4, clk, reset, d5), 
         lfsr100_6 (d5, clk, reset, d6),
         lfsr100_7 (d6, clk, reset, d7),
         lfsr100_8 (d7, clk, reset, d8),
         lfsr100_9 (d8, clk, reset, d9),
         lfsr100_10(d9, clk, reset, q );


endmodule  

module lfsr10000 (d, clk, reset, q);

  input d, clk, reset;
  output q;

  lfsr1000 	lfsr1000_1 (d, clk, reset, d1),
		lfsr1000_2 (d1, clk, reset, d2),
		lfsr1000_3 (d2, clk, reset, d3),
		lfsr1000_4 (d3, clk, reset, d4),
		lfsr1000_5 (d4, clk, reset, d5),
		lfsr1000_6 (d5, clk, reset, d6),
		lfsr1000_7 (d6, clk, reset, d7),
		lfsr1000_8 (d7, clk, reset, d8),
		lfsr1000_9 (d8, clk, reset, d9),
		lfsr1000_10 (d9, clk, reset, q);
endmodule

module lfsr32000 (d, clk, reset, q);

  input d, clk, reset;
  output q;

  lfsr10000	lfsr10000_1 (d, clk, reset, d1),
		lfsr10000_2 (d1, clk, reset, d2),
		lfsr10000_3 (d2, clk, reset, d3);

  lfsr1000	lfsr1000_1 (d3, clk, reset, d4),
		lfsr1000_2 (d4, clk, reset, q);

endmodule

module test;

wire	d;

reg 		reset, creset;
wire		clk_out;
wire		clk_in;

reg	[0:31] 	tempword;
reg	[0:4]	j;

lfsr32000	lfsr32000_1 (d, clk_out, reset, q);

buf		buf_1 (d,q);

not 	#50	clk_1 (clk_in, clk_out);
and		clk_2 (clk_out, clk_in, creset);

  initial
  begin
		reset = 1;
	#25	reset = 0; creset = 0;
	#25	reset = 1;
	#50	creset = 1;
//    	#99900 	$finish(2);
    	#1000 	$finish(2);
  end

// Results display code

initial
begin
	tempword = 0; j = 0;
	#25 $write ("\n");
	    $display ("Results display");
end

always
begin
	@ (posedge clk_out)
	begin
	tempword[j] = d;
	j = j + 1;
	if ( j == 0)
	begin
	$display ($time,,"%b",tempword[0:7],,"%b",tempword[8:15],,"%b",tempword[16:23],,"%b",tempword[24:31],);
	tempword = 0;
	end
	end
end

endmodule
