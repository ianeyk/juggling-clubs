ECHO Hello World!

cd ../data/assets

@echo off

@REM Strip hashes appended to files. For example, index-0388e401.css becomes index.css

setlocal enabledelayedexpansion
set deletestring=*-*.*

for /f %%F in ('dir /b "%deletestring%"') do (
    for /f "tokens=2 delims=." %%E in ("%%F") do (
        for /f "tokens=1,* delims=-" %%A in ("%%F") do (
            Ren %%F %%A.%%E
        )
    )
)
endlocal

@REM Delete redundant webfont files. Keep only materialdesignicons.woff
del materialdesignicons.eot
del materialdesignicons.ttf
del materialdesignicons.woff2

@REM Find and replace inside the file index.css to match the renamed materialdesignicon files
set ffile='index.css'
set fold='materialdesignicons.*?\.'
set fnew='materialdesignicons.'
powershell -Command "(gc %ffile%) -replace %fold%, %fnew% | Out-File %ffile%"

ECHO Finished!
