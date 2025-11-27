# SIMILAR2Logo Python DSL - Quick Start Guide

## 🚀 Get Started in 5 Minutes

### Installation

#### Quick Install (Recommended)

```bash
pip3 install similar2logo
```

#### From Source (Development)

```bash
# Clone the repository
git clone https://github.com/gildasmorvan/similar
cd similar

# Build C++ engine (optional but recommended for performance)
./build_cpp_engine.sh

# Install Python dependencies
pip install -r requirements.txt
```

### Your First Simulation

Create `my_first_sim.py`:

```python
from similar2logo.dsl import *
import random

class RandomWalker(Turtle):
    def decide(self, perception):
        # Random walk behavior
        influences = []
        if random.random() < 0.3:
            influences.append(self.influence_turn(random.uniform(-0.5, 0.5)))
        influences.append(self.influence_move_forward(1.0))
        return influences

# Create environment
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

## 📚 Core Concepts

### 1. Turtles (Agents)

Turtles are agents that make decisions:

```python
class MyTurtle(Turtle):
    def decide(self, perception):
        # Access perception data
        nearby = perception['nearby_turtles']
        pheromones = perception['pheromones']
        environment = perception['environment']
        
        # Return list of influences
        return [
            self.influence_move_forward(1.0),
            self.influence_turn(0.1)
        ]
```

### 2. Influences (Actions)

Turtles don't modify themselves directly - they emit influences:

```python
# Movement
self.influence_move_forward(distance)
self.influence_move_to(target_position)

# Rotation
self.influence_turn(angle)
self.influence_turn_towards(target_heading)

# Speed
self.influence_set_speed(speed)
self.influence_change_speed(delta_speed)

# Environment
self.influence_emit_pheromone(pheromone_id, amount)
self.influence_stop()
```

### 3. Environment

The world where turtles live:

```python
# Create environment
env = Environment(
    width=100,
    height=100,
    toroidal=True  # Wrap-around edges
)

# Add pheromones
env.add_pheromone(
    "food",
    diffusion_coef=0.1,
    evaporation_coef=0.05
)

# Utility methods
pos = env.random_position()
heading = env.random_heading()
distance = env.get_distance(pos1, pos2)
```

### 4. Simulation

Orchestrates everything:

```python
sim = Simulation(
    environment=env,
    title="My Simulation"
)

# Add agents
sim.add_agent(MyTurtle())

# Run headless
sim.run(steps=1000)

# Run with web UI
sim.run_web(port=8080, steps_per_frame=1)
```

## 🎯 Common Patterns

### Flocking Behavior

```python
class FlockingBoid(Turtle):
    def decide(self, perception):
        neighbors = perception['nearby_turtles']
        
        # Cohesion: move toward center
        center_x = sum(n['position'].x for n in neighbors) / len(neighbors)
        center_y = sum(n['position'].y for n in neighbors) / len(neighbors)
        
        # Alignment: match heading
        avg_heading = sum(n['heading'] for n in neighbors) / len(neighbors)
        
        # Separation: avoid crowding
        # ... (see boids.py for full implementation)
        
        return influences
```

### Pheromone Following

```python
class AntAgent(Turtle):
    def decide(self, perception):
        # Get pheromone concentration
        pheromone_here = perception['pheromones'].get('trail', 0)
        
        # Emit pheromone
        influences = [
            self.influence_emit_pheromone('trail', 10.0)
        ]
        
        # Follow gradient
        # ... (see ant_foraging.py)
        
        return influences
```

### State Machines

```python
class StatefulAgent(Turtle):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.state = "searching"
        self.energy = 100
    
    def decide(self, perception):
        if self.state == "searching":
            # Search behavior
            if self.found_food():
                self.state = "returning"
        
        elif self.state == "returning":
            # Return behavior
            if self.at_home():
                self.state = "searching"
        
        return influences
```

## 🔥 Performance Tips

### Use C++ Engine

```bash
# Build once
./build_cpp_engine.sh

# Verify it's working
SIMILAR_VERBOSE=1 python3 my_sim.py
# Should see: ✓ Using C++ reaction engine
```

### Optimize Perception

```python
# Use spatial indexing (automatic)
sim = Simulation(environment=env)  # Built-in spatial hash grid

# Limit perception radius
PERCEPTION_RADIUS = 10.0  # Smaller = faster
```

### Batch Processing

```python
# Run headless for data collection
sim.run(steps=10000)

# Access final state
for turtle in sim.turtles:
    print(turtle.position, turtle.heading)
```

## 📊 Examples by Difficulty

### Beginner
- `simple_random_walk.py` - Basic movement
- `passive_turtle.py` - Physics
- `circle.py` - Formation

### Intermediate
- `boids_dsl.py` - Flocking
- `ant_foraging_dsl.py` - Pheromones
- `virus_spread.py` - State machines

### Advanced
- `heatbugs.py` - Diffusion fields
- `transport.py` - Path planning
- `multiturmite.py` - Cellular automata

## 🐛 Troubleshooting

### "Using Python reaction fallback" Warning

**Cause**: C++ engine not built or not compatible

**Fix**:
```bash
./build_cpp_engine.sh
```

### Slow Performance

**Check**:
1. Is C++ engine active? (`SIMILAR_VERBOSE=1`)
2. Too many agents? (Try fewer or optimize perception)
3. Pheromone diffusion? (Reduce grid size or coefficients)

### Import Errors

**Fix**:
```bash
# Ensure you're in the right directory
cd similar
export PYTHONPATH=$PWD/python:$PYTHONPATH
python3 examples/python/my_sim.py
```

## 📖 Next Steps

1. **Run examples**: Try all examples in `examples/python/`
2. **Read code**: Study example implementations
3. **Modify**: Change parameters and behavior
4. **Create**: Build your own simulation
5. **Share**: Contribute back to the project!

## 🔗 Resources

- **Examples**: `examples/python/README.md`
- **API Reference**: `python/similar2logo/dsl/__init__.py`
- **Java Comparison**: `similar2logo-examples/`
- **Status**: `IMPLEMENTATION_STATUS.md`

## 💡 Tips

- Start simple, add complexity gradually
- Use `print()` for debugging
- Visualize with web interface
- Profile with `SIMILAR_VERBOSE=1`
- Read existing examples for patterns

## 🎉 You're Ready!

You now have everything you need to create agent-based simulations with SIMILAR2Logo Python DSL.

**Happy Simulating!** 🚀
