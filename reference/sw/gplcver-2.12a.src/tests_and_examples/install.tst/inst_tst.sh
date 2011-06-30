#!/bin/sh 
# install test procedures

CVER="../../bin/cver -q " 

if test ! -f ../../bin/cver 
then
  echo "ERROR - there is no cver in ../../bin/"
  echo "Must make in src/ directory" 
  exit;
fi

$CVER instid.v >/dev/null
./rmlic.pl
diff verilog.log instid.plg

$CVER instpnd3.v >/dev/null
./rmlic.pl
diff verilog.log instpnd3.plg

$CVER aspike1.v >/dev/null
./rmlic.pl
diff verilog.log aspike1.plg
$CVER +warn_canceled_e aspike1.v >/dev/null
./rmlic.pl
diff verilog.log aspike1a.plg
$CVER +show_canceled_e aspike1.v >/dev/null
./rmlic.pl
diff verilog.log aspike1b.plg
$CVER +show_canceled_e +pulse_e_style_ondetect aspike1.v >/dev/null
./rmlic.pl
diff verilog.log aspike1c.plg
$CVER aspike1.v -et >/dev/null
./rmlic.pl
diff verilog.log aspike1d.plg

$CVER +warn_canceled_e -v JK_Q.v -v JK_QBAR.v udpjkff.v >/dev/null
./rmlic.pl
diff verilog.log udpjkff.plg

$CVER xx2bdel.v >/dev/null
./rmlic.pl
diff verilog.log xx2bdel.plg
$CVER xx2bpth.v >/dev/null
./rmlic.pl
diff verilog.log xx2bpth.plg
$CVER xx2bpth2.v >/dev/null
./rmlic.pl
diff verilog.log xx2bpth2.plg

$CVER c880.v >/dev/null
./rmlic.pl
diff verilog.log c880.plg

$CVER -i force01.inp force01.v >/dev/null
./rmlic.pl
diff verilog.log force01.plg

$CVER smrd04.v -f smrd04.vc >/dev/null
./rmlic.pl
diff verilog.log smrd04.plg

$CVER gatenots.v >/dev/null
./rmlic.pl
diff verilog.log gatenots.plg

$CVER arms_sim.v armscnt.v >/dev/null
./rmlic.pl
diff verilog.log arms.plg

$CVER -f dfpsetd.vc >/dev/null
./rmlic.pl
diff verilog.log dfpsetd.plg

$CVER -f mipdnot1.vc >/dev/null
./rmlic.pl
diff verilog.log mipdnot1.plg

$CVER -f sdfia04.vc >/dev/null
./rmlic.pl
diff verilog.log sdfia04.plg

$CVER dffn.v >/dev/null
./rmlic.pl
diff verilog.log dffn.plg

$CVER xplipnd.v >/dev/null
./rmlic.pl
diff verilog.log xplipnd.plg

$CVER defsplt1.v >/dev/null
./rmlic.pl
diff verilog.log defsplt1.plg

echo ">>>> Install test completed (this should be only message printed)."
echo " "
