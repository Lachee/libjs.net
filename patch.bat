@echo off
setlocal enabledelayedexpansion

set PATCH_DIR=patches
if not exist "%PATCH_DIR%" (
    mkdir "%PATCH_DIR%"
)

if "%~1"=="apply" goto :apply_patches
if "%~1"=="generate" goto :generate_patches
echo ERROR: Invalid argument "%~1".
echo Usage: %~nx0 "[apply|generate]"
exit /b 1

:apply_patches
echo Applying patches...
if not exist "%PATCH_DIR%\*.patch" (
    echo ERROR: No patch files found in "%PATCH_DIR%".
    exit /b 1
)

:: Apply each patch
cd ladybird || exit /b 1
for %%P in ("..\%PATCH_DIR%\*.patch") do (
    echo Applying patch: %%P
    git apply --ignore-space-change --ignore-whitespace "%%P"
    if errorlevel 1 (
        echo ERROR: Failed to apply patch %%P
        exit /b 1
    )
)

echo All patches applied successfully.
exit /b 0

:generate_patches
echo Generating patches...
cd ladybird || exit /b 1

git diff --quiet
if errorlevel 1 (
    echo Uncommitted changes detected. Generating patches...
) else (
    echo ERROR: No changes to generate patches from.
    exit /b 1
)

:: Generate patches for unstaged changes
echo Generating patches for unstaged changes...
for /f "delims=" %%F in ('git diff --name-only --diff-filter=ACMRT') do (
    set FILE=%%F
    set PATCH_FILE=..\%PATCH_DIR%\!FILE:/=_!.patch
    git diff "%%F" > "!PATCH_FILE!"
    echo Created patch for unstaged changes: !PATCH_FILE!
)

echo All patches generated in "%PATCH_DIR%".
exit /b 0
