#!/bin/bash
# Run ant_foraging_dsl.py with web interface
#
# This script sets up the environment and runs the simulation

cd "$(dirname "$0")/../.."

echo "=========================================="
echo "Ant Foraging DSL - Web Interface"
echo "=========================================="
echo ""

# Check for .venv
PYTHON_CMD="python3"
if [ -d ".venv" ]; then
    echo "Using virtual environment in .venv"
    PYTHON_CMD=".venv/bin/python3"
elif [ -z "$VIRTUAL_ENV" ]; then
    echo "Note: Not in a virtual environment."
    echo "Installing dependencies with --user flag..."
    echo ""
    
    # Install dependencies for user
    python3 -m pip install --user fastapi uvicorn websockets 2>/dev/null || true
fi

# Run the simulation
echo "Starting simulation..."
echo "Open http://localhost:8080 in your browser"
echo ""

PYTHONPATH=python $PYTHON_CMD examples/python/ant_foraging_dsl.py --web
