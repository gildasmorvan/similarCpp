#!/bin/bash

# Convenience script to start the JamFree Web UI

set -e

# Get the directory of this script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "========================================"
echo "Starting JamFree Web UI"
echo "========================================"
echo

# Navigate to web directory
cd "$DIR/python/web"

# Run the startup script
./start.sh
