#!/bin/sh 
# install test procedures

# pass name for your OS as argument to shell script
OS=OSX 
CVER=../../bin/cver

if test ! -f ../../bin/cver 
then
  echo "ERROR - there is no cver in ../../bin/"
  echo "Must make in src/ directory" 
  exit;
fi

make -f makefile.$OS probe.dylib
$CVER -q +loadpli1=./probe:pli1_compat_bootstrap probe.v >/dev/null
./rmlic.pl
diff verilog.log probe.plg
rm probe.dylib probe.o

make -f makefile.$OS tfclk.dylib
$CVER -q tfclk.v +loadpli1=./tfclk:pli1_compat_bootstrap >/dev/null
./rmlic.pl
diff verilog.log tfclk.plg
rm tfclk.dylib tfclk.o

make -f makefile.$OS plimfil.dylib
$CVER +memfile+testmem.dat -q +loadpli1=./plimfil:pli1_compat_bootstrap plimfil.v >/dev/null
./rmlic.pl
diff verilog.log plimfil.plg
rm plimfil.dylib plimfil.o

make -f makefile.$OS plimfil2.dylib
$CVER +memfile+testmem2.dat -q +loadpli1=./plimfil2:pli1_compat_bootstrap plimfil2.v >/dev/null
./rmlic.pl
diff verilog.log plimfil2.plg
rm plimfil2.dylib plimfil2.o

echo ">>>> MAC OSX PLI 1.0 test finished - no diff differences should be printed."
echo " "
