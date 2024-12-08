@echo off

:: Build the project
cmake --preset default . 
cd build/release || exit /b 1
ninja || exit /b 1
cd ../../

