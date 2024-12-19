@echo off

mkdir bin

:: Build the C++ project
echo "Building Unmanaged Libraries..."
cmake --preset windows . || exit /b 1
cmake --build Build/release || exit /b 1
cp -r Build/release/*.dll bin

:: Build the C# project
echo "Building Managed Libraries..."
cd library || exit /b 1
dotnet build -o ../bin || exit /b 1
cd ../