#!/bin/sh 
# install test procedures

# set $1 to your OS
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

make -f makefile.$OS async.so
$CVER -q +loadvpi=./async:vpi_compat_bootstrap async.v >/dev/null
./rmlic.pl
diff verilog.log async.plg
rm async.so async.o

make -f makefile.$OS vhello1.so
$CVER -q +loadvpi=./vhello1:vpi_compat_bootstrap vhello1.v >/dev/null
./rmlic.pl
diff verilog.log vhello1.plg
rm vhello1.so vhello1.o

make -f makefile.$OS vhello2.so
$CVER -q +loadvpi=./vhello2:vpi_compat_bootstrap vhello2.v >/dev/null
./rmlic.pl
diff verilog.log vhello2.plg
rm vhello2.so vhello2.o

make -f makefile.$OS vhelbad.so
$CVER -q +loadvpi=./vhelbad:vpi_compat_bootstrap vhelbad.v >/dev/null
./rmlic.pl
diff verilog.log vhelbad.plg
rm vhelbad.so vhelbad.o

make -f makefile.$OS findcaus.so
$CVER -q +loadvpi=./findcaus:vpi_compat_bootstrap task10.v >/dev/null
./rmlic.pl
diff verilog.log findcaus.plg
rm findcaus.so findcaus.o

make -f makefile.$OS vacbtst.so
$CVER -q +loadvpi=./vacbtst:vpi_compat_bootstrap -s -i vacbtst.inp task10.v >/dev/null
./rmlic.pl
diff verilog.log vacbtst.plg
rm vacbtst.so vacbtst.o

make -f makefile.$OS vprtchg.so
$CVER -q +loadvpi=./vprtchg:vpi_compat_bootstrap  task10.v >/dev/null
./rmlic.pl
diff verilog.log vprtchg.plg
rm vprtchg.so vprtchg.o 

make -f makefile.$OS vprtchg2.so
$CVER -q +loadvpi=./vprtchg2:vpi_compat_bootstrap task10.v >/dev/null
./rmlic.pl
diff verilog.log vprtchg2.plg
rm vprtchg2.so vprtchg2.o

make -f makefile.$OS vprtchg3.so
$CVER -q +loadvpi=./vprtchg3:vpi_compat_bootstrap task10.v >/dev/null
./rmlic.pl
diff verilog.log vprtchg3.plg
rm vprtchg3.so vprtchg3.o

make -f makefile.$OS vprtdels.so
$CVER -q +loadvpi=./vprtdels:vpi_compat_bootstrap fdspec01.v >/dev/null
./rmlic.pl
diff verilog.log vprtdels.plg
rm vprtdels.so vprtdels.o

make -f makefile.$OS vprtdel2.so
$CVER -q +loadvpi=./vprtdel2:vpi_compat_bootstrap timtst03.v >/dev/null
./rmlic.pl
diff verilog.log vprtdel2.plg
rm vprtdel2.so vprtdel2.o

make -f makefile.$OS vsetdels.so
$CVER -q +loadvpi=./vsetdels:vpi_compat_bootstrap fdspec01.v >/dev/null
./rmlic.pl
diff verilog.log vsetdels.plg
rm vsetdels.so vsetdels.o

make -f makefile.$OS vsetval1.so
$CVER -q +loadvpi=./vsetval1:vpi_compat_bootstrap vsetval1.v >/dev/null
./rmlic.pl
diff verilog.log vsetval1.plg
rm vsetval1.so vsetval1.o 

make -f makefile.$OS vsetval2.so
$CVER -q +loadvpi=./vsetval2:vpi_compat_bootstrap vsetval2.v >/dev/null
./rmlic.pl
diff verilog.log vsetval2.plg
rm vsetval2.so vsetval2.o

make -f makefile.$OS vtimcbs.so
$CVER -q +loadvpi=./vtimcbs:vpi_compat_bootstrap task10.v >/dev/null
./rmlic.pl
diff verilog.log vtimcbs.plg
rm vtimcbs.so vtimcbs.o 

make -f makefile.$OS vfopen1.so
$CVER -q +loadvpi=./vfopen1:vpi_compat_bootstrap vfopen1.v >/dev/null
./rmlic.pl
diff verilog.log vfopen1.plg
diff vpifout.fil vpifout.xfl
rm vfopen1.so vfopen1.o

make -f makefile.$OS vfopen2.so
$CVER -q +loadvpi=./vfopen2:vpi_compat_bootstrap vfopen2.v >/dev/null
./rmlic.pl
diff verilog.log vfopen2.plg
diff vpitout.fil vpitout.xfl
rm vfopen2.so vfopen2.o

make -f makefile.$OS vconta1.so
$CVER -q +loadvpi=./vconta1:vpi_compat_bootstrap cacatmd1.v >/dev/null
./rmlic.pl
diff verilog.log vconta1.plg
rm vconta1.so vconta1.o

make -f makefile.$OS vchkprt1.so
$CVER -q +loadvpi=./vchkprt1:vpi_compat_bootstrap prtbg09.v >/dev/null
# cannot compare verilog.log because has vpi_get_vlog_info
diff fff9 fff9.exp
rm vchkprt1.so vchkprt1.o

make -f makefile.$OS vdrvld1.so
$CVER -q +loadvpi=./vdrvld1:vpi_compat_bootstrap fdspec01.v >/dev/null
./rmlic.pl
diff verilog.log vdrvld1.plg
rm vdrvld1.so vdrvld1.o

make -f makefile.$OS vdrvld2.so
$CVER -q +loadvpi=./vdrvld2:vpi_compat_bootstrap cacatmd1.v >/dev/null
./rmlic.pl
diff verilog.log vdrvld2.plg
rm vdrvld2.so vdrvld2.o

make -f makefile.$OS dfpsetd.so
$CVER -q +loadvpi=./dfpsetd:vpi_compat_bootstrap dfpsetd.v >/dev/null
./rmlic.pl
diff verilog.log dfpsetd.plg
rm dfpsetd.so dfpsetd.o

echo ">>>> PLI 2.0 Install test Finished - no diff differences should be printed."
echo " "
