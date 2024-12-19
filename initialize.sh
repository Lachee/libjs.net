#!/bin/bash
git submodule update --init
# patch.bat apply
mkdir Build
cp -r ladybird/Toolchain Toolchain

determine_python_version() {
    if command -v python3 &>/dev/null; then
        echo "python3"
    elif command -v python &>/dev/null; then
        echo "python"
    else
        echo "No suitable Python interpreter found" >&2
        exit 1
    fi
}

PYTHON=$(determine_python_version)
$PYTHON ./Toolchain/BuildVcpkg.py
