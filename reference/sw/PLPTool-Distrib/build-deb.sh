#!/bin/bash
echo "- PLPTool deb package builder"
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
EXT_DEST="./deb/usr/lib/plptool5/extensions"
WEB_DEST="./deb/usr/lib/plptool5/web"

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
  rm -rf ./deb/usr
  mkdir ./deb/usr
  mkdir ./deb/usr/bin
  mkdir ./deb/usr/lib
  mkdir ./deb/usr/lib/plptool5
  mkdir ./deb/usr/share
  mkdir ./deb/usr/share/plptool5
  mkdir $EXT_DEST
  mkdir $WEB_DEST
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
  if [ -e $EXT_SOCKETINTERFACE ]
  then
    cp -v $EXT_SOCKETINTERFACE $WEB_DEST
  fi
  cp -vr ../examples ./deb/usr/share/plptool5
  cp -vr ../libplp ./deb/usr/share/plptool5
  echo "- generating run script"
  echo "#!/bin/sh" > ./deb/usr/bin/plptool5
  echo "java -jar /usr/lib/plptool5/PLPToolStatic.jar -W -D /usr/lib/plptool5/extensions \$@" >> ./deb/usr/bin/plptool5
  chmod a+x ./deb/usr/bin/plptool5
  echo "- building .deb"
  if [ -z $FAKEROOT ]
  then
    chown -R root:root ./deb/usr
    dpkg --build deb
  else
    fakeroot dpkg --build deb
  fi
  mv -v deb.deb plptool5.deb
  echo "- done"
fi

