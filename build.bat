@echo off

mkdir bin

:: Build the C++ project
echo "Building Unmanaged Libraries..."
cmake --preset windows . 
cmake --build Build/release
cp -r Build/release/*.dll bin

:: Build the C# project
echo "Building Managed Libraries..."
cd library || exit /b 1
dotnet build -o ../bin || exit /b 1
cd ../