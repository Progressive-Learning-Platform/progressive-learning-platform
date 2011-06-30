#!/bin/sh 
# install test procedures

CVER="../../bin/cver -q " 

if test ! -f ../../bin/cver 
then
  echo "ERROR - there is no cver in ../../bin/"
  echo "Must make in src/ directory" 
  exit;
fi

$CVER arithtest.v >/dev/null
../install.tst/rmlic.pl
diff verilog.log arithtest.plg

$CVER comparetest.v >/dev/null
../install.tst/rmlic.pl
diff verilog.log comparetest.plg

$CVER dsignedtest.v >/dev/null
../install.tst/rmlic.pl
diff verilog.log dsignedtest.plg

$CVER dsign.v >/dev/null
../install.tst/rmlic.pl
diff verilog.log dsign.plg

$CVER fgets1.v >flog
mv flog verilog.log
../install.tst/rmlic.pl
diff verilog.log fgets1.plg

$CVER getc1.v >flog
mv flog verilog.log
../install.tst/rmlic.pl
diff verilog.log getc1.plg

$CVER getc2.v >flog
mv flog verilog.log
../install.tst/rmlic.pl
diff verilog.log getc2.plg

$CVER lofptst.v >/dev/null
../install.tst/rmlic.pl
diff verilog.log lofptst.plg

echo ">>>> v2001 test completed (this should be only message printed)."
echo " "
