# JamFree Bash Scripts - Quick Reference

## Overview

JamFree provides several bash scripts to simplify running and configuring traffic simulations.

## Main Scripts

### 🎯 `run_simulation.sh` - Interactive Menu (Recommended)

**Best for:** Beginners, exploration, manual testing

```bash
cd cpp/jamfree
./run_simulation.sh
```

**Features:**
- Interactive menu interface
- Color-coded output
- Automatic dependency checking
- Build options included
- Test runner included

**Menu Options:**
1. Adaptive Hybrid Simulation
2. OSM Hybrid Simulation  
3. Incident Response Simulation
4. Traffic Signal Optimization
5. GPU Metal Acceleration Demo
6. Start Web UI (Interactive)
7. Build JamFree (Full)
8. Build Python Bindings Only
9. Build with Metal GPU Support
10. Run All Tests

---

### ⚡ `quick_run.sh` - Command-Line Runner

**Best for:** Automation, scripts, CI/CD

```bash
cd cpp/jamfree
./quick_run.sh [simulation_name]
```

**Available Commands:**
```bash
./quick_run.sh adaptive    # Adaptive hybrid simulation
./quick_run.sh osm         # OSM-based simulation
./quick_run.sh incident    # Incident response
./quick_run.sh signal      # Traffic signal optimization
./quick_run.sh gpu         # GPU acceleration demo
./quick_run.sh web         # Start web UI
./quick_run.sh test        # Run all tests
```

---

### 🌐 `run_web_ui.sh` - Web Interface

**Best for:** Interactive visualization, real-time control

```bash
cd cpp/jamfree
./run_web_ui.sh
```

Then open: http://localhost:5001

**Features:**
- Interactive map visualization
- Real-time OSM data loading
- GPU acceleration toggle
- Performance metrics dashboard
- Configuration controls

---

## Build Scripts

### Full Build
```bash
./build.sh
```
Builds everything: C++ core + Python bindings

### Python Only
```bash
./build_python.sh
```
Faster rebuild when only Python bindings changed

### With GPU Support (macOS only)
```bash
./build_metal.sh
```
Includes Apple Metal GPU acceleration

---

## Usage Examples

### Run a Quick Test
```bash
cd cpp/jamfree
./quick_run.sh adaptive
```

### Interactive Exploration
```bash
cd cpp/jamfree
./run_simulation.sh
# Select option from menu
```

### Start Web UI
```bash
cd cpp/jamfree
./run_web_ui.sh
# Open browser to http://localhost:5001
```

### Run All Tests
```bash
cd cpp/jamfree
./quick_run.sh test
```

### Automation Script Example
```bash
#!/bin/bash
cd cpp/jamfree

# Run multiple simulations
for sim in adaptive osm incident signal; do
    echo "Running $sim simulation..."
    ./quick_run.sh $sim > results_$sim.log 2>&1
done

echo "All simulations complete!"
```

---

## Script Locations

```
cpp/jamfree/
├── run_simulation.sh          # Interactive menu
├── quick_run.sh               # Command-line runner
├── run_web_ui.sh              # Web UI launcher
├── build.sh                   # Full build
├── build_python.sh            # Python bindings build
├── build_metal.sh             # GPU build (macOS)
└── python/web/
    └── start.sh               # Web UI startup script
```

---

## Environment Variables

Customize simulations with environment variables:

```bash
# Number of vehicles
export JAMFREE_NUM_VEHICLES=1000

# Enable GPU
export JAMFREE_USE_GPU=1

# Simulation time step
export JAMFREE_DT=0.1

# Run simulation
./quick_run.sh adaptive
```

---

## Troubleshooting

### Scripts Not Executable
```bash
chmod +x *.sh
chmod +x python/web/*.sh
```

### JamFree Not Found
```bash
./build_python.sh
# or
./build.sh
```

### Web UI Dependencies Missing
```bash
cd python/web
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

---

## Performance Comparison

| Simulation Type | Vehicles | Speedup | Script |
|----------------|----------|---------|--------|
| Adaptive Hybrid | 500 | 8.5x | `./quick_run.sh adaptive` |
| GPU Accelerated | 10,000 | 50-70x | `./quick_run.sh gpu` |
| Standard Micro | 500 | 1x | N/A (baseline) |

---

## Next Steps

1. **First Time:**
   ```bash
   ./build.sh
   ./run_simulation.sh
   ```

2. **Quick Test:**
   ```bash
   ./quick_run.sh adaptive
   ```

3. **Web Interface:**
   ```bash
   ./run_web_ui.sh
   ```

4. **GPU Demo (macOS):**
   ```bash
   ./build_metal.sh
   ./quick_run.sh gpu
   ```

---

## Documentation

- `SIMULATION_SCRIPTS.md` - Detailed script documentation
- `QUICK_START.md` - Getting started guide
- `examples/python/README.md` - Python examples
- `GPU_WEB_INTEGRATION.md` - GPU and Web UI guide

---

## Support

For detailed documentation, see:
- `SIMULATION_SCRIPTS.md` - Full script reference
- `TEST_RESULTS_SUMMARY.md` - Test results
- `examples/python/` - Example scripts
