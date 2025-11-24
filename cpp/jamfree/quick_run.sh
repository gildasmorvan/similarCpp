#!/bin/bash

# Quick JamFree Simulation Runner (Non-Interactive)
# Usage: ./quick_run.sh [simulation_name]

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

print_usage() {
    echo "Usage: $0 [simulation]"
    echo
    echo "Available simulations:"
    echo "  adaptive      - Adaptive Hybrid Simulation"
    echo "  osm           - OSM Hybrid Simulation"
    echo "  incident      - Incident Response Simulation"
    echo "  signal        - Traffic Signal Optimization"
    echo "  gpu           - GPU Metal Acceleration Demo"
    echo "  web           - Start Web UI"
    echo "  test          - Run all tests"
    echo
    echo "Examples:"
    echo "  $0 adaptive"
    echo "  $0 web"
    echo "  $0 test"
}

if [ $# -eq 0 ]; then
    print_usage
    exit 1
fi

case "$1" in
    adaptive)
        echo -e "${BLUE}Running Adaptive Hybrid Simulation...${NC}"
        python3 examples/python/adaptive_hybrid_example.py
        ;;
    osm)
        echo -e "${BLUE}Running OSM Hybrid Simulation...${NC}"
        python3 examples/python/osm_hybrid_simulation.py
        ;;
    incident)
        echo -e "${BLUE}Running Incident Response Simulation...${NC}"
        python3 examples/python/incident_response.py
        ;;
    signal)
        echo -e "${BLUE}Running Traffic Signal Optimization...${NC}"
        python3 examples/python/traffic_signal_optimization.py
        ;;
    gpu)
        echo -e "${BLUE}Running GPU Metal Demo...${NC}"
        python3 examples/python/gpu_metal_example.py
        ;;
    web)
        echo -e "${BLUE}Starting Web UI...${NC}"
        ./run_web_ui.sh
        ;;
    test)
        echo -e "${BLUE}Running all tests...${NC}"
        for example in examples/python/*_example.py examples/python/*_simulation.py; do
            if [ -f "$example" ]; then
                echo -e "${GREEN}Testing: $(basename $example)${NC}"
                python3 "$example" || echo -e "${RED}Failed: $(basename $example)${NC}"
            fi
        done
        ;;
    *)
        echo -e "${RED}Unknown simulation: $1${NC}"
        echo
        print_usage
        exit 1
        ;;
esac
