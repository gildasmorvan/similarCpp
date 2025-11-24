# SIMILAR2Logo Python - Probe System Implementation

## Overview

The probe system has been fully implemented in the Python port, matching the functionality of the original Java implementation. Probes allow you to observe and control simulations at various points in their execution.

## What Are Probes?

Probes are observers that can:
- Monitor simulation state at each step
- Control simulation timing (speed up/slow down)
- Log data for analysis
- Trigger actions based on simulation state
- Provide custom visualization or metrics

## Implemented Probes

### RealTimeMatcherProbe

The `RealTimeMatcherProbe` controls simulation speed to match real time or a multiple thereof.

**Constructor:**
```python
RealTimeMatcherProbe(acceleration_factor: float = 1.0)
```

**Parameters:**
- `acceleration_factor`: Speed multiplier relative to real time
  - `1.0` = real time (1 simulation second = 1 real second)
  - `10.0` = 10x faster than real time
  - `0.5` = half speed (slow motion)

**Example Usage:**
```python
from similar2logo import LogoSimulation, Environment, RealTimeMatcherProbe

# Create simulation
env = Environment(100, 100)
sim = LogoSimulation(env)

# Add probe for 10x speed
sim.probe_manager.add_probe("timing", RealTimeMatcherProbe(10.0))

# Run simulation - will automatically be paced
sim.run(steps=100)
```

## Creating Custom Probes

You can create custom probes by extending the `IProbe` interface:

```python
from similar2logo.probes import IProbe

class LoggingProbe(IProbe):
    """Logs simulation state at each step."""
    
    def __init__(self, log_file):
        self.log_file = log_file
        self.file = None
    
    def observe_at_initial_time(self, initial_time, simulation):
        """Called when simulation starts."""
        self.file = open(self.log_file, 'w')
        self.file.write(f"Simulation started at t={initial_time}\n")
        self.file.write(f"Number of agents: {len(simulation.turtles)}\n")
    
    def observe_at_partial_consistent_time(self, current_time, simulation):
        """Called at each simulation step."""
        self.file.write(f"t={current_time}: {len(simulation.turtles)} agents\n")
    
    def observe_at_final_time(self, final_time, simulation):
        """Called when simulation ends."""
        self.file.write(f"Simulation ended at t={final_time}\n")
        self.file.close()

# Usage
sim.probe_manager.add_probe("logger", LoggingProbe("simulation.log"))
```

## Probe Manager API

The `ProbeManager` class manages all probes for a simulation:

```python
# Add a probe
sim.probe_manager.add_probe(name: str, probe: IProbe)

# Remove a probe
sim.probe_manager.remove_probe(name: str)

# Access all probes
sim.probe_manager.probes  # Dictionary of name -> probe
```

## Common Use Cases

### 1. Visualization at Real-Time Speed
```python
# Slow down to real-time for visualization
sim.probe_manager.add_probe("timing", RealTimeMatcherProbe(1.0))
```

### 2. Fast Batch Processing
```python
# Speed up 100x for data collection
sim.probe_manager.add_probe("timing", RealTimeMatcherProbe(100.0))
```

### 3. Slow Motion Analysis
```python
# Half speed for detailed observation
sim.probe_manager.add_probe("timing", RealTimeMatcherProbe(0.5))
```

### 4. Multiple Probes
```python
# Combine timing control with logging
sim.probe_manager.add_probe("timing", RealTimeMatcherProbe(10.0))
sim.probe_manager.add_probe("logger", LoggingProbe("sim.log"))
sim.probe_manager.add_probe("metrics", MetricsProbe())
```

## Performance Impact

The `RealTimeMatcherProbe` has minimal overhead:
- Only sleeps when simulation is running too fast
- Uses high-precision timing (`time.perf_counter()`)
- Negligible CPU usage when pacing

## Comparison with Java Implementation

The Python implementation matches the Java version:

| Feature | Java | Python | Status |
|---------|------|--------|--------|
| IProbe interface | ✅ | ✅ | Complete |
| RealTimeMatcherProbe | ✅ | ✅ | Complete |
| Probe manager | ✅ | ✅ | Complete |
| Custom probes | ✅ | ✅ | Complete |
| Timing precision | ✅ | ✅ | Complete |

## Examples

See `examples/python/probe_example.py` for a complete working example demonstrating:
- Basic probe usage
- Different acceleration factors
- Timing verification

Run it with:
```bash
source .venv/bin/activate
export PYTHONPATH=$PYTHONPATH:$(pwd)/python
python examples/python/probe_example.py
```

## Files Modified/Created

1. **`python/similar2logo/probes.py`** (NEW)
   - `IProbe` interface
   - `RealTimeMatcherProbe` implementation
   - `ProbeManager` class

2. **`python/similar2logo/model.py`**
   - Added `probe_manager` to `LogoSimulation`
   - Integrated probe notifications in `step()` and `run()`

3. **`python/similar2logo/__init__.py`**
   - Exported probe classes

4. **`examples/python/probe_example.py`** (NEW)
   - Demonstration of probe usage

## Future Enhancements

Potential additional probes to implement:
- **ConsoleProbe**: Print simulation state to console
- **CSVProbe**: Export data to CSV files
- **PlotProbe**: Real-time plotting with matplotlib
- **CheckpointProbe**: Save simulation state periodically
- **PerformanceProbe**: Track FPS and timing statistics
