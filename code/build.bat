@echo off

SET FLAGS=/Od /Zi /FC /Fe:"2dengine" /wd4668 /wd5039 /wd4100
SET LIBS=gdi32.lib user32.lib kernel32.lib winmm.lib

IF NOT EXIST .\build mkdir .\build
pushd ..\build
cl /EHcs ..\code\win32_venus2d.cpp %FLAGS% /link %LIBS% /SUBSYSTEM:WINDOWS
popd