#!/bin/bash
git submodule update --init --recursive
mkdir build
cp -r ladybird/Toolchain Toolchain
"Toolchain/BuildVcpkg.sh"