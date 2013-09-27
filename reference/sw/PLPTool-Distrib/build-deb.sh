#!/bin/bash
if [ "$(id -u)" != "0" ]
then
  echo "This script must be run as root" 1>&2
  exit 1
fi

EXT_PREFIX="../PLPTool-Extensions"
EXT_FLOWCHART="$EXT_PREFIX/Flowchart/dist/Flowchart.jar"
EXT_CACHESIM="$EXT_PREFIX/CacheSim/dist/CacheSim.jar"
EXT_AUTOSAVER5="$EXT_PREFIX/AutoSaver5/dist/AutoSaver5.jar"
EXT_DEST="./deb/usr/lib/plptool5/extensions"

if [ ! -e ../PLPTool/store/PLPToolStatic.jar ]
then
  cd ../PLPTool
  ant package-for-store
  cd ../PLPTool-Distrib
fi

if [ -e ../PLPTool/store/PLPToolStatic.jar ]
then
  rm -rf ./deb/usr
  mkdir ./deb/usr
  mkdir ./deb/usr/bin
  mkdir ./deb/usr/lib
  mkdir ./deb/usr/lib/plptool5
  mkdir ./deb/usr/share
  mkdir ./deb/usr/share/plptool5
  mkdir $EXT_DEST
  cp -v ../PLPTool/store/PLPToolStatic.jar ./deb/usr/lib/plptool5
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
  echo "#!/bin/sh" > ./deb/usr/bin/plptool5
  echo "java -jar /usr/lib/plptool5/PLPToolStatic.jar -W -D /usr/lib/plptool5/extensions \$@" >> ./deb/usr/bin/plptool5
  cp -vr ../examples ./deb/usr/share/plptool5
  cp -vr ../libplp ./deb/usr/share/plptool5
  chown -R root:root ./deb/usr
  chmod a+x ./deb/usr/bin/plptool5
  dpkg --build deb
  mv -v deb.deb plptool5.deb
fi

