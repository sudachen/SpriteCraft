@echo off
rem rc.py
builder.py
for /F %%i in ('version 4 scraft.dll') do set BUILD=%%i
for /F %%i in ('version 1 scraft.dll') do set VERSION=%%i
set ZIPNAME=scraftdll.%VERSION%.%BUILD%
del /f/q %ZIPNAME%.zip 2>nul
gendoctmpl.py
gendoc.py en
zip %ZIPNAME%.zip scraft.dll scraft.en.htm
zip %ZIPNAME%.zip -r scraft.en.doc
