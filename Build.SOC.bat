@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

:: 0. Check Windows version
for /f "tokens=2 delims=[]" %%a in ('ver') do set "ver=%%a"
for /f "tokens=2,3,4 delims=. " %%b in ("!ver!") do set /a "maj=%%b, min=%%c, build=%%d"

:: Windows 10 20H2 has version 10.0.19042. Windows 11 starts with version 10.0.22000
if !maj! lss 10 (
    echo Windows 10 or higher is required.
    pause
    exit /b 1
) else if !maj! equ 10 (
    if !build! lss 19042 (
        echo Windows 10 version 20H2 or higher is required.
        pause
        exit /b 1
    )
)

:: 1. Check for Unreal Engine version 5.3 installation
set "UEVersion=5.3"
set "RegistryKey=HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine\%UEVersion%"
reg query "%RegistryKey%" >nul 2>&1
if %errorlevel% neq 0 (
    echo Unreal Engine %UEVersion% is not installed.
    pause
    exit /b 1
)

:: Save path to UnrealBuildTool.exe in a variable
for /f "tokens=2*" %%A in ('reg query "%RegistryKey%" /v InstalledDirectory ^| find "REG_SZ"') do (
    set "UnrealBuildToolPath=%%B\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
)
echo Path to UnrealBuildTool.exe: %UnrealBuildToolPath% >nul

:: 3. Check for Visual Studio 2022 installation
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "tokens=*" %%i in ('"%VSWHERE%" -version 17 -property installationPath') do (
    set "VSPath=%%i"
)
if "!VSPath!"=="" (
    echo Visual Studio 2022 not found.
    pause
    exit /b 1
)
echo Path to Visual Studio: !VSPath! >nul

:: 4. Check for "Game development with C++" component installation
set "FOUND=0"
for /f "tokens=*" %%i in ('"%VSWHERE%" -version 17 -products * -requires Microsoft.VisualStudio.Workload.NativeGame -format text') do (
    set "line=%%i"
    if "!line:installationPath=!" neq "!line!" set "FOUND=1"
)
if "!FOUND!"=="0" (
    echo "Game development with C++" package not found.
    pause
    exit /b 1
)

:: Path to VsDevCmd.bat
call "!VSPath!\Common7\Tools\VsDevCmd.bat" >nul

:: Configuration selection
echo Choose build configuration:
echo [1] Development (Development Editor) *Recommended
echo [2] Debug (DebugGame Editor)
set /p ConfigChoice="Enter configuration number: "
if "!ConfigChoice!"=="1" (
    set "EngineConfig=Release"
    set "StalkerConfig=Development Editor"
) else if "!ConfigChoice!"=="2" (
    set "EngineConfig=Debug"
    set "StalkerConfig=DebugGame Editor"
) else (
    echo Invalid choice.
    pause
    exit /b 1
)

:: Build execution
echo Building SOC project with %EngineConfig% (%StalkerConfig%)...

echo Start Build RedImage...
call msbuild "./Source/XRayEngine/Source/External/RedImage/RedImageTool/RedImageTool.vcxproj" /p:Configuration=Release /p:Platform=x64 /maxCpuCount /nologo
if errorlevel 1 (
    echo Build of RedImage failed.
    pause
    exit /b 1
)
echo RedImage successfully built.

echo Start Build EngineSOC with %EngineConfig%...
call msbuild "./Source/XRayEngine/Source/EngineSOC.sln" /p:Configuration=%EngineConfig% /p:Platform=x64 /maxCpuCount /nologo
if errorlevel 1 (
    echo Build of EngineSOC failed.
    pause
    exit /b 1
)
echo EngineSOC successfully built.

if exist "%~dp0Stalker.sln" (
    echo Project detected, skipping project generation.
) else (
    call "%UnrealBuildToolPath%" -projectfiles -project="%~dp0Stalker.uproject" -game -engine -progress || (
        echo Project generation failed.
        pause
        exit /b 1
    )
    echo Project successfully generated.
)

echo Start Build Stalker with %StalkerConfig%...
call msbuild "./Stalker.sln" /t:Games\Stalker /p:Configuration="%StalkerConfig%" /p:Platform=Win64 /maxCpuCount /nologo
if errorlevel 1 (
    echo Build of Stalker failed.
    pause
    exit /b 1
)
echo Stalker successfully built.

echo Project successfully built.
pause

endlocal
