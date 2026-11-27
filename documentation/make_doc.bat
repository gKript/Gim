@echo off
rem make_doc.bat - entry point of the documentation on Windows: gkmake runs this file when it exists
rem (see gkmake/main.cc, documentation_command). The modern look (cards, gim_modern.css) is built by
rem make_doc_modern.bat, so this file only delegates to it. Output: html\index.html
call "%~dp0make_doc_modern.bat"
exit /b %errorlevel%
