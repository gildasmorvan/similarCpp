#!/bin/bash

# Startup script for JamFree Web UI

set -e

echo "========================================"
echo "JamFree Web UI - Startup"
echo "========================================"
echo

# Check if in correct directory
if [ ! -f "app.py" ]; then
    echo "Error: Must run from python/web directory"
    exit 1
fi

# Check Python
if ! command -v python3 &> /dev/null; then
    echo "Error: Python 3 not found"
    exit 1
fi

echo "Setting up virtual environment..."

# Create virtual environment if it doesn't exist
if [ ! -d "venv" ]; then
    echo "Creating virtual environment..."
    python3 -m venv venv
    echo "✓ Virtual environment created"
fi

# Activate virtual environment
source venv/bin/activate

echo "Checking dependencies..."

# Check if Flask is installed in venv
if ! python -c "import flask" 2>/dev/null; then
    echo "Installing dependencies..."
    pip install -r requirements.txt
else
    echo "✓ Dependencies installed"
fi

echo

# Check if jamfree is available
if python -c "import jamfree" 2>/dev/null; then
    echo "✓ JamFree Python bindings available"
else
    echo "⚠ JamFree Python bindings not found"
    echo "  Build them with: cd ../.. && ./build_python.sh"
    echo "  Web UI will run in limited mode"
fi

echo
echo "========================================"
echo "Starting JamFree Web UI"
echo "========================================"
echo
echo "Open your browser to:"
echo "  http://localhost:5001"
echo
echo "Press Ctrl+C to stop"
echo

# Create uploads directory
mkdir -p uploads

# Run Flask app
python app.py
