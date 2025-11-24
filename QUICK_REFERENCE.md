# SIMILAR & JamFree - Quick Command Reference

One-page reference for the most common commands.

## 🚀 Installation & Setup

```bash
# Clone and setup
git clone https://github.com/gildasmorvan/similar
cd similar
pip install -r requirements.txt

# Build C++ engine (RECOMMENDED)
./build_cpp_engine.sh

# Build JamFree
cd cpp/jamfree && ./build.sh && cd ../..
```

## 🐢 SIMILAR2Logo - Quick Commands

### Run Examples

```bash
# Basic examples
python3 examples/python/simple_random_walk.py
python3 examples/python/circle.py
python3 examples/python/boids_dsl.py

# With C++ verification
SIMILAR_VERBOSE=1 python3 examples/python/boids_dsl.py

# Run all examples
python3 examples/python/run_examples.py
```

### Create New Simulation

```python
from similar2logo.dsl import *

class MyTurtle(Turtle):
    def decide(self, perception):
        return [self.influence_move_forward(1.0)]

env = Environment(100, 100, toroidal=True)
sim = Simulation(environment=env)
sim.add_agent(MyTurtle(position=env.random_position()))
sim.run_web(port=8080)  # Or sim.run(steps=1000)
```

### Common Influences

```python
# Movement
self.influence_move_forward(distance)
self.influence_move_to(position)

# Rotation
self.influence_turn(angle)
self.influence_turn_towards(heading)

# Speed
self.influence_set_speed(speed)
self.influence_change_speed(delta)

# Environment
self.influence_emit_pheromone(id, amount)
self.influence_stop()
```

## 🚗 JamFree - Quick Commands

### Web Interface (Easiest)

```bash
cd cpp/jamfree
./run_web_ui.sh
# Open http://localhost:5001
```

### Command Line

```bash
cd cpp/jamfree

# Basic simulation
python3 examples/basic_simulation.py

# OSM simulation
python3 examples/osm_simulation.py --file map.osm

# Hybrid (micro/macro)
python3 examples/hybrid_simulation.py

# GPU accelerated (macOS)
python3 examples/gpu_simulation.py
```

### Quick Run

```bash
cd cpp/jamfree
./quick_run.sh --vehicles 500 --adaptive --gpu
```

### Python API

```python
import jamfree

# Create network
network = jamfree.OSMParser.parse_file("map.osm")

# Create vehicle with IDM
vehicle = jamfree.Vehicle("v1")
idm = jamfree.IDM(
    desired_speed=33.3,  # m/s (120 km/h)
    time_headway=1.5,
    min_gap=2.0
)

# Run simulation step
dt = 0.1  # seconds
acc = idm.calculate_acceleration(vehicle, leader)
vehicle.update(dt, acc)
```

## 🔧 Build Commands

### SIMILAR2Logo C++ Engine

```bash
# Build
./build_cpp_engine.sh

# Verify
SIMILAR_VERBOSE=1 python3 examples/python/boids_dsl.py
# Should see: ✓ Using C++ reaction engine
```

### JamFree

```bash
cd cpp/jamfree

# Core library
./build.sh

# With Python bindings
./build_python.sh

# With GPU support (macOS)
./build_metal.sh

# Clean build
rm -rf build && ./build.sh
```

## 🐛 Troubleshooting

### C++ Engine Not Working

```bash
# Rebuild
./build_cpp_engine.sh

# Check
SIMILAR_VERBOSE=1 python3 examples/python/simple_random_walk.py
```

### Import Errors

```bash
# Set PYTHONPATH
export PYTHONPATH=$PWD/python:$PYTHONPATH

# Or install
pip install -e .
```

### Port Already in Use

```python
# SIMILAR2Logo
sim.run_web(port=8081)  # Change from 8080

# JamFree - edit python/web/app.py line 945
app.run(port=5002)  # Change from 5001
```

## 📊 Performance

### Check Performance

```bash
# SIMILAR2Logo
SIMILAR_VERBOSE=1 python3 your_sim.py

# JamFree - enable in web UI or:
python3 examples/hybrid_simulation.py  # Shows stats
```

### Optimize

**SIMILAR2Logo:**
- Build C++ engine (10-100x faster)
- Reduce perception radius
- Fewer agents
- Use `sim.run()` instead of `sim.run_web()`

**JamFree:**
- Enable IDM Lookup (30-40% faster)
- Enable Spatial Index (O(log N) queries)
- Enable Adaptive Hybrid (auto switching)
- Enable GPU Metal (macOS, 5-10x faster)

## 📖 Documentation

```bash
# Main README
cat README.md

# Quick start
cat QUICKSTART.md

# Running examples
cat RUNNING_EXAMPLES.md

# JamFree docs
cat cpp/jamfree/README.md
cat cpp/jamfree/QUICK_START.md

# Examples
cat examples/python/README.md
```

## 🎯 Common Tasks

### Run Boids Simulation

```bash
python3 examples/python/boids_dsl.py
# Open http://localhost:8080
```

### Run Traffic Simulation

```bash
cd cpp/jamfree
./run_web_ui.sh
# Open http://localhost:5001
# Click "Download Area" or upload OSM file
# Click "Start"
```

### Create Custom Simulation

```bash
# Copy template
cp examples/python/simple_random_walk.py my_sim.py

# Edit my_sim.py
# Run
python3 my_sim.py
```

### Batch Processing

```python
# No visualization
sim.run(steps=10000)

# Access results
for turtle in sim.turtles:
    print(turtle.position)
```

## 🔗 Quick Links

- **Main README**: [README.md](README.md)
- **Quick Start**: [QUICKSTART.md](QUICKSTART.md)
- **Running Examples**: [RUNNING_EXAMPLES.md](RUNNING_EXAMPLES.md)
- **JamFree Docs**: [cpp/jamfree/README.md](cpp/jamfree/README.md)
- **Examples**: [examples/python/README.md](examples/python/README.md)

## 💡 Tips

- **Start simple**: Try `simple_random_walk.py` first
- **Use C++**: Always build the C++ engine
- **Check verbose**: Use `SIMILAR_VERBOSE=1` to debug
- **Web UI**: Great for visualization
- **Read examples**: 18+ working examples to learn from

---

**Quick Help**: `python3 examples/python/run_examples.py --help`
