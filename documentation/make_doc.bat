@echo off
rem make_doc.bat - Windows version of make_doc.sh: generates the Gim documentation in html\
cd /d "%~dp0"

rem Graphviz (dot) is needed for the graphs: without it Doxygen skips them silently
if exist "C:\Program Files\Graphviz\bin\dot.exe" set "PATH=%PATH%;C:\Program Files\Graphviz\bin"

copy /Y ..\gim_gkfp\gkfp.cc . >nul

rem start from a clean html\ folder: old pages would stay mixed with the new ones
if exist html rmdir /s /q html

doxygen Doxyfile
if errorlevel 1 (
	echo Doxygen failed
	exit /b 1
)

copy /Y Logo_gKript_documentazione.png html\ >nul
copy /Y Logo_gKript_documentazione_small.png html\ >nul
copy /Y *.conf html\ >nul
copy /Y *.gkk html\ >nul
copy /Y *.cc html\ >nul

echo Documentation done !
