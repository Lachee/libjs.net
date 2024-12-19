git submodule update --init
:: patch.bat apply || echo "patch.bat apply failed, continuing..."
mkdir Build
cp -r ladybird/Toolchain Toolchain
python ./Toolchain/BuildVcpkg.py