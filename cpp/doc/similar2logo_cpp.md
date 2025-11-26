## Similar2Logo C++ Multithreaded Engine Overview

Similar2Logo combines the SIMILAR architecture with a Logo-style grid world. In this project, the heavy‑weight simulation engine is implemented in **C++** and exposed to Python, so Logo simulations written in Python can run with multi‑threaded C++ performance.

### Components and Location

The C++ engine and bindings are mainly located in:

- `cpp/similar2logo/` – C++ kernel and libraries.
- `python/similar2logo/` – Python package and DSL that can use the C++ engine.

High-level documentation and status:

- `MULTITHREADED_ENGINE_PYTHON.md` – summary of the Python/C++ integration.
- `CPP_ENGINE_README.md` – build + usage guide for the C++ engine.
- `docs/CPP_ENGINE.md` – extended documentation for the multithreaded engine.
- `docs/MULTITHREADING.md` – architecture and threading model.

### Architecture

The execution path is:

```text
Python DSL (similar2logo.dsl) → pybind11 bindings → C++ multithreaded engine
```

Key ideas:

- **Python side**:
  - Simulations are declared using a high-level DSL (`Simulation`, `Turtle`, etc.).
  - Users write agent logic in Python (e.g. Boids, ant foraging, predator–prey).
- **C++ side**:
  - `LogoAgent` and related types manage state and spatial indexing.
  - `LogoSimulationModel` and a **multithreaded engine** execute decision and reaction phases in parallel.
  - A `PythonDecisionModel` bridge uses pybind11 to call back into Python for agent decisions, while still releasing the GIL and running threads.

### Building the C++ Engine

From the project root:

```bash
./build_cpp_engine.sh
```

This script configures and builds:

- The C++ multithreaded engine library.
- The Python extension module(s) used by `similar2logo`.

For more detail, see `CPP_ENGINE_README.md`.

### Using the Engine from Python

Once the C++ engine is built, Python automatically uses it when available. For example:

```python
from similar2logo.dsl import Simulation, Turtle

class Boid(Turtle):
    def decide(self, perception):
        neighbors = perception.get("nearby_turtles", [])
        influences = []
        # Flocking logic here; executed inside the C++ multithreaded engine
        return influences

(Simulation()
 .grid(100, 100)
 .agents(Boid, 1000)
 .run_web())
```

You can verify that the C++ engine is active with:

```python
from similar2logo.cpp_engine import use_cpp_engine
print(use_cpp_engine())  # True if C++ engine is available
```

### Performance

Performance measurements (see `docs/CPP_ENGINE_COMPLETE.md` and `MULTITHREADED_ENGINE_PYTHON.md`) show large speedups over the pure‑Python engine, especially for large numbers of agents:

- Hundreds to thousands of agents run at much higher frame rates.
- True parallel execution during decision phases (GIL released).
- Spatial indexing and optimized reactions implemented in C++.

Typical speedups (rough order of magnitude):

- 100 agents: several times faster.
- 1000+ agents: one to two orders of magnitude faster, depending on the scenario.

Exact numbers and benchmarks are documented in the C++ engine reports in `docs/`.

### Relationship to SIMILAR Documentation

Similar2Logo is built on the same SIMILAR foundations:

- Agents and levels (here, Logo grids).
- Influences and reactions.
- Probes and visualization.

The original conceptual documentation in `src/doc` remains the reference for the multi‑level simulation model. The files in `cpp/doc` (and the detailed reports in `docs/` and `CPP_ENGINE_*.md`) describe how these ideas are implemented and optimized in the C++/Python multithreaded engine used by Similar2Logo.


