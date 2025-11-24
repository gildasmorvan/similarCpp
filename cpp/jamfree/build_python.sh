#!/bin/bash

# Build script for JamFree Python bindings

set -e  # Exit on error

echo "========================================"
echo "Building JamFree Python Bindings"
echo "========================================"
echo

# Check for pybind11
echo "Checking for pybind11..."
python3 -c "import pybind11" 2>/dev/null || {
    echo "pybind11 not found. Installing..."
    pip3 install pybind11
}
echo "✓ pybind11 found"
echo

# Build extension
echo "Building C++ extension..."
python3 setup.py build_ext --inplace
echo "✓ Extension built"
echo

# Install in development mode
echo "Installing in development mode..."
pip3 install -e .
echo "✓ Installed"
echo

echo "========================================"
echo "✓ Build complete!"
echo "========================================"
echo
echo "Test the installation:"
echo "  python3 -c 'import jamfree; print(jamfree.__version__)'"
echo
echo "Run example:"
echo "  python3 python/examples/highway_simulation.py"
echo
