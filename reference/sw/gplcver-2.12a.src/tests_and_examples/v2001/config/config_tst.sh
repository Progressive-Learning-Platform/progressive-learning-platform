#!/bin/sh 
# install test procedures

CVER="../../../bin/cver -q +config" 

if test ! -f ../../../bin/cver 
then
  echo "ERROR - there is no cver in ../../../bin/"
  echo "Must make in src/ directory" 
  exit;
fi

$CVER config1.map >/dev/null
../../install.tst/rmlic.pl
diff verilog.log config1.plg

$CVER config2.map >/dev/null
../../install.tst/rmlic.pl
diff verilog.log config2.plg

$CVER config3.map >/dev/null
../../install.tst/rmlic.pl
diff verilog.log config3.plg

$CVER config4.map >/dev/null
../../install.tst/rmlic.pl
diff verilog.log config4.plg

$CVER config5.map >/dev/null
../../install.tst/rmlic.pl
diff verilog.log config5.plg

echo ">>>> v2001 test completed (this should be only message printed)."
echo " "
