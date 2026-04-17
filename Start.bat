@echo off
cd /d "%~dp0"
set PATH=%~dp0libs;%PATH%
start "" "%~dp0Powietrze.exe"