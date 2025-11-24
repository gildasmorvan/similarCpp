#!/bin/bash

# JamFree C++ Build Script

set -e  # Exit on error

echo "========================================"
echo "Building JamFree C++"
echo "========================================"
echo ""

# Create build directory
BUILD_DIR="build"
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning existing build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo ""
echo "Building..."
make -j$(sysctl -n hw.ncpu)

echo ""
echo "========================================"
echo "✓ Build complete!"
echo "========================================"
echo ""
echo "Executable: build/bin/highway_example"
echo ""
echo "Run the example:"
echo "  cd build/bin"
echo "  ./highway_example"
echo ""
