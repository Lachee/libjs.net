#!/bin/bash
# Build the project
cmake --preset default .
cd Build/release
ninja
cd ../../


