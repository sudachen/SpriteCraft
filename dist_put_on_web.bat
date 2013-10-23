@echo off
for /F %%i in ('..\..\My.Tools\version 4 scraft.dll') do set BUILD=%%i
for /F %%i in ('..\..\My.Tools\version 1 scraft.dll') do set VERSION=%%i
set ZIPNAME=scraftdll.%VERSION%.%BUILD%.zip
cd Setup

echo open www.suda-chen.biz > ftp-commands
echo builder@suda-chen.biz>> ftp-commands
echo kolosha96>> ftp-commands
echo passive >> ftp-commands
echo bin >> ftp-commands
echo put %ZIPNAME% >> ftp-commands
echo quit >> ftp-commands

ftp < ftp-commands

del ftp-commands
