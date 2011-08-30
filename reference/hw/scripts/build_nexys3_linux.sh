#!/bin/sh

#assumes that you've already sourced the xilinx shrc file. on the buildbot system, it goes something like:
# source /opt/Xilinx/12.3/ISE_DS/settings64.sh 

#for the vga bug
export XIL_PAR_KEEP_BRAM_MULT_APART=1

cd build
cp -R build_nexys3_skeleton build_nexys3_volatile
cd build_nexys3_volatile

echo "==========================================="
echo "Create Verilog project list"
echo "==========================================="

rm top.prj

for i in `ls ../../verilog/*.v`
do
	echo "verilog work ${i}" >> top.prj
done

echo "==========================================="
echo "Create programming file"
echo "==========================================="
xst      -intstyle xflow -ifn top.xst -ofn top.syr
ngdbuild -intstyle xflow -dd _ngo -nt timestamp -uc top.ucf -p xc6slx16-csg324-3 "top.ngc" top.ngd
map      -intstyle xflow -p xc6slx16-csg324-3 -cm area -ir off -pr off -c 100 -o top_map.ncd top.ngd top.pcf
par      -intstyle xflow -w -ol high -t 1 top_map.ncd top.ncd top.pcf
trce     -intstyle xflow -v 3 -s 4 -n 3 -fastpaths -xml top.twx top.ncd -o top.twr top.pcf
bitgen   -intstyle xflow -f top.ut top.ncd
promgen -w -c FF -x xcf04s -p mcs -o top -u 0 top.bit 


