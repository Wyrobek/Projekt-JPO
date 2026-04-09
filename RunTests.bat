@echo off
:: Przejdź do głównego folderu projektu przed uruchomieniem testów
cd /d "%~dp0"
set PATH=%~dp0libs;%PATH%
"%~dp0Tests\tests.exe"
pause