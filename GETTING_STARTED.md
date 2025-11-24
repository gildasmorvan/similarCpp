# Getting Started with SIMILAR & JamFree

Welcome! This guide will get you up and running with SIMILAR2Logo and JamFree in under 10 minutes.

## 📋 What You'll Learn

1. Install and setup the project
2. Run your first SIMILAR2Logo simulation
3. Run your first JamFree traffic simulation
4. Understand the basic concepts
5. Know where to go next

## ⚡ Quick Install (5 minutes)

### Step 1: Clone Repository

```bash
git clone https://github.com/gildasmorvan/similar
cd similar
```

### Step 2: Install Python Dependencies

```bash
pip install -r requirements.txt
```

That's it! You can now run Python-only simulations.

### Step 3: Build C++ Engine (Optional but HIGHLY Recommended)

For 10-100x performance boost:

```bash
./build_cpp_engine.sh
```

**Requirements**: C++17 compiler (GCC 9+, Clang 10+, or MSVC 2019+)

### Step 4: Verify Installation

```bash
SIMILAR_VERBOSE=1 python3 examples/python/simple_random_walk.py
```

**Success**: You should see `✓ Using C++ reaction engine`

**Warning**: If you see `⚠️ Using Python reaction model`, the C++ engine isn't built (still works, just slower)

## 🐢 Your First SIMILAR2Logo Simulation (2 minutes)

### Run a Pre-Built Example

```bash
python3 examples/python/boids_dsl.py
```

A browser window should open showing flocking birds!

**What you're seeing**:
- 100 agents (boids) following three simple rules
- Cohesion: move toward neighbors
- Alignment: match neighbor heading
- Separation: avoid crowding

### Create Your Own Simulation

Create `my_first_sim.py`:

```python
from similar2logo.dsl import *
import random

class RandomWalker(Turtle):
    """A turtle that walks randomly"""
    
    def decide(self, perception):
        influences = []
        
        # 30% chance to turn
        if random.random() < 0.3:
            influences.append(
                self.influence_turn(random.uniform(-0.5, 0.5))
            )
        
        # Always move forward
        influences.append(self.influence_move_forward(1.0))
        
        return influences

# Create 100x100 environment with wrap-around edges
env = Environment(100, 100, toroidal=True)

# Create simulation
sim = Simulation(environment=env, title="My First Simulation")

# Add 50 random walkers
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

**Congratulations!** You've created your first agent-based simulation! 🎉

## 🚗 Your First JamFree Traffic Simulation (3 minutes)

### Build JamFree

```bash
cd cpp/jamfree
./build.sh
```

### Run Web Interface

```bash
./run_web_ui.sh
```

Open http://localhost:5001

### Use the Interface

1. **Load a Map**:
   - Click "Download Area"
   - Pan/zoom the map to your area of interest
   - Click "Download Area" again
   - Wait for roads to load (blue lines)

2. **Configure Simulation**:
   - Number of vehicles: 200
   - Desired speed: 120 km/h
   - ✅ Enable "Adaptive Hybrid Simulation"

3. **Start**:
   - Click "▶️ Start"
   - Watch vehicles (red dots) move on roads
   - Observe performance metrics

**What you're seeing**:
- Real road network from OpenStreetMap
- Vehicles following IDM car-following model
- Automatic micro/macro mode switching
- Real-time performance stats

**Congratulations!** You've run a realistic traffic simulation! 🚗

## 🎓 Understanding the Basics

### SIMILAR2Logo Concepts

**Turtles (Agents)**:
- Autonomous entities that make decisions
- Have position, heading, speed
- Perceive their environment
- Emit influences (actions)

**Influences**:
- Actions that turtles want to perform
- Examples: move, turn, emit pheromone
- Resolved by the simulation engine
- Prevents conflicts

**Environment**:
- The world where turtles live
- Can be toroidal (wrap-around) or bounded
- Contains pheromones, obstacles
- Provides utility methods

**Simulation**:
- Orchestrates everything
- Runs perception → decision → action loop
- Can run headless or with web UI

### JamFree Concepts

**Microscopic Simulation**:
- Individual vehicle tracking
- IDM car-following model
- MOBIL lane-changing
- Accurate but slower

**Macroscopic Simulation**:
- Continuum flow model (LWR, CTM)
- Treats traffic as fluid
- Fast but less detailed

**Hybrid Simulation**:
- Automatically switches between micro/macro
- Best of both worlds
- Short roads/intersections: microscopic
- Long highways: macroscopic

**Routing**:
- A* pathfinding on road network
- Turn-by-turn navigation
- Lane selection for turns

## 📚 Next Steps

### Learn by Example

**Beginner** (Start here):
1. `simple_random_walk.py` - Basic movement
2. `circle.py` - Formation patterns
3. `boids_dsl.py` - Flocking behavior

**Intermediate**:
4. `ant_foraging_dsl.py` - Pheromones and stigmergy
5. `virus_spread.py` - State machines and epidemiology
6. `segregation_model.py` - Social dynamics

**Advanced**:
7. `predator_prey.py` - Population dynamics
8. `transport.py` - Path planning
9. Create your own!

### Explore Documentation

- **[README.md](README.md)** - Project overview
- **[QUICKSTART.md](QUICKSTART.md)** - Detailed quick start
- **[RUNNING_EXAMPLES.md](RUNNING_EXAMPLES.md)** - All examples explained
- **[QUICK_REFERENCE.md](QUICK_REFERENCE.md)** - Command cheat sheet

### JamFree Deep Dive

- **[cpp/jamfree/README.md](cpp/jamfree/README.md)** - JamFree overview
- **[cpp/jamfree/QUICK_START.md](cpp/jamfree/QUICK_START.md)** - Detailed guide
- **[cpp/jamfree/WEB_UI_SUMMARY.md](cpp/jamfree/WEB_UI_SUMMARY.md)** - Web interface guide

### Modify and Experiment

**SIMILAR2Logo**:
```python
# Try changing these in examples:
NUM_AGENTS = 200           # More agents
PERCEPTION_RADIUS = 20.0   # Larger perception
COHESION_WEIGHT = 0.02     # Stronger cohesion
```

**JamFree**:
- Try different numbers of vehicles
- Enable/disable optimizations
- Load different map areas
- Adjust speed limits

## 🎯 Common Use Cases

### Research

**Agent-Based Modeling**:
- Implement your model as Turtle subclass
- Run experiments with different parameters
- Collect data with `sim.run(steps=10000)`
- Analyze results

**Traffic Engineering**:
- Load real road networks
- Test traffic management strategies
- Validate against real data
- Optimize signal timing

### Education

**Teaching ABM**:
- Students modify existing examples
- Visual feedback with web UI
- Immediate results
- No complex setup

**Traffic Simulation Course**:
- Demonstrate IDM, MOBIL models
- Show micro vs. macro differences
- Real-world network integration
- Performance optimization concepts

### Prototyping

**Quick Validation**:
- Test ideas rapidly
- Visual debugging
- Easy parameter tuning
- Fast iteration

## 🐛 Troubleshooting

### "C++ engine not available"

**Fix**:
```bash
./build_cpp_engine.sh
```

### "ModuleNotFoundError: No module named 'similar2logo'"

**Fix**:
```bash
export PYTHONPATH=$PWD/python:$PYTHONPATH
# Or
pip install -e .
```

### Web UI not loading

**Fix**:
```bash
# Check if port is in use
lsof -i :8080  # SIMILAR2Logo
lsof -i :5001  # JamFree

# Use different port
sim.run_web(port=8081)
```

### Slow performance

**Fixes**:
1. Build C++ engine: `./build_cpp_engine.sh`
2. Reduce agent count
3. Limit perception radius
4. Enable JamFree optimizations

## 💡 Pro Tips

1. **Always build C++ engine** - 10-100x faster
2. **Use SIMILAR_VERBOSE=1** - See what's happening
3. **Start simple** - Modify working examples
4. **Read the code** - Examples are well-documented
5. **Web UI for debugging** - Visual feedback is invaluable
6. **Batch for data** - Use `sim.run()` for experiments

## 🎉 You're Ready!

You now have:
- ✅ Working installation
- ✅ First SIMILAR2Logo simulation
- ✅ First JamFree simulation
- ✅ Understanding of core concepts
- ✅ Resources for next steps

**What to do now**:
1. Try more examples from `examples/python/`
2. Modify parameters and see what happens
3. Read the documentation
4. Create your own simulation
5. Share your work!

## 🔗 Quick Links

- **Main README**: [README.md](README.md)
- **Quick Reference**: [QUICK_REFERENCE.md](QUICK_REFERENCE.md)
- **All Examples**: [examples/python/README.md](examples/python/README.md)
- **JamFree Guide**: [cpp/jamfree/QUICK_START.md](cpp/jamfree/QUICK_START.md)

## 🆘 Getting Help

- **Check examples**: 18+ working examples
- **Read docs**: Comprehensive documentation
- **Verbose mode**: `SIMILAR_VERBOSE=1`
- **GitHub Issues**: Report bugs or ask questions

---

**Happy Simulating!** 🚀

*Questions? Check [README.md](README.md) or [RUNNING_EXAMPLES.md](RUNNING_EXAMPLES.md)*
