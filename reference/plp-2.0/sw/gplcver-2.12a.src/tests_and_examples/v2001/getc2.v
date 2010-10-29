/*
 * fileio test of getc - copy to stdout
 */

/*
 * ungetc test
 * read and output a file but emit extra new line with ungetc
 */
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
  integer d;

  initial
     begin : file_block
      file = $fopen("infil.txt", "r");
      if (file == `NULL) disable file_block;
  
      begin : read_block
       forever  
         begin
          c = $fgetc(file);
          if (c == `EOF) disable read_block;
	  d = $fgetc(file);
          if (d == `EOF) disable read_block;
          $fwrite(`STDOUT, "%c%c", c, d);
          if (c == "\n")
           begin
            d = $ungetc(d, file);
           end
         end
      end
      $fclose(file);
    end
endmodule // read_pattern
