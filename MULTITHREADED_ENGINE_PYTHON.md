# MultiThreadedSimulationEngine Python Bindings

## Overview

The `MultiThreadedSimulationEngine` is now exposed to Python, allowing you to leverage multi-threaded C++ performance from Python code. This document explains how to use it.

## What's Been Exposed

### Core Classes

1. **`ISimulationEngine`** - Base simulation engine interface
2. **`ISimulationModel`** - Base simulation model interface (with Python trampoline support)
3. **`MultiThreadedSimulationEngine`** - High-performance multi-threaded engine
4. **`EnvironmentInitializationData`** - Container for environment initialization
5. **`AgentInitializationData`** - Container for agent initialization

### Import Path

```python
from similar2logo._core.engine import (
    ISimulationModel,
    MultiThreadedSimulationEngine,
    EnvironmentInitializationData,
    AgentInitializationData
)
from similar2logo._core.microkernel import SimulationTimeStamp, LevelIdentifier
```

## Basic Usage

### 1. Create a Custom Model

To use the engine, you need to create a model that inherits from `ISimulationModel`:

```python
class MyModel(ISimulationModel):
    def __init__(self):
        super().__init__()
    
    def get_initial_time(self):
        """Return the initial simulation time."""
        return SimulationTimeStamp(0)
    
    def is_final_time_or_after(self, current_time, engine):
        """Check if simulation should stop."""
        return current_time.get_identifier() >= 1000
    
    def generate_levels(self, initial_time):
        """Generate simulation levels."""
        # Return list of ILevel objects
        return []
    
    def generate_environment(self, initial_time, levels):
        """Generate the environment."""
        # Return EnvironmentInitializationData
        pass
    
    def generate_agents(self, initial_time, levels):
        """Generate agents."""
        # Return AgentInitializationData
        return AgentInitializationData()
```

### 2. Create and Use the Engine

```python
# Create your model
model = MyModel()

# Create the engine with desired number of threads
# Use 0 for automatic (hardware concurrency)
engine = MultiThreadedSimulationEngine(nb_threads=4)

# Run the simulation
engine.run_new_simulation(model)
```

## Performance Benefits

The `MultiThreadedSimulationEngine` provides significant performance improvements over pure Python:

1. **Parallel Processing**: Perception and decision phases run in parallel across agents
2. **C++ Speed**: Core simulation loop runs in optimized C++ code
3. **Optimized Math**: Uses FastMath library with lookup tables for trigonometric functions
4. **Fast PRNG**: Uses Xoshiro256++ instead of Python's random module

## Comparison: Python vs C++ Engine

### Pure Python (LogoSimulation)
- **Pros**: Easy to use, flexible, good for prototyping
- **Cons**: Slower, single-threaded
- **Use case**: Rapid development, simple models

### C++ Engine (MultiThreadedSimulationEngine)
- **Pros**: Much faster, multi-threaded, production-ready
- **Cons**: Requires implementing model in Python with C++ callbacks (GIL limitations)
- **Use case**: Performance-critical simulations, large-scale models

### Best Approach
For maximum performance, implement your entire model in C++ (like `web_predator_prey.cpp`). The Python bindings are useful for:
- Testing the engine from Python
- Hybrid approaches where some logic is in Python
- Gradual migration from Python to C++

## Example

See `examples/python/multithreaded_engine_example.py` for a complete working example.

## Limitations

### GIL (Global Interpreter Lock)
When using Python callbacks from the C++ engine, the GIL can limit parallelism. For true multi-threaded performance, implement agent logic in C++.

### Interface Complexity
The `ISimulationModel` interface requires implementing several methods. For simple use cases, the pure Python `LogoSimulation` may be more convenient.

## Next Steps

1. **For Learning**: Start with the Python `LogoSimulation` class
2. **For Performance**: Implement models in C++ using the examples as templates
3. **For Hybrid**: Use the Python bindings to gradually migrate performance-critical parts to C++

## Related Files

- **C++ Example**: `cpp/extendedkernel/examples/web_predator_prey.cpp`
- **Python Example**: `examples/python/multithreaded_engine_example.py`
- **Bindings Source**: `cpp/similar2logo/python/bindings.cpp`
- **Performance Docs**: `OPTIMIZATION_SUMMARY.md`
