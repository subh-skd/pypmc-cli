@echo off
:: pypmc installer for Windows (CMD)
:: Usage: curl -fsSL https://raw.githubusercontent.com/subh-skd/pypmc-cli/main/install.bat -o install.bat && install.bat
setlocal enabledelayedexpansion

set "REPO=subh-skd/pypmc-cli"
set "BIN_NAME=pypmc.exe"
set "INSTALL_DIR=%LOCALAPPDATA%\pypmc"
set "ASSET_NAME=pypmc-windows-amd64.exe"

:: ── fetch latest release tag ────────────────────────────────────────

echo Detecting latest pypmc release...

set "API_URL=https://api.github.com/repos/%REPO%/releases/latest"
set "TMPJSON=%TEMP%\pypmc-release.json"

curl -fsSL "%API_URL%" -o "%TMPJSON%" 2>nul
if %errorlevel% neq 0 (
    echo Error: Could not fetch latest release. Make sure curl is available.
    echo Download manually from https://github.com/%REPO%/releases
    goto :fail
)

:: extract tag_name from JSON
set "TAG="
for /f "tokens=2 delims=:, " %%a in ('findstr /c:"\"tag_name\"" "%TMPJSON%"') do (
    set "TAG=%%~a"
)
del "%TMPJSON%" 2>nul

if "%TAG%"=="" (
    echo Error: Could not determine latest release tag.
    goto :fail
)

echo Latest version: %TAG%

:: ── download binary ─────────────────────────────────────────────────

set "DOWNLOAD_URL=https://github.com/%REPO%/releases/download/%TAG%/%ASSET_NAME%"
set "TMPFILE=%TEMP%\pypmc-download.exe"

echo Downloading %ASSET_NAME%...

curl -fSL --progress-bar -o "%TMPFILE%" "%DOWNLOAD_URL%"
if %errorlevel% neq 0 (
    echo Error: Failed to download from %DOWNLOAD_URL%
    goto :fail
)

:: ── install ─────────────────────────────────────────────────────────

echo Installing to %INSTALL_DIR%...

if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

move /y "%TMPFILE%" "%INSTALL_DIR%\%BIN_NAME%" >nul
if %errorlevel% neq 0 (
    echo Error: Failed to move binary to %INSTALL_DIR%
    goto :fail
)

:: ── add to PATH if not already there ────────────────────────────────

echo %PATH% | findstr /i /c:"%INSTALL_DIR%" >nul 2>&1
if %errorlevel% neq 0 (
    echo Adding %INSTALL_DIR% to user PATH...
    for /f "tokens=2*" %%a in ('reg query "HKCU\Environment" /v Path 2^>nul') do set "USER_PATH=%%b"
    if defined USER_PATH (
        setx Path "!USER_PATH!;%INSTALL_DIR%" >nul 2>&1
    ) else (
        setx Path "%INSTALL_DIR%" >nul 2>&1
    )
    set "PATH=%PATH%;%INSTALL_DIR%"
)

:: ── verify ──────────────────────────────────────────────────────────

echo.
"%INSTALL_DIR%\%BIN_NAME%" --version
if %errorlevel% equ 0 (
    echo.
    echo pypmc installed successfully!
) else (
    echo pypmc was installed to %INSTALL_DIR%\%BIN_NAME%
)

echo.
echo Get started:
echo   mkdir my-project ^&^& cd my-project
echo   pypmc init
echo.
echo NOTE: Restart your terminal for PATH changes to take effect.
echo       Path added: %INSTALL_DIR%

goto :done

:fail
echo.
echo Installation failed.
exit /b 1

:done
endlocal
