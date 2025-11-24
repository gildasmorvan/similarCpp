# SIMILAR2Logo Python Examples

Complete collection of SIMILAR2Logo simulations implemented in Python using the DSL.

## 🎯 Overview

All examples use the **Python DSL** for clean, intuitive code and leverage the **C++ engine** for optimal performance when available.

## 📚 Available Examples

### Basic Examples

#### 1. **Simple Random Walk** (`simple_random_walk.py`)
- Single turtle performing random walk
- Demonstrates basic movement and influence system
- **Concepts**: Basic turtle, random movement

#### 2. **Random Walk 1D** (`randomwalk_1d.py`)
- Turtle constrained to 1D movement
- Shows dimensional constraints
- **Concepts**: Constrained movement, boundary handling

#### 3. **Passive Turtle** (`passive_turtle.py`)
- Turtle with physics-based movement
- No decision making, pure dynamics
- **Concepts**: Physics simulation, passive agents

### Flocking & Collective Behavior

#### 4. **Boids** (`boids.py`, `boids_dsl.py`, `boids_flocking.py`)
- Classic flocking behavior (Reynolds 1987)
- Cohesion, alignment, and separation
- **Concepts**: Emergent behavior, local rules → global patterns

#### 5. **Boids with Obstacles** (`boids_obstacles.py`) ✨ NEW
- Flocking with obstacle avoidance
- Dynamic navigation around barriers
- **Concepts**: Obstacle avoidance, path planning

#### 6. **Circle** (`circle.py`)
- Agents forming circular patterns
- Demonstrates formation control
- **Concepts**: Pattern formation, coordination

### Biological & Ecological Models

#### 7. **Ant Foraging** (`ant_foraging.py`, `ant_foraging_dsl.py`)
- Ants finding food using pheromones
- Classic swarm intelligence
- **Concepts**: Stigmergy, pheromone trails, foraging

#### 8. **Predator-Prey** (`predator_prey.py`)
- Lotka-Volterra dynamics
- Population cycles
- **Concepts**: Ecological modeling, population dynamics

#### 9. **Virus Spread** (`virus_spread.py`) ✨ NEW
- Epidemiology simulation (SIR model)
- Disease transmission and recovery
- **Concepts**: Epidemiology, contact networks, state transitions

#### 10. **Heatbugs** (`heatbugs.py`) ✨ NEW
- Bugs seeking optimal temperature
- Self-organizing thermal regulation
- **Concepts**: Thermotaxis, self-organization, diffusion

### Environmental & Physical Simulations

#### 11. **Forest Fire** (`forest_fire.py`)
- Fire spreading through forest
- Cellular automaton dynamics
- **Concepts**: Cellular automata, percolation, spread dynamics

#### 12. **Pheromone Following** (`pheromone_following.py`)
- Agents following chemical trails
- Demonstrates chemotaxis
- **Concepts**: Gradient following, chemical signals

#### 13. **Slime Mold** (`slime_mold.py`)
- Physarum polycephalum simulation
- Network formation
- **Concepts**: Bio-inspired algorithms, network optimization

### Artificial Life & Patterns

#### 14. **Turmite** (`turmite.py`)
- Single Langton's Ant
- Emergent highway formation
- **Concepts**: Cellular automaton, emergence, complexity from simplicity

#### 15. **Multi-Turmite** (`multiturmite.py`) ✨ NEW
- Multiple Langton's Ants interacting
- Complex pattern interference
- **Concepts**: Multi-agent cellular automata, pattern interaction

#### 16. **Firework** (`firework.py`) ✨ NEW
- Particle system simulation
- Explosions with gravity
- **Concepts**: Particle physics, visual effects, lifetime management

### Social & Urban Models

#### 17. **Segregation** (`segregation_model.py`)
- Schelling's segregation model
- Demonstrates tipping points
- **Concepts**: Social dynamics, emergence, tipping points

#### 18. **Transport** (`transport.py`) ✨ NEW
- Vehicle traffic on road networks
- Collision avoidance and traffic flow
- **Concepts**: Traffic simulation, path following, collision avoidance

## 🚀 Running Examples

### Basic Usage

```bash
# Run any example
python3 examples/python/boids_dsl.py

# With verbose C++ info
SIMILAR_VERBOSE=1 python3 examples/python/virus_spread.py
```

### Web Interface

Most examples include a web interface:

```bash
python3 examples/python/heatbugs.py
# Then open http://localhost:8080
```

### Batch Running

```bash
# Run all examples (for testing)
python3 examples/python/run_examples.py
```

## 🎨 Example Categories

| Category | Examples | Key Concepts |
|----------|----------|--------------|
| **Flocking** | Boids, Boids+Obstacles, Circle | Collective behavior, emergence |
| **Foraging** | Ants, Pheromone Following, Slime Mold | Stigmergy, optimization |
| **Ecology** | Predator-Prey, Virus, Forest Fire | Population dynamics, spread |
| **Physics** | Passive Turtle, Firework | Dynamics, particle systems |
| **Patterns** | Turmite, Multi-Turmite, Segregation | Emergence, cellular automata |
| **Urban** | Transport | Traffic, networks |
| **Thermodynamics** | Heatbugs | Diffusion, self-organization |

## 📊 Performance

All examples automatically use the **C++ reaction engine** when available for 10-100x speedup:

```
✓ Using C++ reaction engine for optimal performance
```

If you see a warning:
```
⚠️ Using Python reaction model (C++ not available)
```

Build the C++ engine:
```bash
./build_cpp_engine.sh
```

## 🔧 Customization

Each example is designed to be easily modified:

```python
# Modify parameters
NUM_AGENTS = 500  # More agents
PERCEPTION_RADIUS = 20.0  # Larger perception

# Change behavior
class MyCustomTurtle(Turtle):
    def decide(self, perception):
        # Your custom logic here
        return influences
```

## 📖 Learning Path

**Beginner:**
1. Simple Random Walk → Basic concepts
2. Boids → Flocking behavior
3. Ant Foraging → Pheromones

**Intermediate:**
4. Virus Spread → State machines
5. Heatbugs → Diffusion fields
6. Segregation → Social dynamics

**Advanced:**
7. Multi-Turmite → Complex emergence
8. Transport → Path planning
9. Custom simulations → Build your own!

## 🎓 Educational Use

These examples are perfect for:
- **Agent-Based Modeling** courses
- **Complex Systems** demonstrations
- **Artificial Life** research
- **Swarm Intelligence** studies
- **Computational Biology** teaching

## 🤝 Contributing

To add a new example:

1. Create `my_example.py` in `examples/python/`
2. Use the DSL pattern (see existing examples)
3. Add documentation and parameters
4. Test with both Python and C++ engines
5. Update this README

## 📝 License

All examples follow the SIMILAR2Logo license (CeCILL-B).

## ✨ What's New

**Latest additions:**
- ✅ Virus Spread - Epidemiology simulation
- ✅ Heatbugs - Temperature-seeking agents
- ✅ Firework - Particle system with physics
- ✅ Boids with Obstacles - Flocking + avoidance
- ✅ Multi-Turmite - Multiple Langton's ants
- ✅ Transport - Traffic simulation

**All Java examples now implemented in Python DSL!** 🎉
