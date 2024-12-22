@echo off
setlocal enabledelayedexpansion

echo == Initializing Submodules
git submodule update --init

echo == Applying Patches
call patch.bat apply || echo "Patch failed, continuing anyway..."

echo == Building Vcpkg
if not exist "Build" mkdir "Build"
cp -r ladybird/Toolchain Toolchain
python ./Toolchain/BuildVcpkg.py

echo Initialization complete.