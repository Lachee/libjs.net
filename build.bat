@echo off

:: Build the project
cmake --preset windows . 
cd build/release || exit /b 1
ninja || exit /b 1
cd ../../

