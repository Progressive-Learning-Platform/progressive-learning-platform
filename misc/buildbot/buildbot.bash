#!/bin/bash

DATE=`date +%y%m%d`

# we assume that /backup/fritz/buildbot has a valid hg repo in it
# we also assume that NOBODY makes changes in the buildbot directory, i.e. it's always sane

#update the repo
cd /backup/fritz/buildbot
hg pull
hg update

#copy the repo for building
cd /backup/fritz
cp -R /backup/fritz/buildbot /backup/fritz/buildbot_tmp
cd /backup/fritz/buildbot_tmp

#we're in our temporary repo, let's build some things
cd reference/hw
source /opt/Xilinx/13.1/ISE_DS/settings64.sh
cd bootrom
sh build.sh > bootrom_log
cd ..
sh scripts/build_500k_linux.sh > build_nexys2_500k_log
sh scripts/build_1200k_linux.sh > build_nexys2_1200k_log
sh scripts/build_nexys3_linux.sh > build_nexys3_log

#update the PLPTool nightly version
cd ../sw/PLPTool/src/plptool
sed -i "s/\"\(.*\)\"/\"\1-${DATE}\"/g" Version.java 

#build PLPTool
cd ../..
ant clean
ant package-for-store > build_plptool_log
ant javadoc > build_javadoc_log

#run the autotext
cd /backup/fritz/buildbot_tmp/reference/sw
yes | /usr/local/bin/djtgcfg prog -d Nexys2 -i 0 -f ../hw/build/build_500k_volatile/top.bit >> auto_test_log
sleep 10
java -jar PLPTool/store/PLPToolStatic.jar -plp examples/auto_test.plp -p /dev/ttyUSB0 >> auto_test_log
python /backup/fritz/scripts/auto_test.py /dev/ttyUSB0 >> auto_test_log

#everything should be built (or have failed), so get things ready to push to the website
cd /backup/fritz/buildbot_tmp
mkdir to_push
cp reference/sw/auto_test_log to_push
cp reference/hw/*_log to_push
cp reference/hw/bootrom/*_log to_push
cp reference/hw/build/build_500k_volatile/top.bit to_push/nexys2_500k.bit
cp reference/hw/build/build_500k_volatile/top.mcs to_push/nexys2_500k.mcs
cp reference/hw/build/build_1200k_volatile/top.bit to_push/nexys2_1200k.bit
cp reference/hw/build/build_1200k_volatile/top.mcs to_push/nexys2_1200k.mcs
cp reference/hw/build/build_nexys3_volatile/top.bit to_push/nexys3.bit
cp reference/sw/PLPTool/*_log to_push
cp reference/sw/PLPTool/store/* to_push
cp -R reference/sw/PLPTool/dist/javadoc to_push

#our data is ready to send off to the webserver
ssh fritz@rome.ceat.okstate.edu "rm -rf plp/nightly/*"
scp -r to_push/* fritz@rome.ceat.okstate.edu:~/plp/nightly

#nuke the buildbot tmp
cd /backup/fritz
rm -rf buildbot_tmp


