#!/bin/bash

# JamFree Simulation Runner
# Comprehensive script to run and configure JamFree simulations

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

# Helper functions
print_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ $1${NC}"
}

# Check if JamFree is built
check_jamfree() {
    if python3 -c "import jamfree" 2>/dev/null; then
        print_success "JamFree Python bindings available"
        return 0
    else
        print_error "JamFree Python bindings not found"
        echo
        echo "Build options:"
        echo "  1. Full build:    ./build.sh"
        echo "  2. Python only:   ./build_python.sh"
        echo "  3. With Metal GPU: ./build_metal.sh"
        return 1
    fi
}

# Show main menu
show_menu() {
    print_header "JamFree Simulation Runner"
    
    echo "Select simulation mode:"
    echo
    echo "  ${GREEN}Python Examples:${NC}"
    echo "    1) Adaptive Hybrid Simulation"
    echo "    2) OSM Hybrid Simulation"
    echo "    3) Incident Response Simulation"
    echo "    4) Traffic Signal Optimization"
    echo "    5) GPU Metal Acceleration Demo"
    echo
    echo "  ${BLUE}Web UI:${NC}"
    echo "    6) Start Web UI (Interactive)"
    echo
    echo "  ${YELLOW}Build & Setup:${NC}"
    echo "    7) Build JamFree (Full)"
    echo "    8) Build Python Bindings Only"
    echo "    9) Build with Metal GPU Support"
    echo "   10) Run All Tests"
    echo
    echo "    0) Exit"
    echo
}

# Run adaptive hybrid example
run_adaptive_hybrid() {
    print_header "Adaptive Hybrid Simulation"
    
    if ! check_jamfree; then
        return 1
    fi
    
    print_info "Running adaptive hybrid simulation..."
    echo
    python3 examples/python/adaptive_hybrid_example.py
}

# Run OSM hybrid example
run_osm_hybrid() {
    print_header "OSM Hybrid Simulation"
    
    if ! check_jamfree; then
        return 1
    fi
    
    print_info "Running OSM-based hybrid simulation..."
    echo
    python3 examples/python/osm_hybrid_simulation.py
}

# Run incident response example
run_incident_response() {
    print_header "Incident Response Simulation"
    
    if ! check_jamfree; then
        return 1
    fi
    
    print_info "Running incident response simulation..."
    echo
    python3 examples/python/incident_response.py
}

# Run traffic signal optimization
run_traffic_signal() {
    print_header "Traffic Signal Optimization"
    
    if ! check_jamfree; then
        return 1
    fi
    
    print_info "Running traffic signal optimization..."
    echo
    python3 examples/python/traffic_signal_optimization.py
}

# Run GPU demo
run_gpu_demo() {
    print_header "GPU Metal Acceleration Demo"
    
    if ! check_jamfree; then
        return 1
    fi
    
    # Check if Metal is available
    if python3 -c "import jamfree; exit(0 if jamfree.is_metal_available() else 1)" 2>/dev/null; then
        print_success "Metal GPU acceleration available"
    else
        print_warning "Metal GPU not available on this system"
        print_info "The demo will show simulated performance metrics"
    fi
    
    echo
    print_info "Running GPU acceleration demo..."
    echo
    python3 examples/python/gpu_metal_example.py
}

# Start web UI
start_web_ui() {
    print_header "JamFree Web UI"
    
    if ! check_jamfree; then
        print_warning "Web UI will run in limited mode"
    fi
    
    print_info "Starting web server..."
    echo
    
    cd python/web
    
    # Check if venv exists
    if [ ! -d "venv" ]; then
        print_info "Creating virtual environment..."
        python3 -m venv venv
    fi
    
    # Activate venv
    source venv/bin/activate
    
    # Install dependencies
    if ! python -c "import flask" 2>/dev/null; then
        print_info "Installing dependencies..."
        pip install -q -r requirements.txt
    fi
    
    print_success "Web UI ready"
    echo
    echo "Open your browser to: ${GREEN}http://localhost:5001${NC}"
    echo
    print_info "Press Ctrl+C to stop"
    echo
    
    # Run app
    python app.py
}

# Build full
build_full() {
    print_header "Building JamFree (Full)"
    
    print_info "This will build all components..."
    echo
    
    if [ -f "build.sh" ]; then
        ./build.sh
    else
        print_error "build.sh not found"
        return 1
    fi
}

# Build Python only
build_python() {
    print_header "Building Python Bindings"
    
    print_info "Building Python bindings only..."
    echo
    
    if [ -f "build_python.sh" ]; then
        ./build_python.sh
    else
        print_error "build_python.sh not found"
        return 1
    fi
}

# Build with Metal
build_metal() {
    print_header "Building with Metal GPU Support"
    
    if [[ "$OSTYPE" != "darwin"* ]]; then
        print_error "Metal GPU is only available on macOS"
        return 1
    fi
    
    print_info "Building with Metal GPU support..."
    echo
    
    if [ -f "build_metal.sh" ]; then
        ./build_metal.sh
    else
        print_error "build_metal.sh not found"
        return 1
    fi
}

# Run all tests
run_all_tests() {
    print_header "Running All Tests"
    
    if ! check_jamfree; then
        return 1
    fi
    
    print_info "Running Python examples..."
    echo
    
    # Run each example
    for example in examples/python/*.py; do
        if [[ "$example" == *"example.py" ]] || [[ "$example" == *"simulation.py" ]]; then
            basename=$(basename "$example")
            print_info "Running $basename..."
            if python3 "$example" > /dev/null 2>&1; then
                print_success "$basename passed"
            else
                print_error "$basename failed"
            fi
        fi
    done
    
    echo
    print_success "All tests completed"
}

# Main loop
main() {
    while true; do
        show_menu
        read -p "Enter your choice [0-10]: " choice
        echo
        
        case $choice in
            1) run_adaptive_hybrid ;;
            2) run_osm_hybrid ;;
            3) run_incident_response ;;
            4) run_traffic_signal ;;
            5) run_gpu_demo ;;
            6) start_web_ui ;;
            7) build_full ;;
            8) build_python ;;
            9) build_metal ;;
           10) run_all_tests ;;
            0) 
                print_info "Goodbye!"
                exit 0
                ;;
            *)
                print_error "Invalid choice. Please try again."
                ;;
        esac
        
        echo
        echo
        read -p "Press Enter to continue..."
        clear
    done
}

# Run main
main
