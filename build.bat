@echo off

REM DEFINE FILES, PATHS AND LIBRARIES
set SRC=chart.c
set SRC=%SRC% libs/glad/include/glad/glad.c
set OUTPUT=main.exe

set INCS=/I"libs/glad/include" /I"libs/freetype/include" /I"libs/glfw/include"
set LIBPATH=/LIBPATH:"libs/glfw/lib" /LIBPATH:"libs/freetype"
set LIBS=opengl32.lib glfw3_mt.lib freetype.lib Shell32.lib User32.lib Gdi32.lib

REM *** RELEASE BUILD ***
REM cl /nologo %INCS% %SRC% /link /SUBSYSTEM:WINDOWS %LIBPATH% %LIBS% /OUT:%OUTPUT%

REM *** DEBUG BUILD ***
cl /Zi /fsanitize=address %INCS% %SRC% /link /SUBSYSTEM:CONSOLE /DEBUG:FULL %LIBPATH% %LIBS% /OUT:%OUTPUT%

REM Check if the build was successful
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b %ERRORLEVEL%
)
