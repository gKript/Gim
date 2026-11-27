@echo off
rem ---------------------------------------------------------------------------
rem  Gim documentation - modern skin (Doxygen 1.18)
rem  Same look as the PicGIM documentation: gim_modern.css is loaded by Doxygen
rem  itself (HTML_EXTRA_STYLESHEET), cards are added by postprocess_cards.ps1.
rem  Output: html\index.html
rem ---------------------------------------------------------------------------
cd /d "%~dp0"

where doxygen >nul 2>&1 || set "PATH=E:\GitHub Asy\Tools portable\Documentazione\doxygen-1.18.0;%PATH%"
for /f "tokens=1" %%v in ('doxygen --version') do set DOXYVER=%%v
echo Doxygen %DOXYVER%

rem Graphviz (dot) is needed for the graphs: without it Doxygen skips them silently
if exist "C:\Program Files\Graphviz\bin\dot.exe" set "PATH=%PATH%;C:\Program Files\Graphviz\bin"

rem Start from a clean html\ folder: old pages would stay mixed with the new ones
if exist html rmdir /S /Q html

copy /Y ..\gim_gkfp\gkfp.cc . >nul 2>&1

doxygen Doxyfile
if errorlevel 1 (
	echo Doxygen failed.
	exit /b 1
)

copy /Y *.png html\ >nul
copy /Y *.gif html\ >nul
copy /Y *.conf html\ >nul
copy /Y *.gkk html\ >nul
copy /Y *.cc html\ >nul

rem Cards: wrap every section in a card (remove this line to get the plain look)
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0postprocess_cards.ps1" -HtmlDir "%~dp0html"

echo %DOXYVER%> html\doxygen_version.txt
echo Done: html\index.html
