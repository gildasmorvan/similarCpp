# Python DSL Implementation Plan for Similar2Logo

**Date**: 2025-11-20  
**Objective**: Create a simplified Python DSL for Similar2Logo that leverages C++ optimizations

---

## Analysis of Jython Examples

### Key Patterns Found

1. **Clean Inheritance Structure**
   - `LogoSimulationParameters` for configuration
   - `AbstractAgtDecisionModel` for agent behavior
   - `AbstractLogoSimulationModel` for simulation setup

2. **Factory Pattern**
   - `TurtleFactory.generate()` for creating agents

3. **Minimal Boilerplate**
   - One-liner web runner: `Similar2LogoWebRunner()`
   - Simple initialization: `runner.initializeRunner(model)`

4. **Declarative Configuration**
   - Parameters as simple class attributes
   - No complex setup code

---

## Proposed Python DSL Design

### Goals

1. ✅ **Simpler than Jython** - Remove Java-isms
2. ✅ **Pythonic** - Use decorators, context managers, list comprehensions
3. ✅ **Auto-optimize** - Automatically use C++ when available
4. ✅ **Web-ready** - Built-in web interface support
5. ✅ **Beginner-friendly** - Clear, minimal API

### Design Principles

1. **Convention over Configuration**
   - Smart defaults for everything
   - Override only what you need

2. **Decorator-based Definitions**
   ```python
   @agent_behavior
   def boid_decide(self, perception):
       # Simple, clean decision logic
       pass
   ```

3. **Fluent API**
   ```python
   sim = Simulation()        .grid(100, 100)        .agents(Boid, count=50)        .web(port=8080)        .run()
   ```

4. **Automatic C++ Optimization**
   - No code changes needed
   - Transparent performance boost

---

## Implementation Phases

### Phase 1: Core DSL (Week 1) ✅ IN PROGRESS

**Deliverables**:
- [ ] `similar2logo.dsl` module
- [ ] `@agent` decorator
- [ ] `@behavior` decorator  
- [ ] `Simulation` class with fluent API
- [ ] Auto-web integration

**Examples to Convert**:
- [x] Boids (already done)
- [x] Segregation (already done)
- [x] Ant Foraging (already done)
- [ ] **Turmite** (new)
- [ ] **Multi-Turmite** (new)
- [ ] **Passive Turtle** (new)

### Phase 2: Advanced DSL (Week 2)

**Deliverables**:
- [ ] Parameter validation
- [ ] Agent templates library
- [ ] Pre-built behaviors (flocking, stigmergy, etc.)
- [ ] Visual parameter tuning

**Examples to Convert**:
- [ ] **Transport Exploration**
- [ ] **Predator-Prey** (using DSL)
- [ ] **Fire Spread**
- [ ] **Traffic Simulation**

### Phase 3: DSL Extensions (Week 3)

**Deliverables**:
- [ ] Pheromone DSL
- [ ] Network/graph support
- [ ] 3D extensions
- [ ] Multi-level simulations

---

## DSL API Design

### 1. Simple Simulation (Beginner)

```python
from similar2logo.dsl import Simulation, Turtle

# Define agent behavior with decorator
class Boid(Turtle):
    @behavior
    def decide(self, perception):
        neighbors = perception.nearby_turtles(radius=5)
        if neighbors:
            # Simple flocking rules
            self.align_with(neighbors)
            self.separate_from(neighbors, min_distance=1)
            self.cohere_with(neighbors)

# Create and run simulation
sim = Simulation("Boids Flocking")
sim.add_agents(Boid, count=100)
sim.run_web()  # Auto-starts web interface
```

### 2. Intermediate Simulation (Regular User)

```python
from similar2logo.dsl import Simulation, Turtle, Parameters

@dataclass
class BoidsParams(Parameters):
    repulsion_distance: float = 1.0
    orientation_distance: float = 2.0
    attraction_distance: float = 4.0
    num_agents: int = 200

class Boid(Turtle):
    def __init__(self, params):
        super().__init__()
        self.params = params
    
    @behavior(perception_radius=4.0)
    def flock(self, perception):
        for neighbor in perception.turtles:
            if neighbor.distance < self.params.repulsion_distance:
                self.turn_away_from(neighbor)
            elif neighbor.distance < self.params.orientation_distance:
                self.align_with(neighbor)
            else:
                self.move_towards(neighbor)

# Create simulation with parameters
params = BoidsParams()
sim = Simulation(params)   .grid(100, 100, toroidal=True)   .agents(Boid, count=params.num_agents)   .visualize(web=True, port=8080)   .run(max_steps=1000)
```

### 3. Advanced Simulation (Power User)

```python
from similar2logo.dsl import (
    Simulation, Turtle, Environment,
    Pheromone, Marker, influence
)

# Custom environment
class ForagingEnvironment(Environment):
    def setup(self):
        self.add_pheromone('food', diffusion=0.1, evaporation=0.01)
        self.add_pheromone('home', diffusion=0.1, evaporation=0.01)
        self.place_food_sources([(20, 20), (80, 80)])

# Ant agent with state
class Ant(Turtle):
    def __init__(self):
        super().__init__()
        self.carrying_food = False
        self.nest = (50, 50)
    
    @behavior
    def forage(self, perception):
        if self.carrying_food:
            return self.return_to_nest(perception)
        else:
            return self.search_for_food(perception)
    
    def search_for_food(self, perception):
        # Follow pheromone gradient
        food_gradient = perception.pheromone_gradient('food')
        if food_gradient:
            return self.follow_gradient(food_gradient)
        else:
            return self.random_walk()
    
    @influence.emit_pheromone('food', amount=10)
    def return_to_nest(self, perception):
        if self.at_position(self.nest):
            self.carrying_food = False
        return self.move_towards(self.nest)

# Run simulation
sim = Simulation()   .environment(ForagingEnvironment())   .add_agents(Ant, count=50)   .run_web(port=8080)
```

---

## DSL Components

### 1. Core Classes

```python
class Simulation:
    """Main simulation orchestrator"""
    def grid(width, height, toroidal=True)
    def add_agents(agent_class, count, **kwargs)
    def environment(env_class)
    def run(max_steps=None, max_time=None)
    def run_web(port=8080, update_rate=30)
    def add_probe(probe)
    def set_speed(multiplier)
```

### 2. Agent Base Classes

```python
class Turtle:
    """Base agent class"""
    # Movement
    def move_forward(distance)
    def turn(angle)
    def turn_towards(target)
    def move_to(x, y)
    
    # Perception helpers
    def nearby_turtles(radius)
    def turtles_in_cone(radius, angle)
    def sense_pheromone(pheromone_id)
    
    # Influence helpers
    def emit_pheromone(pheromone_id, amount)
    def place_mark(content)
    
    # Flocking helpers (common behaviors)
    def align_with(neighbors)
    def separate_from(neighbors, min_distance)
    def cohere_with(neighbors)
```

### 3. Decorators

```python
@behavior(perception_radius=5.0, perception_angle=math.pi)
def my_behavior(self, perception):
    pass

@influence.change_direction
def turn_left(self):
    return -0.1

@influence.emit_pheromone('trail', amount=1.0)
def leave_trail(self):
    pass

@agent(category='predator', color='red', speed=2.0)
class Predator(Turtle):
    pass
```

### 4. Context Managers

```python
with Simulation("My Sim") as sim:
    sim.grid(100, 100)
    
    with sim.agent_group("prey", count=50) as prey:
        prey.behavior(prey_behavior)
        prey.color("blue")
    
    with sim.agent_group("predators", count=5) as pred:
        pred.behavior(predator_behavior)
        pred.color("red")
    
    sim.run_web()
```

---

## Examples to Implement

### Priority 1 (Essential)
1. ✅ **Boids Flocking** - Classic emergent behavior
2. ✅ **Segregation** - Schelling's model
3. ✅ **Ant Foraging** - Stigmergy demonstration
4. ⏳ **Turmite** - Simple cellular automaton
5. ⏳ **Passive Turtle** - Random walk

### Priority 2 (Common Use Cases)
6. ⏳ **Predator-Prey** - Population dynamics
7. ⏳ **Fire Spread** - Environmental simulation
8. ⏳ **Traffic** - Urban modeling
9. ⏳ **Epidemics** - SIR/SEIR models
10. ⏳ **Game of Life** - Classic CA

### Priority 3 (Advanced)
11. ⏳ **Transport/Exploration** - Complex coordination
12. ⏳ **Multi-Turmite** - Multiple species
13. ⏳ **Slime Mold** - Network formation
14. ⏳ **Particle Systems** - Physics simulation
15. ⏳ **Social Networks** - Graph-based ABM

---

## File Structure

```
python/similar2logo/
├── dsl/
│   ├── __init__.py
│   ├── simulation.py      # Main Simulation class
│   ├── agents.py          # Turtle, Agent bases
│   ├── decorators.py      # @behavior, @influence, etc.
│   ├── environment.py     # Environment helpers
│   ├── behaviors.py       # Pre-built behaviors
│   └── fluent.py          # Fluent API implementation
│
├── templates/
│   ├── flocking.py        # Reusable flocking template
│   ├── stigmergy.py       # Pheromone-based template
│   ├── cellular.py        # CA template
│   └── network.py         # Network ABM template
│
└── examples_dsl/          # NEW: DSL-based examples
    ├── boids_dsl.py
    ├── segregation_dsl.py
    ├── ant_foraging_dsl.py
    ├── turmite_dsl.py
    ├── passive_dsl.py
    ├── predator_prey_dsl.py
    └── ...
```

---

## Migration Path

### From Jython to Python DSL

**Before (Jython)**:
```python
class BoidDecisionModel(AbstractAgtDecisionModel):
    def __init__(self, parameters):
        self.parameters = parameters
        super().__init__(LogoSimulationLevelList.LOGO)
    
    def decide(self, timeLowerBound, timeUpperBound, 
               globalState, publicLocalState, privateLocalState,
               perceivedData, producedInfluences):
        # 50 lines of Java-style code
        producedInfluences.add(ChangeDirection(...))
```

**After (Python DSL)**:
```python
class Boid(Turtle):
    @behavior
    def flock(self, perception):
        # 10 lines of Pythonic code
        self.align_with(perception.nearby_turtles())
```

**Reduction**: 80% less code!

---

## Next Steps

### Immediate (This Session)
1. Create DSL module structure
2. Implement `Simulation` class
3. Implement `Turtle` base class
4. Create `@behavior` decorator
5. Port Turmite example as proof-of-concept

### Short Term (Next Session)
1. Complete all Priority 1 examples
2. Add pre-built behavior library
3. Create comprehensive documentation
4. Performance benchmarks

### Long Term (Future)
1. Visual model builder (GUI)
2. Parameter optimization tools
3. Multi-paradigm support (discrete events, ODE)
4. Cloud deployment helpers

---

## Success Metrics

1. **Code Reduction**: 70-80% less code vs Jython
2. **Learning Curve**: Beginners create first sim in < 30 min
3. **Performance**: Auto-optimization gives 5-10x speedup
4. **Adoption**: 90% of new examples use DSL
5. **Expressiveness**: All Jython examples convertible

---

**Status**: PLANNING COMPLETE  
**Ready**: Implementation Phase 1
