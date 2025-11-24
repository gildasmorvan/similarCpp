# JamFree Python Examples

This directory contains example scripts demonstrating the features of the JamFree traffic simulation library.

## Adaptive Hybrid Simulation
These examples demonstrate the micro-macro hybrid simulation capabilities:

- **`adaptive_hybrid_example.py`**: General demonstration of dynamic mode switching.
- **`osm_hybrid_simulation.py`**: Loads a real-world OSM network (highway junction) and runs the adaptive simulation.
- **`incident_response.py`**: Simulates an accident and shows how to force detailed microscopic simulation around the incident area.
- **`traffic_signal_optimization.py`**: Demonstrates a "split lane" strategy for intersections, using critical areas for queue discharge.

## GPU Acceleration
- **`gpu_metal_example.py`**: Benchmarks CPU vs. GPU performance using Apple Metal. Requires macOS.

## Other Simulations
- **`city_scale_simulation.py`**: Large scale simulation demo.
- **`macroscopic_example.py`**: Pure macroscopic simulation (LWR/CTM models).
- **`realistic_simulation.py`**: Simulation with realistic parameters and behaviors.

## Running Examples
Ensure `jamfree` is installed or in your `PYTHONPATH`:

```bash
export PYTHONPATH=../../python:$PYTHONPATH
python3 adaptive_hybrid_example.py
```

## Web UI
Most examples can be visualized using the Web UI. Run the Flask app:

```bash
cd ../../python/web
python3 app.py
```
Then open `http://localhost:5000`.
