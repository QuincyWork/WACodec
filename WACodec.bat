echo off
REM 
REM 	WACodec Bat
REM
set PWD=%~dp0
cd /d %PWD%

lua.exe WACodec.lua %1
echo Decode Successful
echo Output file:%1.out
echo on