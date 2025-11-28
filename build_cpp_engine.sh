#!/bin/bash

# Build script for SIMILAR2Logo C++ Engine with Python Bindings
# This script builds only the C++ components and Python bindings.
# Java components are built separately using Maven.

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

# Function to build with appropriate build system
build_project() {
    local project_dir="$1"
    local build_dir="$2"
    local project_name="$3"

    echo "Building $project_name..."
    mkdir -p "$build_dir"
    cd "$build_dir"
    cmake -DCMAKE_BUILD_TYPE=Release "$project_dir"

    # Use appropriate build command based on platform
    if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
        # Windows with MSVC
        cmake --build . --config Release
    else
        # Unix-like systems (Linux, macOS)
        make -j$(python3 -c "import os; print(os.cpu_count())")
    fi
}

# Build microkernel first
MICROKERNEL_DIR="$SCRIPT_DIR/cpp/microkernel"
MICROKERNEL_BUILD="$MICROKERNEL_DIR/build"
build_project "$MICROKERNEL_DIR" "$MICROKERNEL_BUILD" "microkernel"

# Build extendedkernel
echo ""
EXTKERNEL_DIR="$SCRIPT_DIR/cpp/extendedkernel"
EXTKERNEL_BUILD="$EXTKERNEL_DIR/build"
build_project "$EXTKERNEL_DIR" "$EXTKERNEL_BUILD" "extendedkernel"

# Create build directory for Logo engine
echo ""
echo "Creating Logo engine build directory..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Build Logo engine
echo ""
echo "Building Logo engine..."
# Copy CMakeLists_logo_cpp.txt to CMakeLists.txt for cmake
cp "$CPP_DIR/CMakeLists_logo_cpp.txt" "$CPP_DIR/CMakeLists.txt"

# Get pybind11 directory from pip installation
PYBIND11_DIR=$(python3 -c "import pybind11; import os; print(os.path.join(os.path.dirname(pybind11.__file__), 'share', 'cmake', 'pybind11'))")

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake -DCMAKE_BUILD_TYPE=Release \
      -DPYTHON_EXECUTABLE=$(which python3) \
      -Dpybind11_DIR="$PYBIND11_DIR" \
      "$CPP_DIR"

# Use appropriate build command based on platform
if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    # Windows with MSVC
    cmake --build . --config Release
else
    # Unix-like systems (Linux, macOS)
    make -j$(python3 -c "import os; print(os.cpu_count())")
fi

# Install
echo ""
echo "Installing to Python package..."
if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    echo "Windows detected - extension built to python/similar2logo/ directory"
    # On Windows, LIBRARY_OUTPUT_DIRECTORY should be sufficient
else
make install
fi

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
