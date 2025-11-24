# JamFree Simulation Scripts

This directory contains several bash scripts to help you run and configure JamFree simulations.

## Quick Start

### Interactive Menu (Recommended for Beginners)
```bash
./run_simulation.sh
```

This launches an interactive menu where you can:
- Run any Python example simulation
- Start the Web UI
- Build JamFree components
- Run all tests

### Command-Line Usage (For Automation)
```bash
./quick_run.sh [simulation]
```

Examples:
```bash
./quick_run.sh adaptive    # Run adaptive hybrid simulation
./quick_run.sh osm         # Run OSM-based simulation
./quick_run.sh gpu         # Run GPU acceleration demo
./quick_run.sh web         # Start web UI
./quick_run.sh test        # Run all tests
```

## Available Scripts

### Main Runner Scripts

| Script | Description | Usage |
|--------|-------------|-------|
| `run_simulation.sh` | Interactive menu for all simulations | `./run_simulation.sh` |
| `quick_run.sh` | Command-line runner for automation | `./quick_run.sh [name]` |
| `run_web_ui.sh` | Start the web UI directly | `./run_web_ui.sh` |

### Build Scripts

| Script | Description | When to Use |
|--------|-------------|-------------|
| `build.sh` | Full build (C++ + Python) | First time setup |
| `build_python.sh` | Python bindings only | After C++ code changes |
| `build_metal.sh` | Build with Metal GPU support | For GPU acceleration (macOS only) |

### Web UI Scripts

| Script | Description | Location |
|--------|-------------|----------|
| `python/web/start.sh` | Web UI startup with dependency check | `python/web/` |

## Simulation Examples

### 1. Adaptive Hybrid Simulation
Demonstrates automatic switching between microscopic and macroscopic modes based on traffic density.

```bash
./quick_run.sh adaptive
# or
python3 examples/python/adaptive_hybrid_example.py
```

**Features:**
- Automatic mode switching
- 8.5x speedup on average
- Real-time performance metrics

### 2. OSM Hybrid Simulation
Runs simulation on real-world road networks from OpenStreetMap.

```bash
./quick_run.sh osm
# or
python3 examples/python/osm_hybrid_simulation.py
```

**Features:**
- Real OSM network data
- Adaptive simulation on realistic roads
- Critical area detection

### 3. Incident Response Simulation
Simulates traffic accidents and demonstrates forced microscopic mode.

```bash
./quick_run.sh incident
# or
python3 examples/python/incident_response.py
```

**Features:**
- Accident simulation
- Forced microscopic mode for critical lanes
- Queue formation analysis

### 4. Traffic Signal Optimization
Demonstrates "split lane" strategy for traffic light optimization.

```bash
./quick_run.sh signal
# or
python3 examples/python/traffic_signal_optimization.py
```

**Features:**
- Split lane approach
- Queue discharge dynamics
- Signal timing optimization

### 5. GPU Metal Acceleration Demo
Benchmarks CPU vs GPU performance for large-scale simulations.

```bash
./quick_run.sh gpu
# or
python3 examples/python/gpu_metal_example.py
```

**Features:**
- CPU vs GPU comparison
- Up to 70x speedup for 10,000+ vehicles
- Metal GPU integration (macOS only)

### 6. Web UI
Interactive web-based simulation interface.

```bash
./quick_run.sh web
# or
./run_web_ui.sh
```

**Features:**
- Interactive map visualization
- Real-time OSM data loading
- GPU acceleration toggle
- Performance monitoring

## Configuration Options

### Environment Variables

You can customize simulations using environment variables:

```bash
# Set number of vehicles
export JAMFREE_NUM_VEHICLES=500

# Enable GPU acceleration
export JAMFREE_USE_GPU=1

# Set simulation time step
export JAMFREE_DT=0.1

# Run simulation
./quick_run.sh adaptive
```

### Python Configuration

For more control, modify the example scripts directly or create your own:

```python
import jamfree

# Configure adaptive simulator
config = jamfree.AdaptiveSimulatorConfig()
config.micro_to_macro_density = 0.08  # Switch to macro at 80 veh/km
config.macro_to_micro_density = 0.04  # Switch to micro at 40 veh/km
config.micro_to_macro_count = 50      # Or at 50 vehicles per lane

# Create simulator
simulator = jamfree.AdaptiveSimulator(config)
```

## Building JamFree

### First Time Setup
```bash
./build.sh
```

This builds:
- C++ core library
- Python bindings
- All examples

### Rebuild After Changes
```bash
# Python bindings only (faster)
./build_python.sh

# Full rebuild
./build.sh
```

### GPU Support (macOS only)
```bash
./build_metal.sh
```

This builds everything with Metal GPU acceleration support.

## Testing

### Run All Tests
```bash
./quick_run.sh test
```

### Run Specific Test
```bash
python3 examples/python/adaptive_hybrid_example.py
```

### Web UI Tests
```bash
cd python/web
source venv/bin/activate
python3 test_all_optimizations.py
```

## Troubleshooting

### "JamFree not found"
```bash
# Build Python bindings
./build_python.sh

# Or full build
./build.sh
```

### "Flask not found" (Web UI)
```bash
cd python/web
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

### "Metal not available"
Metal GPU is only available on macOS. The simulations will still work using CPU.

### Permission Denied
```bash
chmod +x *.sh
chmod +x python/web/*.sh
```

## Performance Tips

1. **Use GPU for large simulations** (10,000+ vehicles)
   - Enable in Web UI or set `use_gpu=True` in Python

2. **Use Adaptive Hybrid for mixed traffic**
   - Automatically optimizes based on density
   - 8-10x speedup typical

3. **Use IDM Lookup Tables**
   - 30-40% faster than standard IDM
   - Enabled by default

4. **Enable Spatial Index**
   - O(log N) instead of O(N) queries
   - Enabled by default

## Documentation

- `README.md` - Main project documentation
- `QUICK_START.md` - Quick start guide
- `GPU_WEB_INTEGRATION.md` - GPU and Web UI integration
- `ADAPTIVE_HYBRID_QUICKSTART.md` - Adaptive simulation guide
- `examples/python/README.md` - Python examples documentation

## Support

For issues or questions:
1. Check the documentation in the `docs/` directory
2. Review example scripts in `examples/python/`
3. Check test results in `TEST_RESULTS_SUMMARY.md`

## License

See LICENSE file in the project root.
