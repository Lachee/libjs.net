#!/bin/bash

mkdir bin

# Build the project
cmake --preset default .
cd Build/release
ninja
cp -r *.so ../../bin
cd ../../

# Build the C# project
echo "Building Managed Libraries..."
cd library || exit /b 1
dotnet build -o ../bin || exit /b 1
cd ../