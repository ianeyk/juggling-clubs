ECHO Hello World!

@REM @echo off

@REM Strip hashes appended to files. For example, index-0388e401.css becomes index.css

set data-dir=..\data
set assets-dir=%data-dir%\assets

setlocal enabledelayedexpansion
set deletestring=*-*.*

for /f %%F in ('dir %assets-dir% /b "%deletestring%"') do (
    for /f "tokens=2 delims=." %%E in ("%%F") do (
        for /f "tokens=1,* delims=-" %%A in ("%%F") do (
            Ren %assets-dir%\%%F %%A.%%E
        )
    )
)
endlocal

@REM Delete redundant webfont files. Keep only materialdesignicons.woff
del %assets-dir%\materialdesignicons.eot
del %assets-dir%\materialdesignicons.ttf
del %assets-dir%\materialdesignicons.woff2

@REM Find and replace inside the file index.css to match the renamed materialdesignicon files
set ffile='%assets-dir%\index.css'
set fold='materialdesignicons.*?\.'
set fnew='materialdesignicons.'
powershell -Command "(gc %ffile%) -replace %fold%, %fnew% | Out-File %ffile% -encoding utf8"

@REM Find and replace inside the file index.css to match the renamed materialdesignicon files
set ffile='%data-dir%\index.html'
set fold='index.*?\.'
set fnew='index.'
powershell -Command "(gc %ffile%) -replace %fold%, %fnew% | Out-File %ffile% -encoding utf8"

@echo on

ECHO Finished!
