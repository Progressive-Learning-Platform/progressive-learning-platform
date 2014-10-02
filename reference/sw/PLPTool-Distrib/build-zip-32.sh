#!/bin/bash
echo "- PLPTool 32-bit zip package builder"
if [ "$(id -u)" != "0" ] && [ -z "$(command -v fakeroot)" ]
then
  echo "fakeroot must be installed or this script must be run as root" 1>&2
  exit 1
fi
if [ ! -z "$(command -v fakeroot)" ]
then
  echo "- using fakeroot"
  FAKEROOT=1
fi

EXT_PREFIX="../PLPTool-Extensions"
EXT_FLOWCHART="$EXT_PREFIX/Flowchart/dist/Flowchart.jar"
EXT_CACHESIM="$EXT_PREFIX/CacheSim/dist/CacheSim.jar"
EXT_AUTOSAVER5="$EXT_PREFIX/AutoSaver5/dist/AutoSaver5.jar"
EXT_SOCKETINTERFACE="$EXT_PREFIX/SocketInterface/dist/SocketInterface.jar"
EXT_DEST="./plptool5/extensions"
WEB_DEST="./plptool5/web"

if [ ! -e ../PLPTool/store/PLPToolStatic.jar ]
then
  echo "- PLPTool static package not found, building"
  cd ../PLPTool
  ant package-for-store
  cd ../PLPTool-Distrib
fi

if [ -e ../PLPTool/store/PLPToolStatic.jar ]
then
  echo "- copying files to include in the package"
  rm -rf ./plptool5
  mkdir ./plptool5
  mkdir $EXT_DEST
  mkdir $WEB_DEST
  cp -v ../PLPTool/store/PLPToolStatic.jar ./plptool5
  if [ -e $EXT_FLOWCHART ]
  then
    cp -v $EXT_FLOWCHART $EXT_DEST
  fi
  if [ -e $EXT_CACHESIM ]
  then
    cp -v $EXT_CACHESIM $EXT_DEST
  fi
  if [ -e $EXT_AUTOSAVER5 ]
  then
    cp -v $EXT_AUTOSAVER5 $EXT_DEST
  fi
  if [ -e $EXT_SOCKETINTERFACE ]
  then
    cp -v $EXT_SOCKETINTERFACE $WEB_DEST
  fi
  cp -vr ../examples ./plptool5
  cp -vr ../libplp ./plptool5
  cp ../PLPTool/store/rxtxSerial32.dll ./plptool5/rxtxSerial.dll
  echo "- generating run script"
  echo "#!/bin/sh" > ./plptool5/run.sh
  echo "java -jar /usr/lib/plptool5/PLPToolStatic.jar -W -D ./extensions \$@" >> ./plptool5/run.sh
  chmod a+x ./plptool5/run.sh
  echo "java -Djava.library.path=. -jar PLPToolStatic.jar -W -D .\\extensions %1 %2 %3 %4 %5 %6 %7 %8 %9" > ./plptool5/run-windows.bat
  echo "- packing into zip file"
  zip -r plptool5-32bit.zip plptool5
  echo "- done"
fi

