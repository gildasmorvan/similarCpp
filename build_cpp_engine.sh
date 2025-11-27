#!/bin/bash

# Build script for SIMILAR2Logo C++ Engine Python Bindings

set -e  # Exit on error

echo "========================================"
echo "Building SIMILAR2Logo C++ Engine"
echo "========================================"
echo ""

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
CPP_DIR="$SCRIPT_DIR/cpp/similar2logo/python"
BUILD_DIR="$CPP_DIR/build_cpp_engine"

# Check for required tools
command -v cmake >/dev/null 2>&1 || { 
    echo "Error: cmake is required but not installed."
    echo "Install with: brew install cmake (macOS) or apt-get install cmake (Linux)"
    exit 1
}

command -v python3 >/dev/null 2>&1 || {
    echo "Error: python3 is required but not installed."
    exit 1
}

# Check for pybind11
python3 -c "import pybind11" 2>/dev/null || {
    echo "Error: pybind11 is required but not installed."
    echo "Install with: pip install pybind11"
    exit 1
}

echo "✓ All dependencies found"
echo ""

# Clean build directories for CI environments
if [ -n "$CI" ] || [ -n "$GITHUB_ACTIONS" ]; then
    echo "CI environment detected, cleaning build directories..."
    rm -rf "$SCRIPT_DIR/cpp/microkernel/build"
    rm -rf "$SCRIPT_DIR/cpp/extendedkernel/build"
    rm -rf "$BUILD_DIR"
fi

# Build microkernel first
echo "Building microkernel..."
MICROKERNEL_DIR="$SCRIPT_DIR/cpp/microkernel"
MICROKERNEL_BUILD="$MICROKERNEL_DIR/build"
mkdir -p "$MICROKERNEL_BUILD"
cd "$MICROKERNEL_BUILD"
cmake -DCMAKE_BUILD_TYPE=Release "$MICROKERNEL_DIR"
make -j$(python3 -c "import os; print(os.cpu_count())")

# Build extendedkernel
echo ""
echo "Building extendedkernel..."
EXTKERNEL_DIR="$SCRIPT_DIR/cpp/extendedkernel"
EXTKERNEL_BUILD="$EXTKERNEL_DIR/build"
mkdir -p "$EXTKERNEL_BUILD"
cd "$EXTKERNEL_BUILD"
cmake -DCMAKE_BUILD_TYPE=Release "$EXTKERNEL_DIR"
make -j$(python3 -c "import os; print(os.cpu_count())")

# Create build directory for Logo engine
echo ""
echo "Creating Logo engine build directory..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo "Configuring CMake..."
# Copy CMakeLists_logo_cpp.txt to CMakeLists.txt for cmake
cp "$CPP_DIR/CMakeLists_logo_cpp.txt" "$CPP_DIR/CMakeLists.txt"
cmake -DCMAKE_BUILD_TYPE=Release \
      -DPYTHON_EXECUTABLE=$(which python3) \
      "$CPP_DIR"

# Build
echo ""
echo "Building C++ engine..."
NPROC=$(python3 -c "import os; print(os.cpu_count())")
make -j$NPROC

# Install
echo ""
echo "Installing to Python package..."
make install

echo ""
echo "========================================"
echo "✓ Build complete!"
echo "========================================"
echo ""
echo "The C++ engine is now available."
echo "Test with:"
echo "  python3 -c 'from similar2logo.cpp_engine import use_cpp_engine; print(use_cpp_engine())'"
echo ""
echo "Run an example:"
echo "  ./examples/python/run_boids_cpp.sh"
echo ""
