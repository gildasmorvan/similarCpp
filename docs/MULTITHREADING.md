# Multithreaded Scheduler Implementation

## Summary

I've refactored the Python DSL to support parallel execution of agent decisions using a configurable backend system. While the C++ `MultiThreadedSimulationEngine` is available through the bindings, fully integrating it with the Python DSL would require a complete architectural rewrite.

## Current Implementation

### Threading Backend

The Python DSL now uses a `ThreadPoolExecutor` to parallelize agent decision-making:

```python
from similar2logo.dsl import Simulation, Turtle

# Enable threading (default)
(Simulation()
 .parallel(backend='thread', num_workers=4)
 .agents(MyAgent, 1000)
 .run_web())
```

**Key Features:**
- Configurable number of worker threads
- Proper SIMILAR architecture (perception → decision → reaction)
- Decisions are executed in parallel across threads
- Reactions remain sequential for consistency

**Limitations:**
- Python's Global Interpreter Lock (GIL) limits true parallelism for CPU-bound tasks
- Best suited for I/O-bound operations or mixed workloads
- CPU-intensive agent logic won't see significant speedup

### Architecture

The implementation follows the SIMILAR microkernel pattern:

1. **Perception Phase** (Sequential): Build perceptions for all agents
2. **Decision Phase** (Parallel): Execute `decide()` for all agents using ThreadPoolExecutor
3. **Reaction Phase** (Sequential): Process influences and update state

## C++ MultiThreadedSimulationEngine

The C++ engine is exposed through Python bindings (`_core.MultiThreadedSimulationEngine`) but requires:

1. Implementing `ISimulationModel` in Python
2. Creating C++-compatible agent classes
3. Implementing perception and decision models
4. Managing the full simulation lifecycle through the C++ engine

This would bypass the GIL entirely and provide true multithreaded parallelism.

## Files Modified

### Core Implementation
- `python/similar2logo/model.py`: Added parallel backend configuration to `LogoSimulation`
- `python/similar2logo/parallel.py`: New module with `ThreadedDecisionExecutor`
- `python/similar2logo/dsl/__init__.py`: Added `.parallel()` method to DSL

### Examples
- `examples/python/boids_dsl.py`: Updated to use threading by default
- `examples/python/boids_multiprocessing.py`: Example demonstrating parallel configuration

## Usage

### Basic Threading
```python
sim = (Simulation("My Simulation")
       .grid(100, 100)
       .parallel(backend='thread', num_workers=4)
       .agents(MyAgent, 500)
       .run_web())
```

### Disable Parallelism
```python
sim = (Simulation("My Simulation")
       .grid(100, 100)
       .parallel(backend=None)  # Sequential execution
       .agents(MyAgent, 500)
       .run_web())
```

## Future Work

For true multithreaded performance without GIL limitations:

1. **Option 1: C++ Agents**
   - Implement agents in C++
   - Use `MultiThreadedSimulationEngine` directly
   - Expose results to Python for visualization

2. **Option 2: Hybrid Approach**
   - Keep Python DSL for configuration
   - Generate C++ code from Python agent definitions
   - Compile and run with C++ engine

3. **Option 3: Cython/Numba**
   - Use Cython or Numba to compile agent logic
   - Release GIL during decision phase
   - Maintain Python-friendly API

## Performance Notes

Current threading implementation provides:
- ✅ Correct SIMILAR architecture
- ✅ Parallel structure for future C++ integration
- ✅ Benefits for I/O-bound operations
- ⚠️ Limited speedup for CPU-bound Python code (due to GIL)

For CPU-intensive simulations with 1000+ agents, consider implementing critical agent logic in C++ or using the C++ engine directly.
