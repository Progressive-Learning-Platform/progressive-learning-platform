module test #(parameter integer p1 = 5, parameter p2 = 7,
 parameter r1 = 7.3) (
 input [7:0] a,
 input signed [7:0] b, c, d, // multiple port that share attributes
 output [7:0] e,
 output signed [7:0] f,g,
 output signed [7:0] h) ;

 task my_task(input a, b, inout c, output signed [15:0] d, e);
  begin
   c = a;
   d = f;
   e= 2*f;
  end
 endtask

endmodule
  
