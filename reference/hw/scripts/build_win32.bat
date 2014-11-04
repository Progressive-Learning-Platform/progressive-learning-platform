@echo off
cd ..\build
xcopy /E /I build_500k_skeleton build_volatile
cd build_volatile

rem ===========================================
rem Set enviromental variables (envvar)
rem ===========================================

rem Find the base Xilinx directory by finding "ise.exe"
rem for /f "tokens=*" %%a in ('dir C:\Xilinx\ise.exe /B /P /S') do (set XILINX=%%a)

rem trim path to base Xilinx directory for XILINX envvar
rem @set XILINX=%XILINX:~0,-15%
@set XILINX=C:\Xilinx\14.4\ISE_DS\ISE

rem Add Xilinx bin and lib paths to PATH envvar
@set PATH=%XILINX%\lib\nt;%XILINX%\bin\nt;%PATH%

rem ===========================================
rem Create Verilog project list
rem ===========================================

rem Remove old .prj file
del top.prj

rem Create new .prj file
echo Creating new project file
for /F "tokens=*" %%G in ('DIR ..\..\verilog /B /P /S') do (
echo verilog work "%%G"
echo verilog work "%%G" >> top.prj
)

rem ===========================================
rem Create programming file
rem ===========================================
xst      -intstyle silent -ifn top.xst -ofn top.syr
ngdbuild -intstyle silent -dd _ngo -nt timestamp -uc top.ucf -p xc3s500e-fg320-4 "top.ngc" top.ngd
map      -intstyle silent -p xc3s500e-fg320-4 -cm area -ir off -pr off -c 100 -o top_map.ncd top.ngd top.pcf
par      -intstyle silent -w -ol high -t 1 top_map.ncd top.ncd top.pcf
trce     -intstyle silent -v 3 -s 4 -n 3 -fastpaths -xml top.twx top.ncd -o top.twr top.pcf
bitgen   -intstyle silent -f top.ut top.ncd
promgen -w -c FF -x xcf04s -p mcs -o top -u 0 top.bit 

cd ..\..
