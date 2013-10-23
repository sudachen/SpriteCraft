@echo off
rc.py
rem ..\..\My.Tools\builder.py
call dist_make_dll.bat
for /F %%i in ('..\..\My.Tools\version 4 scraft.dll') do set BUILD=%%i
for /F %%i in ('..\..\My.Tools\version 1 scraft.dll') do set VERSION=%%i
set ZIPNAME=scraftdll.%VERSION%.%BUILD%.zip
copy %ZIPNAME% Setup\%ZIPNAME%
del /f %ZIPNAME%
cd Setup
rem svn add %ZIPNAME%
rem svn commit -m '' %ZIPNAME%
