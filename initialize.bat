git submodule update
patch.bat apply
mkdir Build
cp -r ladybird/Toolchain Toolchain
python ./Toolchain/BuildVcpkg.py