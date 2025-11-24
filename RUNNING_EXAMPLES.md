# Running SIMILAR2Logo and JamFree Examples

Complete guide to running all examples in the SIMILAR/JamFree project.

## 📋 Table of Contents

- [Prerequisites](#prerequisites)
- [SIMILAR2Logo Examples](#similar2logo-examples)
- [JamFree Examples](#jamfree-examples)
- [Troubleshooting](#troubleshooting)
- [Performance Tips](#performance-tips)

## Prerequisites

### System Requirements

- **Python**: 3.8 or higher
- **C++ Compiler**: GCC 9+, Clang 10+, or MSVC 2019+
- **CMake**: 3.15 or higher (for building C++ components)
- **Operating System**: Linux, macOS, or Windows

### Installation

```bash
# Clone repository
git clone https://github.com/gildasmorvan/similar
cd similar

# Install Python dependencies
pip install -r requirements.txt

# Build C++ engine (HIGHLY RECOMMENDED for performance)
./build_cpp_engine.sh
```

### Verify Installation

```bash
# Test C++ engine
SIMILAR_VERBOSE=1 python3 examples/python/simple_random_walk.py

# You should see:
# ✓ Using C++ reaction engine for optimal performance
```

## SIMILAR2Logo Examples

All examples are in `examples/python/`. Each can run standalone.

### Basic Examples

#### 1. Simple Random Walk

**What it does**: Single turtle performing random walk

```bash
python3 examples/python/simple_random_walk.py
```

**Parameters to modify**:
```python
# In the file
GRID_SIZE = 100        # Environment size
STEPS = 1000           # Simulation steps
TURN_PROBABILITY = 0.3 # How often to turn
```

#### 2. Circle Formation

**What it does**: Agents form circular patterns

```bash
python3 examples/python/circle.py
```

**Web interface**: Opens automatically on http://localhost:8080

#### 3. Passive Turtle

**What it does**: Physics-based movement without decision-making

```bash
python3 examples/python/passive_turtle.py
```

### Flocking & Collective Behavior

#### 4. Boids (Classic Flocking)

**What it does**: Reynolds' flocking algorithm with cohesion, alignment, separation

```bash
# DSL version (recommended)
python3 examples/python/boids_dsl.py

# Original version
python3 examples/python/boids.py

# Advanced flocking
python3 examples/python/boids_flocking.py
```

**Parameters**:
```python
NUM_BOIDS = 100              # Number of agents
PERCEPTION_RADIUS = 15.0     # How far they see
COHESION_WEIGHT = 0.01       # Attraction to center
ALIGNMENT_WEIGHT = 0.05      # Match heading
SEPARATION_WEIGHT = 0.1      # Avoid crowding
```

**Web UI**: http://localhost:8080

#### 5. Boids with Obstacles

**What it does**: Flocking with obstacle avoidance

```bash
python3 examples/python/boids_obstacles.py
```

**Features**:
- Dynamic obstacle placement
- Collision avoidance
- Path planning around barriers

### Biological & Ecological Models

#### 6. Ant Foraging

**What it does**: Ants find food using pheromone trails

```bash
# DSL version (recommended)
python3 examples/python/ant_foraging_dsl.py

# Original version
python3 examples/python/ant_foraging.py
```

**Parameters**:
```python
NUM_ANTS = 50
PHEROMONE_EVAPORATION = 0.01
PHEROMONE_DIFFUSION = 0.1
FOOD_SOURCES = 3
```

**What to observe**:
- Pheromone trail formation
- Shortest path emergence
- Collective foraging efficiency

#### 7. Virus Spread (Epidemiology)

**What it does**: SIR model of disease transmission

```bash
python3 examples/python/virus_spread.py
```

**Parameters**:
```python
POPULATION = 200
INITIALLY_INFECTED = 5
INFECTION_RADIUS = 3.0
INFECTION_PROBABILITY = 0.3
RECOVERY_TIME = 100  # steps
```

**States**:
- 🟢 Green = Susceptible
- 🔴 Red = Infected
- 🔵 Blue = Recovered

#### 8. Predator-Prey

**What it does**: Lotka-Volterra population dynamics

```bash
python3 examples/python/predator_prey.py
```

**Parameters**:
```python
NUM_PREY = 100
NUM_PREDATORS = 20
PREY_REPRODUCTION_RATE = 0.05
PREDATOR_DEATH_RATE = 0.02
```

**What to observe**:
- Population cycles
- Predator-prey oscillations
- Equilibrium dynamics

#### 9. Heatbugs

**What it does**: Bugs seeking optimal temperature

```bash
python3 examples/python/heatbugs.py
```

**Features**:
- Heat diffusion
- Temperature-seeking behavior
- Self-organizing patterns

### Environmental Simulations

#### 10. Forest Fire

**What it does**: Fire spreading through forest (cellular automaton)

```bash
python3 examples/python/forest_fire.py
```

**Parameters**:
```python
FOREST_DENSITY = 0.6
FIRE_SPREAD_PROBABILITY = 0.5
WIND_DIRECTION = 0  # radians
```

#### 11. Pheromone Following

**What it does**: Agents following chemical trails

```bash
python3 examples/python/pheromone_following.py
```

#### 12. Slime Mold

**What it does**: Physarum polycephalum network formation

```bash
python3 examples/python/slime_mold.py
```

**Features**:
- Bio-inspired algorithm
- Network optimization
- Shortest path finding

### Artificial Life & Patterns

#### 13. Turmite (Langton's Ant)

**What it does**: Single ant creating emergent patterns

```bash
python3 examples/python/turmite.py
```

**What to observe**:
- Initial chaos
- Highway formation (~10,000 steps)
- Emergent complexity

#### 14. Multi-Turmite

**What it does**: Multiple Langton's ants interacting

```bash
python3 examples/python/multiturmite.py
```

**Parameters**:
```python
NUM_TURMITES = 5
GRID_SIZE = 200
```

#### 15. Firework

**What it does**: Particle system with physics

```bash
python3 examples/python/firework.py
```

**Features**:
- Gravity simulation
- Particle lifetime
- Visual effects

### Social & Urban Models

#### 16. Segregation Model

**What it does**: Schelling's segregation model

```bash
python3 examples/python/segregation_model.py
```

**Parameters**:
```python
POPULATION = 400
SIMILARITY_THRESHOLD = 0.3  # Tolerance level
EMPTY_RATIO = 0.1
```

**What to observe**:
- Tipping point dynamics
- Emergent segregation
- Effect of tolerance threshold

#### 17. Transport

**What it does**: Vehicle traffic simulation

```bash
python3 examples/python/transport.py
```

**Features**:
- Road networks
- Collision avoidance
- Traffic flow

### Running All Examples

```bash
# Run all examples sequentially (for testing)
python3 examples/python/run_examples.py

# Run specific category
python3 examples/python/run_examples.py --category flocking
```

### Using Shell Scripts

```bash
# Boids with optimal settings
./examples/python/run_boids_dsl.sh

# Ant foraging
./examples/python/run_ant_foraging_dsl.sh

# Test all examples
./examples/python/test_all.sh
```

## JamFree Examples

JamFree examples are in `cpp/jamfree/examples/`.

### Setup

```bash
cd cpp/jamfree

# Build JamFree
./build.sh

# Build Python bindings
./build_python.sh

# (Optional) Build with GPU support (macOS only)
./build_metal.sh
```

### Web Interface (Recommended)

The easiest way to use JamFree:

```bash
cd cpp/jamfree
./run_web_ui.sh

# Or manually:
python python/web/app.py
```

Then open http://localhost:5001

**Features**:
1. **Load Network**:
   - Upload OSM file
   - Or download area from map
2. **Configure Simulation**:
   - Number of vehicles
   - Desired speed
   - Enable/disable optimizations
3. **Run**:
   - Click "Start"
   - Watch real-time visualization
   - Monitor performance metrics

### Command-Line Examples

#### 1. Basic Simulation

```bash
cd cpp/jamfree
python3 examples/basic_simulation.py
```

**What it does**:
- Creates simple road network
- Spawns vehicles with IDM model
- Runs microscopic simulation

#### 2. OSM Integration

```bash
python3 examples/osm_simulation.py --file path/to/map.osm
```

**What it does**:
- Loads real-world road network
- Parses lanes and speed limits
- Runs realistic simulation

#### 3. Hybrid Simulation

```bash
python3 examples/hybrid_simulation.py
```

**What it does**:
- Demonstrates micro/macro switching
- Shows performance benefits
- Automatic mode selection

#### 4. GPU Acceleration (macOS only)

```bash
python3 examples/gpu_simulation.py
```

**Requirements**:
- macOS with Metal support
- Built with `./build_metal.sh`

**Performance**: 5-10x faster for large simulations

#### 5. Routing Example

```bash
python3 examples/routing_demo.py
```

**Features**:
- A* pathfinding
- Turn-by-turn navigation
- Lane selection for turns

### Quick Run Script

```bash
cd cpp/jamfree
./quick_run.sh

# Options:
./quick_run.sh --vehicles 500 --adaptive --gpu
```

### Simulation Scripts

```bash
# Run with specific configuration
./run_simulation.sh --config configs/highway.json

# Run batch simulations
./run_simulation.sh --batch experiments/
```

See [cpp/jamfree/SIMULATION_SCRIPTS.md](cpp/jamfree/SIMULATION_SCRIPTS.md) for details.

## Troubleshooting

### "Using Python reaction fallback" Warning

**Problem**: C++ engine not being used

**Solution**:
```bash
# Rebuild C++ engine
./build_cpp_engine.sh

# Verify
SIMILAR_VERBOSE=1 python3 examples/python/boids_dsl.py
```

### Import Errors

**Problem**: `ModuleNotFoundError: No module named 'similar2logo'`

**Solution**:
```bash
# Set PYTHONPATH
export PYTHONPATH=$PWD/python:$PYTHONPATH

# Or install in development mode
pip install -e .
```

### JamFree Build Errors

**Problem**: CMake or compilation errors

**Solution**:
```bash
cd cpp/jamfree

# Clean build
rm -rf build
./build.sh

# Check dependencies
cmake --version  # Should be 3.15+
c++ --version    # Should be C++17 compatible
```

### Web UI Not Loading

**Problem**: Port already in use

**Solution**:
```bash
# SIMILAR2Logo (default port 8080)
# Change in your script:
sim.run_web(port=8081)

# JamFree (default port 5001)
# Edit python/web/app.py, line 945:
app.run(debug=False, host='0.0.0.0', port=5002, threaded=True)
```

### Slow Performance

**Problem**: Simulation running slowly

**Solutions**:

1. **Build C++ engine**:
```bash
./build_cpp_engine.sh
```

2. **Reduce agent count**:
```python
NUM_AGENTS = 100  # Instead of 1000
```

3. **Limit perception radius**:
```python
PERCEPTION_RADIUS = 10.0  # Instead of 50.0
```

4. **Disable web UI for batch runs**:
```python
sim.run(steps=10000)  # Instead of sim.run_web()
```

### Metal GPU Not Available

**Problem**: GPU acceleration not working

**Requirements**:
- macOS only
- Metal-compatible GPU
- Built with `./build_metal.sh`

**Check**:
```python
import jamfree
print(jamfree.is_metal_available())  # Should be True
```

## Performance Tips

### SIMILAR2Logo

1. **Always build C++ engine**: 10-100x speedup
2. **Limit perception**: Smaller radius = faster
3. **Batch processing**: Use `sim.run()` instead of `sim.run_web()`
4. **Profile**: Use `SIMILAR_VERBOSE=1` to see timing

### JamFree

1. **Enable optimizations** in web UI:
   - ✅ IDM Lookup Tables (30-40% faster)
   - ✅ Spatial Index (O(log N) queries)
   - ✅ Adaptive Hybrid (auto micro/macro)
   - ✅ GPU Metal (macOS, 5-10x faster)

2. **Use appropriate model**:
   - Microscopic: < 500 vehicles
   - Hybrid: 500-5000 vehicles
   - Macroscopic: > 5000 vehicles

3. **Optimize network**:
   - Simplify road geometry
   - Reduce lane count where possible
   - Use macroscopic on highways

## Next Steps

1. **Try examples**: Start with simple ones, progress to complex
2. **Modify parameters**: Experiment with different settings
3. **Read code**: Study implementations
4. **Create your own**: Build custom simulations
5. **Contribute**: Share your work!

## Additional Resources

- **SIMILAR2Logo Quick Start**: [QUICKSTART.md](../QUICKSTART.md)
- **JamFree Documentation**: [cpp/jamfree/README.md](../cpp/jamfree/README.md)
- **Examples README**: [examples/python/README.md](../examples/python/README.md)
- **DSL Reference**: [DSL_QUICK_REFERENCE.md](../DSL_QUICK_REFERENCE.md)

---

**Happy Simulating!** 🚀
