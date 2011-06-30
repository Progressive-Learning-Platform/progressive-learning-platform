#!/bin/sh 
# install test procedures

# pass name for your OS as argument to shell script
OS=$1 
CVER=../../bin/cver

if test ! -f ../../bin/cver 
then
  echo "ERROR - there is no cver in ../../bin/"
  echo "Must make in src/ directory" 
  exit;
fi

if [ "$OS" != "lnx" ]
 then
  if [ "$OS" != "sparc-gcc" ]
   then 
    if [ "$OS" != "lnx64" ]
     then
      echo "must specify an OS(lnx, sparc-gcc, lnx64) on command line" 
      exit;
    fi
  fi
fi

make -f makefile.$OS acc_probe.so
$CVER -q +loadpli1=./acc_probe:pli1_compat_bootstrap probe.v >/dev/null
./rmlic.pl
diff verilog.log acc_probe.plg
rm acc_probe.so acc_probe.o 

make -f makefile.$OS acc_nxtchld.so
$CVER -q +loadpli1=./acc_nxtchld:pli1_compat_bootstrap nc_fdsp.v >/dev/null
./rmlic.pl
diff verilog.log acc_nxtchld.plg
rm acc_nxtchld.so acc_nxtchld.o

make -f makefile.$OS accxl_drvld.so
$CVER -q +loadpli1=./accxl_drvld:pli1_compat_bootstrap accxldrvtst.v >/dev/null
./rmlic.pl
diff verilog.log accxldrvtst.plg
rm accxl_drvld.so accxl_drvld.o 

make -f makefile.$OS acc_prtchg.so
$CVER -q +loadpli1=./acc_prtchg:pli1_compat_bootstrap pchg_fdsp.v >/dev/null
./rmlic.pl
diff verilog.log pchg_fdsp.plg
rm acc_prtchg.so acc_prtchg.o 

echo ">>>> acc_ test Finished - no diff differences should be printed."
echo " "
