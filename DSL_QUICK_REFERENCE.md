# Similar2Logo Python DSL - Quick Reference

**A Pythonic, simplified API for agent-based modeling**

---

## Installation

```bash
# Already installed if you have Similar2Logo
cd /path/to/similar
PYTHONPATH=python python3 your_simulation.py
```

---

## Quickstart (30 seconds)

```python
from similar2logo.dsl import Simulation, SimpleTurtle

class MyAgent(SimpleTurtle):
    def decide(self, perception):
        neighbors = self.nearby_turtles(radius=5)
        self.align_with(neighbors)
        return []

# Create and run
(Simulation("My First Sim")
 .grid(100, 100)
 .agents(MyAgent, count=50)
 .run_web())
```

Open http://localhost:8080 and watch!

---

## Core Concepts

### 1. Simulation

The main orchestrator using fluent API:

```python
sim = Simulation("My Simulation")

# Configure grid
sim.grid(width=100, height=100, toroidal=True)

# Add agents
sim.agents(MyAgent, count=50, custom_param=value)

# Add monitoring
sim.probe(my_probe)

# Run modes
sim.run(max_steps=1000)           # Console
sim.run_web(port=8080)            # Web interface
```

### 2. Agents

Inherit from `SimpleTurtle` for helpers:

```python
class MyAgent(SimpleTurtle):
    def __init__(self, **kwargs):
        super().__init__()
        # Your initialization
    
    def decide(self, perception):
        # Return list of influences
        return []
```

### 3. Behaviors

Use built-in helpers:

```python
class Boid(SimpleTurtle):
    def decide(self, perception):
        neighbors = self.nearby_turtles(radius=5)
        
        influences = []
        influences.extend(self.align_with(neighbors))
        influences.extend(self.separate_from(neighbors, min_distance=1))
        influences.extend(self.cohere_with(neighbors))
        
        return influences
```

---

## API Reference

### Simulation Class

```python
Simulation(name="My Sim")
    .grid(width, height, toroidal=True)
    .agents(AgentClass, count, **kwargs)
    .environment(EnvironmentClass)
    .probe(probe_instance)
    .speed(multiplier)
    .run(max_steps=None, max_time=None)
    .run_web(port=8080, update_rate=30)
```

### SimpleTurtle Methods

**Perception**:
```python
nearby_turtles(radius=5.0)        # All turtles within radius
turtles_in_cone(radius, angle)    # Turtles in cone of vision
```

**Behaviors**:
```python
align_with(neighbors, weight=1.0)         # Match average heading
separate_from(neighbors, min_dist, weight)  # Avoid crowding
cohere_with(neighbors, weight=1.0)        # Move to center
```

**Influences** (inherited from Turtle):
```python
influence_turn(angle)
influence_move_forward(distance)
influence_change_speed(delta)
influence_emit_pheromone(id, amount)
```

---

## Examples

### Random Walk

```python
import random
from similar2logo.dsl import Simulation, SimpleTurtle

class RandomWalker(SimpleTurtle):
    def decide(self, perception):
        return [
            self.influence_turn(random.uniform(-0.5, 0.5)),
            self.influence_move_forward(1.0)
        ]

(Simulation("Random Walk")
 .grid(100, 100)
 .agents(RandomWalker, count=20)
 .run_web())
```

### Flocking (Boids)

```python
from similar2logo.dsl import Simulation, SimpleTurtle

class Boid(SimpleTurtle):
    def decide(self, perception):
        neighbors = self.nearby_turtles(radius=5)
        if not neighbors:
            return []
        
        return (
            self.separate_from(neighbors, min_distance=1.0) +
            self.align_with(neighbors) +
            self.cohere_with(neighbors)
        )

(Simulation("Boids")
 .grid(100, 100)
 .agents(Boid, count=100)
 .run_web())
```

### Ant Foraging

```python
from similar2logo.dsl import Simulation, SimpleTurtle

class Ant(SimpleTurtle):
    def __init__(self):
        super().__init__()
        self.has_food = False
    
    def decide(self, perception):
        if self.has_food:
            # Return to nest, leaving pheromone
            return [
                self.influence_emit_pheromone('food', 10),
                self.influence_move_forward(1.0)
            ]
        else:
            # Search for food
            return [self.influence_move_forward(1.0)]

(Simulation("Ant Foraging")
 .grid(100, 100)
 .agents(Ant, count=50)
 .run_web())
```

---

## Advanced Features

### Custom Parameters

```python
from dataclasses import dataclass

@dataclass
class MyParams:
    perception_radius: float = 5.0
    speed: float = 1.0
    num_agents: int = 100

class MyAgent(SimpleTurtle):
    def __init__(self, params):
        super().__init__()
        self.params = params
    
    def decide(self, perception):
        neighbors = self.nearby_turtles(self.params.perception_radius)
        return []

params = MyParams()
(Simulation()
 .agents(MyAgent, count=params.num_agents, params=params)
 .run_web())
```

### Context Manager

```python
with Simulation("My Sim") as sim:
    sim.grid(100, 100)
    sim.agents(MyAgent, count=50)
    # Automatically runs on exit
```

### Multiple Agent Types

```python
(Simulation("Predator-Prey")
 .grid(100, 100)
 .agents(Prey, count=100)
 .agents(Predator, count=10)
 .run_web())
```

---

## Best Practices

### 1. Use Dataclasses for Parameters

```python
@dataclass
class Params:
    grid_size: int = 100
    num_agents: int = 50
```

### 2. Keep decide() Simple

```python
def decide(self, perception):
    # Break complex logic into methods
    if self.state == "searching":
        return self.search(perception)
    else:
        return self.return_home(perception)
```

### 3. Use Helper Methods

```python
# Good - uses helpers
neighbors = self.nearby_turtles(radius=5)
self.align_with(neighbors)

# Less good - manual calculation
for other in all_agents:
    if distance(self, other) < 5:
        # manual alignment code
```

### 4. Fluent API for Readability

```python
# Good - readable chain
(Simulation("My Sim")
 .grid(100, 100)
 .agents(MyAgent, count=50)
 .run_web())

# Less good - separate calls
sim = Simulation("My Sim")
sim.grid(100, 100)
sim.agents(MyAgent, count=50)
sim.run_web()
```

---

## Performance Tips

1. **Automatic C++ Optimization**: All DSL code uses C++ when available
2. **Perception Radius**: Smaller = faster
3. **Agent Count**: 100-500 is comfortable, 1000+ may slow down
4. **Update Rate**: Lower update_rate = smoother but slower

---

## Troubleshooting

### "Module not found"
```bash
# Make sure PYTHONPATH is set
export PYTHONPATH=/path/to/similar/python
```

### "No C++ optimization"
```bash
# Build C++ bindings
cd cpp/build
cmake -DBUILD_PYTHON_BINDINGS=ON ..
make -j8
cp python/similar2logo/_core.*.so ../../python/similar2logo/
```

### "Simulation runs too slow"
- Reduce agent count
- Reduce perception radius
- Lower update_rate
- Check that C++ bindings are active

---

## More Examples

See `examples/python/` directory:
- `boids_dsl.py` - Flocking behavior
- `turmite.py` - Cellular automaton
- `segregation_model.py` - Schelling's model
- `ant_foraging.py` - Stigmergy

---

## Documentation

- **Full Plan**: `PYTHON_DSL_PLAN.md`
- **Implementation**: `DSL_IMPLEMENTATION_SUMMARY.md`
- **C++ Optimizations**: `PYTHON_CPP_OPTIMIZATION.md`

---

## Getting Help

1. Check examples in `examples/python/`
2. Read the implementation docs
3. Look at Jython examples for comparison
4. File an issue on GitHub

---

**Happy Modeling!** 🐢🚀
