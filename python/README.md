# SIMILAR2Logo Python Bindings

Python bindings for the SIMILAR2Logo multi-agent simulation framework, enabling easy development of Logo-style agent-based models with web-based visualization.

## Features

- 🐢 **Pythonic API** - Easy-to-use interface for defining agent behaviors
- 🌐 **Web Visualization** - Real-time browser-based visualization and control
- 🧪 **Pheromone System** - Built-in support for stigmergy and chemical communication
- 🔄 **Toroidal Topology** - Wrap-around environments for seamless movement
- ⚡ **High Performance** - C++ backend for efficient simulation
- 📊 **Visualization** - Built-in matplotlib plotting support

## Installation

### Prerequisites

- Python 3.8 or higher
- CMake 3.10 or higher
- C++20 compatible compiler
- pybind11

### Build from Source

```bash
# Clone the repository
git clone https://github.com/gildasmorvan/similar.git
cd similar

# Build C++ libraries
cd cpp
mkdir build && cd build
cmake ..
make

# Build Python bindings
cd ../similar2logo/python
mkdir build && cd build
cmake ..
make

# Install Python package
cd ../../../../
pip install -e .
```

### Install Dependencies

```bash
# Core dependencies
pip install numpy fastapi uvicorn websockets pydantic

# Optional: for matplotlib visualization
pip install matplotlib
```

## Quick Start

### Basic Simulation

```python
from similar2logo import LogoSimulation, Turtle, Environment
import random

class RandomWalker(Turtle):
    def decide(self, perception):
        if random.random() < 0.1:
            self.turn(random.uniform(-0.5, 0.5))
        self.move_forward(1.0)

# Create environment
env = Environment(100, 100, toroidal=True)

# Create and run simulation
sim = LogoSimulation(env, num_turtles=50, turtle_class=RandomWalker)
sim.run(steps=1000)
sim.plot()
```

### Web-Based Visualization

```python
from similar2logo.web import WebSimulation

# Create simulation (as above)
sim = LogoSimulation(env, num_turtles=50, turtle_class=RandomWalker)

# Create web interface
web_sim = WebSimulation(sim)
web_sim.start_server(port=8080)

# Open browser to http://localhost:8080
```

## Core Concepts

### Turtles (Agents)

Turtles are the agents in your simulation. Define custom behavior by subclassing `Turtle`:

```python
class MyTurtle(Turtle):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.energy = 100
    
    def decide(self, perception):
        # Access environment
        env = perception['environment']
        
        # Access pheromones
        food_level = perception['pheromones'].get('food', 0.0)
        
        # Make decisions
        if food_level > 10:
            self.move_forward(2.0)
        else:
            self.random_walk()
```

### Environment

The environment manages the grid, pheromones, and topology:

```python
env = Environment(
    width=100,
    height=100,
    toroidal=True  # Wrap-around edges
)

# Add pheromones
env.add_pheromone(
    identifier='food',
    diffusion_coef=0.1,    # How fast it spreads
    evaporation_coef=0.01,  # How fast it decays
    default_value=0.0,
    min_value=0.1
)

# Set pheromone values
env.set_pheromone(50, 50, 'food', 100.0)
```

### Pheromones

Pheromones enable indirect communication through the environment:

```python
# Create pheromone
food = env.add_pheromone('food', diffusion=0.1, evaporation=0.01)

# Turtles can deposit and sense pheromones
class Forager(Turtle):
    def decide(self, perception):
        # Sense pheromone
        food_level = perception['pheromones']['food']
        
        # Deposit pheromone
        x, y = int(self.position.x), int(self.position.y)
        perception['environment'].set_pheromone(x, y, 'food', 50.0)
```

## API Reference

### Turtle Class

**Attributes:**
- `position` (Point2D) - Current position
- `heading` (float) - Current heading in radians
- `color` (str) - Visual color
- `pen_down` (bool) - Whether turtle draws when moving
- `speed` (float) - Movement speed

**Methods:**
- `decide(perception)` - Override this to define behavior
- `move_forward(distance)` - Move forward
- `turn(angle)` - Turn by angle (radians)
- `turn_towards(heading)` - Turn towards target heading
- `set_heading(heading)` - Set absolute heading
- `random_walk(turn_amount)` - Perform random walk

### Environment Class

**Methods:**
- `add_pheromone(identifier, ...)` - Add pheromone type
- `set_pheromone(x, y, id, value)` - Set pheromone value
- `get_pheromone_value(x, y, id)` - Get pheromone value
- `get_distance(pos1, pos2)` - Calculate distance (toroidal-aware)
- `get_direction(from, to)` - Calculate direction (toroidal-aware)
- `random_position()` - Generate random position
- `random_heading()` - Generate random heading

### LogoSimulation Class

**Methods:**
- `add_turtles(count, turtle_class, **kwargs)` - Add turtles
- `step()` - Execute one simulation step
- `run(steps, callback)` - Run for N steps
- `get_state()` - Get current state as dict
- `reset()` - Reset simulation
- `plot()` - Plot using matplotlib

### WebSimulation Class

**Methods:**
- `start()` - Start simulation
- `pause()` - Pause simulation
- `resume()` - Resume simulation
- `stop()` - Stop simulation
- `start_server(host, port)` - Start web server

## Examples

### Flocking Behavior

```python
class Boid(Turtle):
    def decide(self, perception):
        # Separation: avoid crowding
        # Alignment: steer towards average heading
        # Cohesion: steer towards average position
        # (Simplified implementation)
        
        if random.random() < 0.1:
            self.turn(random.uniform(-0.2, 0.2))
        self.move_forward(1.0)
```

### Ant Colony

```python
class Ant(Turtle):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.has_food = False
    
    def decide(self, perception):
        if self.has_food:
            # Return to nest, deposit pheromone
            self.deposit_pheromone('home')
            self.move_towards_nest()
        else:
            # Search for food, follow food pheromone
            self.follow_pheromone('food')
```

### Segregation Model (Schelling)

```python
class Resident(Turtle):
    def __init__(self, *args, group='A', **kwargs):
        super().__init__(*args, **kwargs)
        self.group = group
        self.similarity_threshold = 0.3
    
    def decide(self, perception):
        # Count similar neighbors
        # Move if too few similar neighbors
        pass
```

## Performance Tips

1. **Batch Operations**: Use NumPy arrays for bulk data transfer
2. **Reduce Callback Frequency**: Only update visualization every N steps
3. **Optimize Perception**: Limit perception radius to reduce computation
4. **Use C++ for Heavy Computation**: Implement complex logic in C++

## Troubleshooting

### Import Error: `_core` module not found

The C++ bindings weren't built. Follow the build instructions above.

### Web Interface Not Loading

Check that port 8080 is not in use:
```bash
lsof -i :8080
```

### Slow Performance

- Reduce number of agents
- Decrease update rate: `WebSimulation(sim, update_rate=10)`
- Disable pheromone diffusion for testing

## Contributing

Contributions are welcome! Please see CONTRIBUTING.md for guidelines.

## License

MIT License - see LICENSE file for details.

## Citation

If you use SIMILAR2Logo in your research, please cite:

```bibtex
@software{similar2logo,
  title={SIMILAR2Logo: A Multi-Agent Simulation Framework},
  author={Morvan, Gildas and others},
  year={2024},
  url={https://github.com/gildasmorvan/similar}
}
```

## Support

- 📖 Documentation: https://similar.readthedocs.io
- 🐛 Issues: https://github.com/gildasmorvan/similar/issues
- 💬 Discussions: https://github.com/gildasmorvan/similar/discussions
