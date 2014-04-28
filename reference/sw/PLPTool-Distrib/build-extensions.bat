@ECHO OFF
set WORKDIR=%CD%
echo WORKDIR: %WORKDIR%
IF "%JAVA_HOME%" == "" (
	ECHO JAVA_HOME is undefined
	GOTO Exit
)
IF "%ANT_HOME%" == "" (
	ECHO ANT_HOME is undefined
	GOTO Exit
)
echo JAVA_HOME: %JAVA_HOME%
echo ANT_HOME: %ANT_HOME%
cd ..\PLPTool
echo Building PLPTool modtools...
ECHO - EmbedManifestTask
cd modtools
javac -cp "%ANT_HOME%"\lib\ant.jar;..\store\PLPToolStatic.jar EmbedManifestTask.java >> %WORKDIR%\winbuild.log
cd ..
ECHO Building Extensions...
ECHO - Flowchart
cd ..\PLPTool-Extensions\Flowchart
call  "%ANT_HOME%"\bin\ant jar >> %WORKDIR%\winbuild.log
ECHO - CacheSim
cd ..\CacheSim
call  "%ANT_HOME%"\bin\ant jar >> %WORKDIR%\winbuild.log
ECHO - AutoSaver5
cd ..\AutoSaver5
call  "%ANT_HOME%"\bin\ant jar >> %WORKDIR%\winbuild.log
ECHO - SocketInterface
cd ..\SocketInterface
call  "%ANT_HOME%"\bin\ant jar >> %WORKDIR%\winbuild.log
ECHO - WebService
cd ..\WebService
call  "%ANT_HOME%"\bin\ant jar >> %WORKDIR%\winbuild.log
cd %WORKDIR%
:Exit
ECHO Exit
