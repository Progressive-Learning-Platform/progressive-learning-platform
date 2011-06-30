module test;
 
 foo f1();
 foo f2();
 adder a1();

 initial $display("top.v %l");   


endmodule


module adder;
 initial $display("top.v adder %l");   
endmodule
