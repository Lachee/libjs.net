@echo off

mkdir bin

:: Build the C++ project
echo "Building Unmanaged Libraries..."
cmake --preset windows . 
cd build/release || exit /b 1
ninja || exit /b 1
cp -r *.dll ../../bin
cd ../../

:: Build the C# project
echo "Building Managed Libraries..."
cd library || exit /b 1
dotnet build -o ../bin || exit /b 1
cd ../