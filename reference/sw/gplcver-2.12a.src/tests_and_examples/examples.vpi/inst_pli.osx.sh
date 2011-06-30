#!/bin/sh 
# install test procedures

OS=OSX
CVER=../../bin/cver

if test ! -f ../../bin/cver 
then
  echo "ERROR - there is no cver in ../../bin/"
  echo "Must make in src/ directory" 
  exit;
fi

make -f makefile.$OS async.dylib
$CVER -q +loadvpi=./async:vpi_compat_bootstrap async.v >/dev/null
./rmlic.pl
diff verilog.log async.plg
rm async.dylib async.o

make -f makefile.$OS vhello1.dylib
$CVER -q +loadvpi=./vhello1:vpi_compat_bootstrap vhello1.v >/dev/null
./rmlic.pl
diff verilog.log vhello1.plg
rm vhello1.dylib vhello1.o

make -f makefile.$OS vhello2.dylib
$CVER -q +loadvpi=./vhello2:vpi_compat_bootstrap vhello2.v >/dev/null
./rmlic.pl
diff verilog.log vhello2.plg
rm vhello2.dylib vhello2.o

make -f makefile.$OS vhelbad.dylib
$CVER -q +loadvpi=./vhelbad:vpi_compat_bootstrap vhelbad.v >/dev/null
./rmlic.pl
diff verilog.log vhelbad.plg
rm vhelbad.dylib vhelbad.o

make -f makefile.$OS findcaus.dylib
$CVER -q +loadvpi=./findcaus:vpi_compat_bootstrap task10.v >/dev/null
./rmlic.pl
diff verilog.log findcaus.plg
rm findcaus.dylib findcaus.o

make -f makefile.$OS vacbtst.dylib
$CVER -q +loadvpi=./vacbtst:vpi_compat_bootstrap -s -i vacbtst.inp task10.v >/dev/null
./rmlic.pl
diff verilog.log vacbtst.plg
rm vacbtst.dylib vacbtst.o

make -f makefile.$OS vprtchg.dylib
$CVER -q +loadvpi=./vprtchg:vpi_compat_bootstrap  task10.v >/dev/null
./rmlic.pl
diff verilog.log vprtchg.plg
rm vprtchg.dylib vprtchg.o 

make -f makefile.$OS vprtchg2.dylib
$CVER -q +loadvpi=./vprtchg2:vpi_compat_bootstrap task10.v >/dev/null
./rmlic.pl
diff verilog.log vprtchg2.plg
rm vprtchg2.dylib vprtchg2.o

make -f makefile.$OS vprtchg3.dylib
$CVER -q +loadvpi=./vprtchg3:vpi_compat_bootstrap task10.v >/dev/null
./rmlic.pl
diff verilog.log vprtchg3.plg
rm vprtchg3.dylib vprtchg3.o

make -f makefile.$OS vprtdels.dylib
$CVER -q +loadvpi=./vprtdels:vpi_compat_bootstrap fdspec01.v >/dev/null
./rmlic.pl
diff verilog.log vprtdels.plg
rm vprtdels.dylib vprtdels.o

make -f makefile.$OS vprtdel2.dylib
$CVER -q +loadvpi=./vprtdel2:vpi_compat_bootstrap timtst03.v >/dev/null
./rmlic.pl
diff verilog.log vprtdel2.plg
rm vprtdel2.dylib vprtdel2.o

make -f makefile.$OS vsetdels.dylib
$CVER -q +loadvpi=./vsetdels:vpi_compat_bootstrap fdspec01.v >/dev/null
./rmlic.pl
diff verilog.log vsetdels.plg
rm vsetdels.dylib vsetdels.o

make -f makefile.$OS vsetval1.dylib
$CVER -q +loadvpi=./vsetval1:vpi_compat_bootstrap vsetval1.v >/dev/null
./rmlic.pl
diff verilog.log vsetval1.plg
rm vsetval1.dylib vsetval1.o 

make -f makefile.$OS vsetval2.dylib
$CVER -q +loadvpi=./vsetval2:vpi_compat_bootstrap vsetval2.v >/dev/null
./rmlic.pl
diff verilog.log vsetval2.plg
rm vsetval2.dylib vsetval2.o

make -f makefile.$OS vtimcbs.dylib
$CVER -q +loadvpi=./vtimcbs:vpi_compat_bootstrap task10.v >/dev/null
./rmlic.pl
diff verilog.log vtimcbs.plg
rm vtimcbs.dylib vtimcbs.o 

make -f makefile.$OS vfopen1.dylib
$CVER -q +loadvpi=./vfopen1:vpi_compat_bootstrap vfopen1.v >/dev/null
./rmlic.pl
diff verilog.log vfopen1.plg
diff vpifout.fil vpifout.xfl
rm vfopen1.dylib vfopen1.o

make -f makefile.$OS vfopen2.dylib
$CVER -q +loadvpi=./vfopen2:vpi_compat_bootstrap vfopen2.v >/dev/null
./rmlic.pl
diff verilog.log vfopen2.plg
diff vpitout.fil vpitout.xfl
rm vfopen2.dylib vfopen2.o

make -f makefile.$OS vconta1.dylib
$CVER -q +loadvpi=./vconta1:vpi_compat_bootstrap cacatmd1.v >/dev/null
./rmlic.pl
diff verilog.log vconta1.plg
rm vconta1.dylib vconta1.o

make -f makefile.$OS vchkprt1.dylib
$CVER -q +loadvpi=./vchkprt1:vpi_compat_bootstrap prtbg09.v >/dev/null
# cannot compare verilog.log because has vpi_get_vlog_info
diff fff9 fff9.exp
rm vchkprt1.dylib vchkprt1.o

make -f makefile.$OS vdrvld1.dylib
$CVER -q +loadvpi=./vdrvld1:vpi_compat_bootstrap fdspec01.v >/dev/null
./rmlic.pl
diff verilog.log vdrvld1.plg
rm vdrvld1.dylib vdrvld1.o

make -f makefile.$OS vdrvld2.dylib
$CVER -q +loadvpi=./vdrvld2:vpi_compat_bootstrap cacatmd1.v >/dev/null
./rmlic.pl
diff verilog.log vdrvld2.plg
rm vdrvld2.dylib vdrvld2.o

make -f makefile.$OS dfpsetd.dylib
$CVER -q +loadvpi=./dfpsetd:vpi_compat_bootstrap dfpsetd.v >/dev/null
./rmlic.pl
diff verilog.log dfpsetd.plg
rm dfpsetd.dylib dfpsetd.o

echo ">>>> PLI 2.0 OSX Install test Finished - no diff differences should be printed."
echo " "
