#!/usr/bin/perl -w

#david fritz
#3.2.2010

#converts an ascii ppm (p3) file to a 3-bit color 320x240 coe file to be used as
#a boot splash screen image

#we're in "hacky" mode, so just assume everything is okay

open INFILE, "<$ARGV[0]" or die("[E] Unable to open $ARGV[1] for reading\n");
open COE, ">$ARGV[0].coe" or die("[E] Unable to open $ARGV[1].coe for writing\n");

@data = <INFILE>; #gobble up all the data

#print the prologue for the coe file
print COE "memory_initialization_radix=2;\n";
print COE "memory_initialization_vector=\n";

for ($i=4; $i <= $#data; $i=$i+3) { #skip the prologue of the ppm file
	chomp $data[$i];
	chomp $data[$i+1];
	chomp $data[$i+2];

	if ($data[$i] == 255)   { $r = 1; } else { $r = 0; }
	if ($data[$i+1] == 255) { $g = 1; } else { $g = 0; }
	if ($data[$i+2] == 255) { $b = 1; } else { $b = 0; }

	#do we want the final comma?
	if ($i+3 > $#data) { #no
		print COE "$r$g$b\n";
	} else {
		print COE "$r$g$b,\n";
	}
}

close(INFILE); 
close(COE);

	
