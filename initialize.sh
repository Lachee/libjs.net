#!/bin/bash
git submodule update --init

# Apply all patches from patches/ directory to ladybird/
if [ -d "patches" ]; then
    for patch in patches/*.patch; do
        if [ -f "$patch" ]; then
            echo "Applying patch: $patch"
            patch -p1 -d ladybird/ < "$patch"
        fi
    done
fi

mkdir -p Build
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
