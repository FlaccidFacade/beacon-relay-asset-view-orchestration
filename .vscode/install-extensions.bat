@echo off
REM Install all recommended VS Code extensions for B.R.A.V.O. development

echo Installing B.R.A.V.O. recommended VS Code extensions...
echo.

REM Check if code command is available
where code >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo VS Code 'code' command not found in PATH
    echo.
    echo Please ensure VS Code is installed and the 'code' command is available.
    echo The 'code' command should be available after VS Code installation.
    exit /b 1
)

set TOTAL=15
set INSTALLED=0
set SKIPPED=0
set FAILED=0

REM Install extensions
call :InstallExtension "dbaeumer.vscode-eslint"
call :InstallExtension "esbenp.prettier-vscode"
call :InstallExtension "bradlc.vscode-tailwindcss"
call :InstallExtension "amazonwebservices.aws-toolkit-vscode"
call :InstallExtension "platformio.platformio-ide"
call :InstallExtension "mathiasfrohlich.kotlin"
call :InstallExtension "vscjava.vscode-java-pack"
call :InstallExtension "eamodio.gitlens"
call :InstallExtension "github.vscode-pull-request-github"
call :InstallExtension "yzhang.markdown-all-in-one"
call :InstallExtension "davidanson.vscode-markdownlint"
call :InstallExtension "editorconfig.editorconfig"
call :InstallExtension "ms-vscode.live-server"
call :InstallExtension "christian-kohler.path-intellisense"
call :InstallExtension "visualstudioexptteam.vscodeintellicode"

REM Summary
echo ========================================
echo Installation Summary:
echo    Total extensions: %TOTAL%
echo    Newly installed: %INSTALLED%
echo    Already installed: %SKIPPED%
echo    Failed: %FAILED%
echo ========================================
echo.

if %FAILED% EQU 0 (
    echo All extensions are now installed!
    echo.
    echo Next steps:
    echo   1. Restart VS Code to activate all extensions
    echo   2. For PlatformIO: Additional components will download on first use
    echo   3. For AWS Toolkit: Configure your AWS credentials
) else (
    echo Some extensions failed to install. Please try installing them manually.
    exit /b 1
)

exit /b 0

:InstallExtension
set "EXT=%~1"
echo Installing: %EXT%

REM Check if already installed
code --list-extensions | findstr /C:"%EXT%" >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo    Already installed, skipping
    set /a SKIPPED+=1
) else (
    REM Try to install
    code --install-extension "%EXT%" >nul 2>nul
    if %ERRORLEVEL% EQU 0 (
        echo    Installed successfully
        set /a INSTALLED+=1
    ) else (
        echo    Failed to install
        set /a FAILED+=1
    )
)
echo.
exit /b 0
