## Python Bindings for the C++ Engine

### Overview

Several components of the SIMILAR ecosystem expose their **C++ implementations** to **Python**:

- The **JamFree** C++ traffic engine (`cpp/jamfree`) via a `jamfree` Python package.
- The **Similar2Logo** C++ multithreaded engine (for Logo-style simulations) via Python bindings used by the `similar2logo` DSL.

Bindings are implemented with **pybind11**, providing a clean, Pythonic API while running high‑performance C++ code underneath.

### JamFree Python Bindings

The JamFree C++ engine exposes:

- Core geometry and model types (e.g. `Point2D`, `Road`, `Lane`, `Vehicle`).
- Microscopic traffic models (`IDM`, `IDMPlus`, `MOBIL`).
- Utility functions (`kmh_to_ms`, `ms_to_kmh`).

The main components are:

- `cpp/jamfree/python/src/bindings.cpp` – pybind11 bindings.
- `cpp/jamfree/python/jamfree/__init__.py` – Python package.
- `cpp/jamfree/python/examples/highway_simulation.py` – working example.
- `cpp/jamfree/PYTHON_BINDINGS_SUMMARY.md` – detailed implementation summary.

#### Building and Installing `jamfree`

From the `cpp/jamfree` directory:

```bash
cd cpp/jamfree
./build_python.sh
```

or manually:

```bash
cd cpp/jamfree
python3 setup.py build_ext --inplace
pip3 install -e .
```

Verify the installation:

```bash
python3 -c "import jamfree; print(jamfree.__version__)"
```

#### Basic JamFree Usage (Python)

```python
import jamfree

# Create a road
road = jamfree.Road(
    "highway_1",
    jamfree.Point2D(0, 0),
    jamfree.Point2D(1000, 0),
    2,      # lanes
    3.5     # lane width (m)
)

# Vehicle and model
vehicle = jamfree.Vehicle("car_1")
vehicle.set_current_lane(road.get_lane(0))
vehicle.set_speed(jamfree.kmh_to_ms(100))

idm = jamfree.IDM(
    desired_speed=jamfree.kmh_to_ms(120),
    time_headway=1.5,
    min_gap=2.0,
)

dt = 0.1
for step in range(100):
    accel = idm.calculate_acceleration(vehicle, leader=None)
    vehicle.update(dt, accel)
```

For a complete example, see `cpp/jamfree/python/examples/highway_simulation.py`.

### Web UI and Optimized Engine (JamFree)

JamFree also provides a **Python/web frontend** that drives the C++ engine:

- Web UI scripts and API in `cpp/jamfree/python/web`.
- Quick start and performance expectations in `cpp/jamfree/QUICK_START.md`.
- Optimization and GPU details in the various `*_OPTIMIZATIONS.md` and `GPU_*` markdown files.

The typical workflow is:

```bash
cd cpp/jamfree
./run_web_ui.sh
# Then open http://localhost:5001 in a browser
```

Configuration (vehicle count, optimizations, etc.) is done via HTTP APIs or Python helper scripts, as described in `QUICK_START.md` and related files.

### Similar2Logo C++ Multithreaded Engine

The **Similar2Logo** project uses a C++ multithreaded engine to run Logo-style simulations from Python:

- C++ engine and bindings live under `cpp/similar2logo` and the top-level `python/similar2logo` package.
- The Python DSL (`similar2logo.dsl`) automatically uses the C++ engine when it is available.

Key references:

- `MULTITHREADED_ENGINE_PYTHON.md` – high-level summary of the Python integration.
- `CPP_ENGINE_README.md` – C++ engine usage and examples.

#### Building the Similar2Logo C++ Engine

From the project root:

```bash
./build_cpp_engine.sh
```

This compiles the C++ multithreaded engine and its Python bindings.

#### Using the C++ Engine from Python

Once built, Python simulations automatically use the C++ engine if present. For example:

```python
from similar2logo.dsl import Simulation, Turtle

class Boid(Turtle):
    def decide(self, perception):
        neighbors = perception.get("nearby_turtles", [])
        influences = []
        # Flocking logic here; decision phase is executed by the C++ engine
        return influences

(Simulation()
 .grid(100, 100)
 .agents(Boid, 1000)
 .run_web())
```

The architecture is:

```text
Python DSL → pybind11 bindings → C++ multithreaded engine
```

### Relationship to the Original Documentation

The original SIMILAR documentation in `src/doc` explains the **conceptual model**:

- Multi-level agents, levels, influences, reactions, time models, and probes.

The C++ port and Python bindings documented in `cpp/doc` should be read as:

- Concrete **implementations** of those concepts in C++.
- High-performance **execution engines** for Python users.

For theory and background, refer to `src/doc`. For how to build and use the C++ and Python layers, use the markdown files in `cpp/doc` together with the more detailed module-specific files in `cpp/` and `cpp/jamfree/`.


