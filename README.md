# SIMILAR & JamFree: High-Performance Agent-Based Simulation Framework

[![License](https://img.shields.io/badge/License-CeCILL--B-blue.svg)](LICENSE.txt)
[![Python](https://img.shields.io/badge/Python-3.8+-green.svg)](https://www.python.org/)
[![C++](https://img.shields.io/badge/C++-17-orange.svg)](https://isocpp.org/)

**SIMILAR** is a powerful agent-based modeling framework with a Python DSL backed by a high-performance C++ engine. **JamFree** is a traffic simulation framework built on SIMILAR, featuring microscopic and macroscopic models with real-time data integration.

## ✨ Key Enhancements (vs Java SIMILAR)

- 🚀 **10-50x Performance**: Multi-threaded C++ engine with FastMath optimizations
- 🐍 **Python DSL**: Intuitive fluent API replacing verbose Java builders
- 🌐 **Advanced Web UI**: Real-time visualization with interactive controls
- 📦 **One-command Install**: `pip install similar2logo` includes everything
- 🔧 **Cross-platform**: Native binaries for Windows, macOS, and Linux

**[View all enhancements →](docs/ENHANCEMENTS.md)**

## 🚀 Quick Start

### Installation

#### Option 1: Install from PyPI (Recommended)

```bash
pip3 install similar2logo
```

This installs the latest stable version with pre-compiled C++ extensions for optimal performance.

#### Option 2: Install from Source

```bash
# Clone the repository
git clone https://github.com/gildasmorvan/similar
cd similar

# Build C++ engine (for development or custom builds)
./build_cpp_engine.sh

# Install Python package
pip install -e .
```

**📖 New to SIMILAR?** See **[Getting Started Guide](GETTING_STARTED.md)** for a complete walkthrough!

**⚡ Quick Commands?** Check **[Quick Reference](QUICK_REFERENCE.md)** for a command cheat sheet!


### Your First Simulation (SIMILAR2Logo)

Create `my_first_sim.py`:

```python
from similar2logo.dsl import *
import random

class RandomWalker(Turtle):
    def decide(self, perception):
        influences = []
        if random.random() < 0.3:
            influences.append(self.influence_turn(random.uniform(-0.5, 0.5)))
        influences.append(self.influence_move_forward(1.0))
        return influences

# Create environment and simulation
env = Environment(100, 100, toroidal=True)
sim = Simulation(environment=env, title="Random Walk")

# Add 50 agents
for _ in range(50):
    sim.add_agent(RandomWalker(
        position=env.random_position(),
        heading=env.random_heading()
    ))

# Run with web interface
sim.run_web(port=8080)
```

Run it:
```bash
python3 my_first_sim.py
# Open http://localhost:8080
```

### Your First Traffic Simulation (JamFree)

```bash
cd cpp/jamfree

# Build JamFree
./build.sh

# Run web interface
./run_web_ui.sh
# Open http://localhost:5001
```

## 📚 What's Included

### SIMILAR2Logo - Agent-Based Modeling

A Python DSL for creating agent-based simulations with:

- **🐢 Turtle Agents**: Logo-style agents with perception and decision-making
- **🌍 Environments**: Toroidal/bounded worlds with pheromones and obstacles
- **🧠 Influences**: Clean action system (move, turn, emit signals)
- **⚡ C++ Engine**: Automatic 10-100x speedup when available
- **🌐 Web UI**: Real-time visualization in browser
- **📊 18+ Examples**: From flocking to epidemiology

**Examples**: Boids, Ant Foraging, Virus Spread, Segregation, Predator-Prey, and more!

### JamFree - Traffic Simulation

A comprehensive traffic simulation framework featuring:

- **🚗 Microscopic Models**: IDM car-following, MOBIL lane-changing
- **🌊 Macroscopic Models**: LWR, CTM continuum flow
- **🔄 Hybrid Simulation**: Automatic micro/macro switching
- **🗺️ OSM Integration**: Real-world road networks
- **📡 Real-Time Data**: TomTom, HERE, OpenTraffic integration
- **🎯 Routing**: A* pathfinding with turn-by-turn navigation
- **⚡ GPU Acceleration**: Metal GPU support (macOS)
- **🌐 Web Interface**: Interactive visualization

## 🎯 Key Features

### Performance

- **C++ Engine**: Core simulation logic in optimized C++
- **Python DSL**: Easy-to-use Python interface
- **Parallel Processing**: Multi-threaded simulation
- **GPU Support**: Metal acceleration for large-scale traffic sims
- **Adaptive Hybrid**: Automatic micro/macro switching for optimal speed

### Flexibility

- **Multiple Paradigms**: Microscopic, macroscopic, hybrid
- **Real Data**: OSM maps, traffic APIs, real-time integration
- **Extensible**: Easy to add new models and behaviors
- **Cross-Platform**: Linux, macOS, Windows

### Usability

- **Clean DSL**: Intuitive Python API
- **Web Visualization**: No GUI installation needed
- **Rich Examples**: 18+ SIMILAR2Logo + traffic examples
- **Documentation**: Comprehensive guides and tutorials

## 📖 Documentation

**📚 [Complete Documentation Index](DOCUMENTATION_INDEX.md)** - Find any document quickly!

### SIMILAR2Logo

- **[Quick Start Guide](QUICKSTART.md)** - Get started in 5 minutes
- **[Examples README](examples/python/README.md)** - All 18 examples explained
- **[DSL Reference](DSL_QUICK_REFERENCE.md)** - API documentation
- **[Implementation Status](IMPLEMENTATION_STATUS.md)** - What's implemented

### JamFree

- **[JamFree README](cpp/jamfree/README.md)** - Overview and concepts
- **[Quick Start](cpp/jamfree/QUICK_START.md)** - Get running fast
- **[Web UI Guide](cpp/jamfree/WEB_UI_SUMMARY.md)** - Using the web interface
- **[Performance Guide](cpp/jamfree/ADVANCED_PERFORMANCE.md)** - Optimization tips
- **[Routing & Traffic Data](cpp/jamfree/REALISTIC_SIMULATION.md)** - Real-world integration

## 🎨 Examples

### SIMILAR2Logo Examples

All located in `examples/python/`:

**Basic**
- `simple_random_walk.py` - Basic movement
- `circle.py` - Formation patterns
- `passive_turtle.py` - Physics simulation

**Flocking & Collective**
- `boids_dsl.py` - Classic flocking behavior
- `boids_obstacles.py` - Flocking with obstacle avoidance
- `ant_foraging_dsl.py` - Pheromone-based foraging

**Biological**
- `virus_spread.py` - Epidemiology (SIR model)
- `predator_prey.py` - Lotka-Volterra dynamics
- `heatbugs.py` - Temperature-seeking agents

**Social & Urban**
- `segregation_model.py` - Schelling's model
- `transport.py` - Traffic simulation

**Patterns & Emergence**
- `turmite.py` - Langton's Ant
- `multiturmite.py` - Multiple ants
- `forest_fire.py` - Cellular automaton

**And more!** See [examples/python/README.md](examples/python/README.md) for the complete list.

### JamFree Examples

Located in `cpp/jamfree/examples/`:

- **Basic Simulation** - Simple traffic flow
- **OSM Integration** - Real-world maps
- **Hybrid Simulation** - Micro/macro switching
- **GPU Acceleration** - Metal-accelerated simulation
- **Routing** - Path planning and navigation

## 🏃 Running Examples

### SIMILAR2Logo

```bash
# Run any example
python3 examples/python/boids_dsl.py

# With C++ engine verification
SIMILAR_VERBOSE=1 python3 examples/python/virus_spread.py

# Run all examples (testing)
python3 examples/python/run_examples.py
```

### JamFree

```bash
cd cpp/jamfree

# Web interface (recommended)
./run_web_ui.sh
# Then open http://localhost:5001

# Command-line simulation
python3 examples/basic_simulation.py

# With performance stats
python3 examples/hybrid_simulation.py
```

## 🔧 Building from Source

### SIMILAR2Logo C++ Engine

```bash
# Build C++ engine
./build_cpp_engine.sh

# Verify it works
SIMILAR_VERBOSE=1 python3 examples/python/boids_dsl.py
# Should see: ✓ Using C++ reaction engine
```

### JamFree

```bash
cd cpp/jamfree

# Build core library
./build.sh

# Build with Python bindings
./build_python.sh

# Build with GPU support (macOS only)
./build_metal.sh
```

## 🌐 Web Interfaces

### SIMILAR2Logo Web UI

Most examples include a built-in web interface:

```python
# In your simulation
sim.run_web(port=8080, steps_per_frame=1)
```

Then open http://localhost:8080

### JamFree Web UI

Full-featured traffic simulation interface:

```bash
cd cpp/jamfree
./run_web_ui.sh
```

Features:
- 🗺️ Load OSM maps (upload or download)
- 🚗 Configure vehicles and parameters
- ⚡ Enable/disable optimizations
- 📊 Real-time performance metrics
- 🎨 Lane mode visualization (micro/macro)

## 📊 Performance

### SIMILAR2Logo

With C++ engine enabled:
- **10-100x faster** than pure Python
- Handles **1000+ agents** in real-time
- Efficient spatial indexing
- Optimized influence resolution

### JamFree

Performance optimizations:
- **IDM Lookup Tables**: 30-40% faster car-following
- **Spatial Indexing**: O(log N) neighbor queries
- **Adaptive Hybrid**: Auto micro/macro switching
- **GPU Acceleration**: Metal support for macOS
- **Multithreading**: Parallel vehicle updates

Example: 1000 vehicles on real OSM map at 10+ FPS

## 🔬 Research & Education

Perfect for:
- **Agent-Based Modeling** courses
- **Complex Systems** research
- **Traffic Engineering** studies
- **Artificial Life** experiments
- **Swarm Intelligence** research
- **Computational Biology** teaching

## 🤝 Contributing

We welcome contributions! To contribute:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests and documentation
5. Submit a pull request

See individual component READMEs for specific guidelines.

## 📄 License

This project is licensed under the **CeCILL-B** license - see [LICENSE.txt](LICENSE.txt) for details.

CeCILL-B is a BSD-like license compatible with international and French law.

## 👥 Authors

- **Gildas Morvan** - Original SIMILAR framework
- **Contributors** - See [AUTHORS.txt](AUTHORS.txt)

## 🔗 Resources

### Documentation
- [SIMILAR2Logo Quick Start](QUICKSTART.md)
- [JamFree Documentation](cpp/jamfree/README.md)
- [Examples Collection](examples/python/README.md)

### Academic References
- **Traffic Flow Dynamics** - Treiber & Kesting (2013)
- **MOBIL Model** - Kesting et al. (2007)
- **Agent-Based Modeling** - Wilensky & Rand (2015)

### Related Projects
- [SUMO](https://eclipse.dev/sumo/) - Traffic simulation
- [MATSim](https://www.matsim.org/) - Multi-agent transport
- [NetLogo](https://ccl.northwestern.edu/netlogo/) - Agent-based modeling

## 🎯 Project Structure

```
similar/
├── cpp/
│   ├── similar2logo/          # C++ engine for SIMILAR2Logo
│   └── jamfree/               # JamFree traffic simulation
│       ├── kernel/            # Core models (Vehicle, Lane, Road)
│       ├── microscopic/       # IDM, MOBIL
│       ├── macroscopic/       # LWR, CTM
│       ├── hybrid/            # Adaptive simulation
│       ├── gpu/               # Metal GPU acceleration
│       ├── realdata/          # OSM parser, traffic APIs
│       ├── python/            # Python bindings
│       └── examples/          # Example simulations
├── python/
│   └── similar2logo/          # Python DSL
│       ├── dsl/               # High-level API
│       ├── model.py           # Core classes
│       └── parallel.py        # Parallel engine
├── examples/
│   └── python/                # 18+ example simulations
└── docs/                      # Additional documentation
```

## 🚦 Status

### SIMILAR2Logo
✅ **Complete** - All Java examples ported to Python DSL with C++ engine

### JamFree
✅ **Production Ready**
- Microscopic simulation (IDM, MOBIL)
- Macroscopic models (LWR, CTM)
- Hybrid adaptive simulation
- OSM integration
- Web UI with visualization
- GPU acceleration (macOS)
- Real-time traffic data integration

## 🎉 Getting Help

- **Issues**: [GitHub Issues](https://github.com/gildasmorvan/similar/issues)
- **Examples**: Check `examples/python/` for working code
- **Documentation**: See individual README files
- **Verbose Mode**: Use `SIMILAR_VERBOSE=1` to see what's happening

## 💡 Tips

- **Start simple**: Try `simple_random_walk.py` first
- **Use C++ engine**: Build it for major speedup
- **Check examples**: 18+ working examples to learn from
- **Web UI**: Great for visualization and debugging
- **Read docs**: Each component has detailed documentation

---

**Happy Simulating!** 🚀

For more information, see:
- [SIMILAR2Logo Quick Start](QUICKSTART.md)
- [JamFree Quick Start](cpp/jamfree/QUICK_START.md)
- [Examples Guide](examples/python/README.md)
