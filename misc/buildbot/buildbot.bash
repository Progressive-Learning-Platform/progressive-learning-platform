#!/bin/bash

cd $1/misc/buildbot
shift
DATE=`date +%y%m%d`

# assume we're in misc/buildbot and make a build repo in ../../../buildbot_tmp
# we also assume that NOBODY makes changes in the buildbot directory, i.e. it's always sane

#update the repo
echo ===============================================================================
echo - Making sure the repo is up to date!
echo ===============================================================================
hg pull
hg update

#copy the repo for building
BBOTPATH=`pwd`
cd ../../
mkdir ../buildbot_tmp
cp -R * ../buildbot_tmp 
cd ../buildbot_tmp
BUILDROOT=`pwd`

#we're in our temporary repo, let's build some things
echo ===============================================================================
echo - Building hardware...
echo ===============================================================================
cd $BUILDROOT/reference/hw
source /opt/Xilinx/13.1/ISE_DS/settings64.sh
cd bootrom
sh build.sh > bootrom_log
cd ..
sh scripts/build_500k_linux.sh > build_nexys2_500k_log
sh scripts/build_1200k_linux.sh > build_nexys2_1200k_log
sh scripts/build_nexys3_linux.sh > build_nexys3_log

#update the PLPTool nightly version
echo ===============================================================================
echo - Building PLPTool and its modules...
echo ===============================================================================
echo   - Updating Version string...
cd $BUILDROOT/reference/sw/PLPTool/src/plptool
sed -i "s/\"\(.*\)\"/\"\1-${DATE}\"/g" Version.java 

#build PLPTool
echo   - Building binary...
cd ../..
ant clean
ant package-for-store > build_plptool_log
echo   - Building javadoc...
ant javadoc > build_javadoc_log

#build the classroom module
echo   - Building lecture publisher...
cd $BUILDROOT/extras/LecturePublisher
ant clean
ant package-for-store > ../build_plptool_modules_log

#build the autosaver module
echo   - Building autosaver...
cd $BUILDROOT/extras/AutoSaver
ant clean
ant package-for-store >> ../build_plptool_modules_log

#run the autotest
echo ===============================================================================
echo - Commencing PLP auto-test...
echo ===============================================================================
cd $BUILDROOT/reference/sw 
yes | /usr/local/bin/djtgcfg prog -d Nexys2 -i 0 -f ../hw/build/build_500k_volatile/top.bit >> auto_test_log
sleep 10
java -jar PLPTool/store/PLPToolStatic.jar -plp examples/auto_test.plp -p /dev/ttyUSB0 >> auto_test_log
python /backup/fritz/scripts/auto_test.py /dev/ttyUSB0 >> auto_test_log

#everything should be built (or have failed), so get things ready to push to the website
echo ===============================================================================
echo - Pushing...
echo ===============================================================================
cd $BUILDROOT 
mkdir to_push
mkdir to_push/hw
mkdir to_push/plptool
cp reference/sw/auto_test_log to_push
cp reference/hw/*_log to_push
cp reference/hw/bootrom/*_log to_push
cp reference/hw/build/build_500k_volatile/top.bit to_push/hw/nexys2_500k.bit
cp reference/hw/build/build_500k_volatile/top.mcs to_push/hw/nexys2_500k.mcs
cp reference/hw/build/build_1200k_volatile/top.bit to_push/hw/nexys2_1200k.bit
cp reference/hw/build/build_1200k_volatile/top.mcs to_push/hw/nexys2_1200k.mcs
cp reference/hw/build/build_nexys3_volatile/top.bit to_push/hw/nexys3.bit

#build the Windows installer
echo ===============================================================================
echo   - Building PLP Tool Windows installer...
echo ===============================================================================
cd $BUILDROOT/extras/PLPToolInstaller
mkdir hardware
cp ../../to_push/hw/*.bit ./hardware
cp ../../to_push/hw/*.mcs ./hardware
./build_linuxhost.sh > build_plptoolinstall_log
cp build_plptoolinstall_log $BUILDROOT/to_push

echo ===============================================================================
echo   - Pushing sw...
echo ===============================================================================
cd $BUILDROOT
cp extras/*_log to_push
cp reference/sw/PLPTool/*_log to_push
cp reference/sw/PLPTool/store/* to_push/plptool
cp extras/LecturePublisher/store/* to_push/plptool
cp extras/AutoSaver/store/* to_push/plptool
cp extras/PLPToolInstaller/plptoolsetup.exe to_push/plptool
cp -R reference/sw/PLPTool/dist/javadoc to_push

#our data is ready to send off to the webserver
echo ===============================================================================
tar -czvf nightly-$DATE.tar.gz to_push/
mv nightly-$DATE.tar.gz $BBOTPATH/
ssh fritz@rome.ceat.okstate.edu "rm -rf plp/nightly/*"
scp -r to_push/* fritz@rome.ceat.okstate.edu:~/plp/nightly

#nuke the buildbot tmp
cd $BBOTPATH
rm -rf $BUILDROOT


