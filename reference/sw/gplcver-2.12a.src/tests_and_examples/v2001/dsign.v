module test;
 reg signed [3:0] reg1;
 reg signed [3:0] reg2;
 reg signed [3:0] result;
 
 initial begin 
     reg1 = -12 / 3; // expression result is -4. regS is a signed register
     $display("%b %d \n", reg1, reg1); 
     reg1 = -4'sd12 / 3;// expression result is 1. -4'sd12 is actually 4
     $display("%b %d \n", reg1, reg1); // should be 1 
 end
endmodule
