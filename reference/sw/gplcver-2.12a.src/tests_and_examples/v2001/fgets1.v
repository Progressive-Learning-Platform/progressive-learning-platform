/*
 * fileio test of getc - copy to stdout
 */

`timescale 1ns / 10 ps
`define EOF -1
`define NULL 0
`define MAX_LINE_LENGTH 1000
`define STDOUT 32'h8000_0001
module test1;
  integer file;
  reg [3:0] bin;
  reg [31:0] dec, hex;
  real real_time;
  reg [8*`MAX_LINE_LENGTH-1:0] line; /* Line of text read from file */
  integer r;
  integer c;

  initial
     begin : file_block
      file = $fopen("infil.txt", "r");
      if (file == `NULL) disable file_block;
  
      begin : read_block
       forever  
         begin
          r = $fgets(line, file);
          if (r == 0) disable read_block;
          $fwrite(`STDOUT, "%s", line);
         end
      end
      $fclose(file);
    end
endmodule // read_pattern
