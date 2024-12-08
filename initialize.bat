git submodule update --init --recursive
patch.bat apply
mkdir build
cp -r ladybird/Toolchain Toolchain
"Toolchain/BuildVcpkg.sh"