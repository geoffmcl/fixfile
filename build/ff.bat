@setlocal
@set TMPEXE=Release\fixfile.exe
@if NOT EXIST %TMPEXE% (
@echo Can NOT locate %TMPEXE%! *** FIX ME ***
@exit /b 1
)

%TMPEXE% %*
