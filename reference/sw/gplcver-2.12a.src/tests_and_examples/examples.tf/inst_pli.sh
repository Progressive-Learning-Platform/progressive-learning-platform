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

make -f makefile.$OS probe.so
$CVER -q +loadpli1=./probe:pli1_compat_bootstrap probe.v >/dev/null
./rmlic.pl
diff verilog.log probe.plg
rm probe.so probe.o

make -f makefile.$OS tfclk.so
$CVER -q tfclk.v +loadpli1=./tfclk:pli1_compat_bootstrap >/dev/null
./rmlic.pl
diff verilog.log tfclk.plg
rm tfclk.so tfclk.o

make -f makefile.$OS plimfil.so
$CVER +memfile+testmem.dat -q +loadpli1=./plimfil:pli1_compat_bootstrap plimfil.v >/dev/null
./rmlic.pl
diff verilog.log plimfil.plg
rm plimfil.so plimfil.o

make -f makefile.$OS plimfil2.so
$CVER +memfile+testmem2.dat -q +loadpli1=./plimfil2:pli1_compat_bootstrap plimfil2.v >/dev/null
./rmlic.pl
diff verilog.log plimfil2.plg
rm plimfil2.so plimfil2.o

echo ">>>> PLI 1.0 test finished - no diff differences should be printed."
echo " "
