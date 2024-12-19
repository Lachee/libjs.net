#!/bin/bash

mkdir bin

# Build the project
cmake --preset default .
cmake --build Build/release
cp -r Build/release/*.so bin

# Build the C# project
echo "Building Managed Libraries..."
cd library || exit /b 1
dotnet build -o ../bin || exit /b 1
cd ../