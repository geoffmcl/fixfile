@setlocal
@set TMPEXE=Release\fixfile.exe
@if NOT EXIST %TMPEXE% (
@echo Can NOT locate %TMPEXE%! *** FIX ME ***
@exit /b 1
)

@set TMPFIL1=temp1.txt
@set TMPFIL2=temp1u.txt
@set TMPFIL3=temp1d.txt

@echo Line 1 > %TMPFIL1%
@echo Line 2 >> %TMPFIL1%
@echo Line 3 >> %TMPFIL1%

%TMPEXE% %TMPFIL1% -u -n -o-%TMPFIL2%

%TMPEXE% %TMPFIL2% -n -o-%TMPFIL3%

fc4 %TMPFIL1% %TMPFIL3%

@rem eof

