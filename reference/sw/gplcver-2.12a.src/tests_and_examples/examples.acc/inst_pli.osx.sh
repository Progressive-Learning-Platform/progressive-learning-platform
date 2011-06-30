#!/bin/sh 
# install test procedures

OS=osx
CVER=../../bin/cver

if test ! -f ../../bin/cver 
then
  echo "ERROR - there is no cver in ../../bin/"
  echo "Must make in src/ directory" 
  exit;
fi

make -f makefile.$OS acc_probe.dylib
$CVER -q +loadpli1=./acc_probe:pli1_compat_bootstrap probe.v >/dev/null
./rmlic.pl
diff verilog.log acc_probe.plg
rm acc_probe.dylib acc_probe.o 

make -f makefile.$OS acc_nxtchld.dylib
$CVER -q +loadpli1=./acc_nxtchld:pli1_compat_bootstrap nc_fdsp.v >/dev/null
./rmlic.pl
diff verilog.log acc_nxtchld.plg
rm acc_nxtchld.dylib acc_nxtchld.o

make -f makefile.$OS accxl_drvld.dylib
$CVER -q +loadpli1=./accxl_drvld:pli1_compat_bootstrap accxldrvtst.v >/dev/null
./rmlic.pl
diff verilog.log accxldrvtst.plg
rm accxl_drvld.dylib accxl_drvld.o 

make -f makefile.$OS acc_prtchg.dylib
$CVER -q +loadpli1=./acc_prtchg:pli1_compat_bootstrap pchg_fdsp.v >/dev/null
./rmlic.pl
diff verilog.log pchg_fdsp.plg
rm acc_prtchg.dylib acc_prtchg.o 

echo ">>>> acc_ test Finished for Mac OSX- no diff differences should be printed."
echo " "
