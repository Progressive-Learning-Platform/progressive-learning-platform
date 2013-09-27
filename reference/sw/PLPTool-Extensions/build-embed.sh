#!/bin/bash
if [ -e dist ]
then
	rm -rf dist
fi
mkdir dist
echo Building PLPTool...
cd ../PLPTool
ant jar &> ./dist/build-log
ant package-for-store &>> ./dist/build-log
cd ../PLPTool-Extensions
echo Building Flowchart Extension...
cd Flowchart
ant jar &>> ./dist/build-log
cd ..
java -jar ../PLPTool/store/PLPToolStatic.jar --embed-manifest ./Flowchart/dist/Flowchart.jar "Flowchart Generator" "PLP Contributors" "GPLv3" "PLPTool extension to generate flowchart for PLPCPU programs. Requires an installation of GraphViz." "1.0" &>> ./dist/build-log
echo Building CacheSim Extension...
cd CacheSim
ant jar &>> ./dist/build-log
cd ..
java -jar ../PLPTool/store/PLPToolStatic.jar --embed-manifest ./CacheSim/dist/CacheSim.jar "Cache Simulator" "PLP Contributors" "GPLv3" "PLPTool extension to simulate a cache while the program is being executed" "1.0" &>> ./dist/build-log
echo Building AutoSaver5 Extension...
cd AutoSaver5
ant jar &>> ./dist/build-log
cd ..
java -jar ../PLPTool/store/PLPToolStatic.jar --embed-manifest ./AutoSaver5/dist/AutoSaver5.jar "Project AutoSaver" "PLP Contributors" "GPLv3" "Saves the currently open project periodically to allow recovery if PLPTool is not shut down properly" "1.0" &>> ./dist/build-log
echo Building Classroom Extension...
cd Classroom
ant jar &>> ./dist/build-log
cd ..
java -jar ../PLPTool/store/PLPToolStatic.jar --embed-manifest ./Classroom/dist/Classroom.jar "Classroom Collaboration" "PLP Contributors" "GPLv3" "Provides Classroom collaboration features to PLPTool" "1.0" &>> ./dist/build-log
cp ./Flowchart/dist/Flowchart.jar ./dist
cp ./CacheSim/dist/CacheSim.jar ./dist
cp ./AutoSaver5/dist/AutoSaver5.jar ./dist
cp ./Classroom/dist/Classroom.jar ./dist

