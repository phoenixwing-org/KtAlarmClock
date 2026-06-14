@echo off
setlocal EnableExtensions EnableDelayedExpansion

chcp 65001 >nul

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%.") do set "PROJECT_ROOT=%%~fI"
cd /d "%PROJECT_ROOT%"

set "QT_ROOT_MANUAL=C:\Qt\6.7.2\msvc2019_64"
set "QT_STATIC_ROOT_MANUAL=C:\Qt\6.8.1-static\msvc2022_64"

set "STATIC_BUILD=OFF"
set "PROJECT_BRANCH=%~1"
if /i "%~1"=="static" (
    set "STATIC_BUILD=ON"
    set "PROJECT_BRANCH=%~2"
)
if "%PROJECT_BRANCH%"=="" set "PROJECT_BRANCH=release"

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "OUT_DIR=%PROJECT_ROOT%\out"
set "ROOT_DIR=%PROJECT_ROOT%"
if /i "%STATIC_BUILD%"=="ON" (
    set "QT_ROOT=%QT_STATIC_ROOT_MANUAL%"
    set "BUILD_DIR=%OUT_DIR%\build\static-release"
    set "DIST_DIR=%ROOT_DIR%\kt\viewer\static-dist"
) else (
    set "QT_ROOT=%QT_ROOT_MANUAL%"
    set "BUILD_DIR=%OUT_DIR%\build\release"
    set "DIST_DIR=%ROOT_DIR%\kt\viewer\dist"
)

if not exist "%QT_ROOT%\lib\cmake\Qt6\Qt6Config.cmake" (
    echo [ERROR] Qt path is invalid: %QT_ROOT%
    if /i "%STATIC_BUILD%"=="ON" (
        echo [ERROR] Edit QT_STATIC_ROOT_MANUAL near the top of build.bat.
    ) else (
        echo [ERROR] Edit QT_ROOT_MANUAL near the top of build.bat.
    )
    exit /b 1
)

echo Clean out folder
if exist "%OUT_DIR%\" (
    rmdir /s /q "%OUT_DIR%"
)
mkdir "%OUT_DIR%"

echo Setting up Visual Studio environment...
if not exist "!VSWHERE!" (
    echo [ERROR] vswhere.exe not found: !VSWHERE!
    exit /b 1
)

for /f "usebackq delims=" %%I in (`"!VSWHERE!" -latest -prerelease -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_INSTALL=%%I"
if not defined VS_INSTALL (
    echo [ERROR] Visual Studio with C++ tools not found!
    exit /b 1
)

set "VCVARSALL=!VS_INSTALL!\VC\Auxiliary\Build\vcvarsall.bat"
if not exist "!VCVARSALL!" (
    echo [ERROR] vcvarsall.bat not found: !VCVARSALL!
    exit /b 1
)

set "NINJA_EXE=!VS_INSTALL!\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
if not exist "!NINJA_EXE!" (
    echo [ERROR] Ninja not found: !NINJA_EXE!
    exit /b 1
)

call "!VCVARSALL!" x64
if errorlevel 1 (
    echo [ERROR] Failed to setup Visual Studio environment!
    exit /b %errorlevel%
)

echo Configure KtAlarmClock (%PROJECT_BRANCH%, static=%STATIC_BUILD%)
cmake -G Ninja ^
    -S "%PROJECT_ROOT%" ^
    -B "%BUILD_DIR%" ^
    -DQt6_DIR:PATH="%QT_ROOT%\lib\cmake\Qt6" ^
    -DCMAKE_PREFIX_PATH:STRING="%QT_ROOT%" ^
    -DCMAKE_BUILD_TYPE:STRING=Release ^
    -DKT_PROJECT_BRANCH:STRING=%PROJECT_BRANCH% ^
    -DKT_STATIC_BUILD:BOOL=%STATIC_BUILD% ^
    -DKT_DIST_SUBSYSTEM:STRING=WINDOWS ^
    -DKT_DIST_PATH:PATH="%DIST_DIR%" ^
    -DCMAKE_MAKE_PROGRAM:FILEPATH="!NINJA_EXE!"
if errorlevel 1 exit /b %errorlevel%

echo Build application
cmake --build "%BUILD_DIR%" --target KtAlarmClock
if errorlevel 1 exit /b %errorlevel%

echo Build and run tests
cmake --build "%BUILD_DIR%" --target KtAlarmClockCoreTests
if errorlevel 1 exit /b %errorlevel%

ctest --test-dir "%BUILD_DIR%" -C Release --output-on-failure
if errorlevel 1 exit /b %errorlevel%

echo Generate distribution
cmake --build "%BUILD_DIR%" --target KtAlarmClock_dist
if errorlevel 1 exit /b %errorlevel%

echo Dist folder: %DIST_DIR%
echo Success
