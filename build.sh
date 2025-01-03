#!/usr/bin/bash

echo "$1"

(echo "Building for Linux" && cd cmake-build-release && cmake .. $1 && cmake --build . --parallel --clean-first --config Release && cp Achi-console Achi-SDL ../Release/Linux/)
(echo "Building for Windows" && cd cmake-build-release-windows && cmake .. -DCMAKE_TOOLCHAIN_FILE=../mingw-w64-x86_64.cmake $1 && cmake --build . --parallel --clean-first --config Release && cp Achi-console.exe Achi-SDL.exe ../Release/Windows/)
