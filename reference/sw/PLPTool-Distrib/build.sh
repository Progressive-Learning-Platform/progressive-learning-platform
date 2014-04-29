#!/bin/bash
ROOT=`pwd`
command -v ant  >/dev/null 2>&1 || { echo >&2 "'ant' is not installed.  Aborting."; exit 1; }
if [ ! -z "$ANT_HOME" ]
then
  ANT_LOCATION=$ANT_HOME/lib/ant.jar 
fi
if [ ! -e /usr/share/ant/lib/ant.jar ] && [ -z "$ANT_LOCATION" ]
then
  echo Can not find /usr/share/ant/lib/ant.jar. Please set ANT_LOCATION for manual location
  exit 1
elif [ ! -e "$ANT_LOCATION" ] && [ ! -z "$ANT_LOCATION" ]
then
  echo Can not find \'$ANT_LOCATION\'
  exit 1
elif [ -e /usr/share/ant/lib/ant.jar ] && [ -z "$ANT_LOCATION" ]
then
  ANT_LOCATION=/usr/share/ant/lib/ant.jar
fi
echo ant.jar: $ANT_LOCATION
echo Building PLPTool...
cd ../PLPTool
echo - clean
ant clean &> $ROOT/build.log
echo - generate-buildinfo
ant generate-buildinfo &> $ROOT/build.log
echo - package-for-store
ant package-for-store &>> $ROOT/build.log
if [ $? -ne 0 ]
then
  echo BUILD FAILED
  exit 1
fi
echo Building modtools...
cd modtools
javac -classpath ../store/PLPToolStatic.jar:$ANT_LOCATION EmbedManifestTask.java &>> $ROOT/build.log
if [ $? -ne 0 ]
then
  echo BUILD FAILED
fi
echo Building extensions...
echo - Flowchart
cd ../../PLPTool-Extensions/Flowchart
ant jar &>> $ROOT/build.log
if [ $? -ne 0 ]
then
  echo BUILD FAILED
fi
echo - CacheSim
cd ../CacheSim
ant jar &>> $ROOT/build.log
if [ $? -ne 0 ]
then
  echo BUILD FAILED
fi
echo - AutoSaver5
cd ../AutoSaver5
ant jar &>> $ROOT/build.log
if [ $? -ne 0 ]
then
  echo BUILD FAILED
fi
echo - SocketInterface
cd ../SocketInterface
ant jar &>> $ROOT/build.log
if [ $? -ne 0 ]
then
  echo BUILD FAILED
fi
cd $ROOT
echo Build done.
