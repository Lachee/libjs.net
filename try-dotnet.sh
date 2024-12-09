#!/bin/bash
cd Build/release
ninja -j32
cd ../../dotnet
#LD_DEBUG=libs dotnet run
dotnet run
