git submodule update --init
patch.bat apply
mkdir Build
cp -r ladybird/Toolchain Toolchain
python ./Toolchain/BuildVcpkg.py