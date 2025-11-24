#!/bin/bash
# Build Metal GPU acceleration module for JamFree

set -e

echo "========================================"
echo "Building JamFree Metal GPU Module"
echo "========================================"
echo

# Check if on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "❌ Metal GPU acceleration is only available on macOS"
    exit 1
fi

echo "✓ Running on macOS"

# Check for Metal framework
if ! xcrun --sdk macosx --show-sdk-path &> /dev/null; then
    echo "❌ Xcode Command Line Tools not found"
    echo "   Install with: xcode-select --install"
    exit 1
fi

echo "✓ Xcode Command Line Tools found"

# Skip Metal shader compilation for now (requires Metal toolchain)
echo
echo "⚠️  Skipping Metal shader compilation (Metal toolchain not installed)"
echo "   To install: xcodebuild -downloadComponent MetalToolchain"
echo "   Shaders can be compiled later when needed"

# Build Python extension with clang++ directly
echo
echo "Building Python extension..."

# Get Python include path
PYTHON_INCLUDE=$(python3 -c "import sysconfig; print(sysconfig.get_path('include'))")
PYBIND11_INCLUDE=$(python3 -c "import pybind11; print(pybind11.get_include())")

# Get Python library path
PYTHON_LIB=$(python3 -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR'))")
PYTHON_VERSION=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")

# Output directory
mkdir -p python/jamfree

# Compile
echo "  Compiling metal_bindings.mm..."
clang++ -std=c++17 -O3 -Wall -shared -fPIC -fobjc-arc \
    -I"$PYTHON_INCLUDE" \
    -I"$PYBIND11_INCLUDE" \
    -I. \
    -framework Metal \
    -framework Foundation \
    -undefined dynamic_lookup \
    python/src/metal_bindings.mm \
    gpu/metal/MetalCompute.mm \
    -o python/jamfree/_metal.cpython-${PYTHON_VERSION//.}-darwin.so

if [ $? -eq 0 ]; then
    echo "✓ Metal module built successfully"
    echo
    echo "Output: python/jamfree/_metal.cpython-${PYTHON_VERSION//.}-darwin.so"
    echo
    echo "To test:"
    echo "  python3 -c 'from jamfree import _metal; print(_metal.is_metal_available())'"
else
    echo "❌ Build failed"
    exit 1
fi
