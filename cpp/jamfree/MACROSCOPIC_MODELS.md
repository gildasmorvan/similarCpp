# Macroscopic Models & Micro-Macro Bridge

## Overview

JamFree now includes macroscopic traffic flow models alongside the existing microscopic models, providing a complete multi-scale traffic simulation framework.

## Macroscopic Models

### 1. LWR (Lighthill-Whitham-Richards) Model

The LWR model is a first-order macroscopic model based on conservation of vehicles.

**Key Features:**
- Continuum traffic flow representation
- Fundamental diagram (speed-density relationship)
- Godunov numerical scheme for stability
- Captures shockwaves and rarefaction waves

**Usage:**
```python
import jamfree

# Create LWR model
lwr = jamfree.LWR(
    free_flow_speed=33.3,  # 120 km/h in m/s
    jam_density=0.15,       # 150 vehicles/km
    road_length=1000.0,     # 1 km
    num_cells=100           # Discretization
)

# Set initial density
for i in range(lwr.get_num_cells()):
    lwr.set_density(i, 0.05)  # 50 vehicles/km

# Simulate
dt = 0.1  # 100ms time step
lwr.update(dt)

# Query state
density = lwr.get_density(50)  # Cell 50
flow = lwr.get_flow(50)
speed = lwr.get_speed(50)
```

**Mathematical Foundation:**
- Conservation equation: ∂ρ/∂t + ∂q/∂x = 0
- Fundamental diagram: v(ρ) = v_f * (1 - ρ/ρ_jam) (Greenshields)
- Flow: q(ρ) = ρ * v(ρ)

### 2. CTM (Cell Transmission Model)

The CTM is a discrete version of LWR that explicitly represents flow between cells.

**Key Features:**
- Discrete cell-based representation
- Sending and receiving flow functions
- Natural handling of bottlenecks
- Efficient for network simulation

**Usage:**
```python
import jamfree

# Create CTM model
ctm = jamfree.CTM(
    free_flow_speed=33.3,  # 120 km/h
    wave_speed=5.56,        # 20 km/h (backward wave)
    jam_density=0.15,
    road_length=1000.0,
    num_cells=100
)

# Set initial state
cell_length = ctm.get_cell_length()
for i in range(ctm.get_num_cells()):
    num_vehicles = 0.05 * cell_length  # 50 veh/km
    ctm.set_num_vehicles(i, num_vehicles)

# Simulate
dt = 0.1
ctm.update(dt)

# Query state
num_veh = ctm.get_num_vehicles(50)
density = ctm.get_density(50)
flow = ctm.get_flow(50)
```

**Mathematical Foundation:**
- Sending flow: S(n) = min(n/Δt, Q_max)
- Receiving flow: R(n) = min(N_max - n, Q_max)
- Update: n_i(t+Δt) = n_i(t) + flow_in - flow_out

## Micro-Macro Bridge

The `MicroMacroBridge` class provides seamless integration between microscopic and macroscopic scales.

### Extract Macroscopic State from Microscopic Simulation

```python
import jamfree

# Microscopic simulation
road = jamfree.Road("road1", start, end, num_lanes=1)
lane = road.get_lane(0)

# ... add vehicles and simulate ...

# Extract macroscopic profiles
num_cells = 50
density_profile = jamfree.MicroMacroBridge.extract_density_profile(lane, num_cells)
flow_profile = jamfree.MicroMacroBridge.extract_flow_profile(lane, num_cells)
speed_profile = jamfree.MicroMacroBridge.extract_speed_profile(lane, num_cells)

# Get aggregate statistics
stats = jamfree.MicroMacroBridge.calculate_aggregate_stats(lane)
print(f"Avg density: {stats.avg_density} veh/m")
print(f"Avg flow: {stats.avg_flow} veh/s")
print(f"Avg speed: {stats.avg_speed} m/s")
```

### Initialize Macroscopic Model from Microscopic State

```python
# Create macroscopic models
lwr = jamfree.LWR(road_length=1000.0, num_cells=50)
ctm = jamfree.CTM(road_length=1000.0, num_cells=50)

# Initialize from microscopic lane
jamfree.MicroMacroBridge.initialize_lwr_from_lane(lane, lwr)
jamfree.MicroMacroBridge.initialize_ctm_from_lane(lane, ctm)

# Now macroscopic models have same state as microscopic
```

### Calibrate Macroscopic Parameters

```python
# Estimate parameters from microscopic data
free_flow_speed = jamfree.MicroMacroBridge.estimate_free_flow_speed(lane)
jam_density = jamfree.MicroMacroBridge.estimate_jam_density(lane)

# Calibrate fundamental diagram
densities, speeds = jamfree.MicroMacroBridge.calibrate_fundamental_diagram(lane)

# Use calibrated parameters
lwr = jamfree.LWR(
    free_flow_speed=free_flow_speed,
    jam_density=jam_density
)
```

## Hybrid Simulation Strategies

### Strategy 1: Microscopic Core, Macroscopic Periphery

Use microscopic simulation in regions of interest (e.g., intersections, incidents) and macroscopic elsewhere.

```python
# Microscopic simulation for critical area
critical_lanes = [...]  # Lanes near incident
for lane in critical_lanes:
    # Detailed IDM/MOBIL simulation
    ...

# Macroscopic simulation for rest of network
for road_segment in peripheral_segments:
    lwr = jamfree.LWR(...)
    lwr.update(dt)
```

### Strategy 2: Multi-Resolution Temporal

Use microscopic for short-term predictions, macroscopic for long-term.

```python
# Short-term (next 5 minutes): Microscopic
for t in range(300):  # 5 minutes
    # IDM updates
    ...

# Long-term (next hour): Extract to macroscopic
lwr = jamfree.LWR(...)
jamfree.MicroMacroBridge.initialize_lwr_from_lane(lane, lwr)

for t in range(3600):  # 1 hour
    lwr.update(dt)
```

### Strategy 3: Calibration Loop

Continuously calibrate macroscopic models from microscopic observations.

```python
# Run microscopic simulation
for step in range(calibration_steps):
    # Update vehicles
    ...

# Calibrate macroscopic model
densities, speeds = jamfree.MicroMacroBridge.calibrate_fundamental_diagram(lane)

# Use calibrated model for prediction
lwr = jamfree.LWR(...)
# ... set parameters based on calibration ...
```

## Performance Comparison

| Model | Vehicles | Computation Time | Use Case |
|-------|----------|------------------|----------|
| Microscopic (IDM) | 100 | 10ms/step | Detailed behavior, small networks |
| Microscopic (IDM) | 1000 | 100ms/step | Medium networks with threading |
| LWR | N/A (continuum) | 1ms/step | Large networks, long-term prediction |
| CTM | N/A (continuum) | 2ms/step | Network simulation, bottlenecks |
| Hybrid | 100 micro + macro | 15ms/step | Best of both worlds |

## API Reference

### LWR Class

**Constructor:**
```python
LWR(free_flow_speed=33.3, jam_density=0.15, road_length=1000.0, num_cells=100)
```

**Methods:**
- `update(dt)`: Update traffic state
- `set_density(cell_index, density)`: Set density at cell
- `get_density(cell_index)`: Get density at cell
- `get_flow(cell_index)`: Get flow at cell
- `get_speed(cell_index)`: Get speed at cell
- `speed_from_density(density)`: Calculate speed from density
- `flow_from_density(density)`: Calculate flow from density
- `get_critical_density()`: Get critical density
- `get_max_flow()`: Get maximum flow

### CTM Class

**Constructor:**
```python
CTM(free_flow_speed=33.3, wave_speed=5.56, jam_density=0.15, 
    road_length=1000.0, num_cells=100)
```

**Methods:**
- `update(dt)`: Update traffic state
- `set_num_vehicles(cell_index, num_vehicles)`: Set vehicles in cell
- `get_num_vehicles(cell_index)`: Get vehicles in cell
- `get_density(cell_index)`: Get density at cell
- `get_flow(cell_index)`: Get flow at cell
- `get_speed(cell_index)`: Get speed at cell
- `sending_flow(num_vehicles, dt)`: Calculate sending flow
- `receiving_flow(num_vehicles, dt)`: Calculate receiving flow

### MicroMacroBridge Class

**Static Methods:**
- `extract_density_profile(lane, num_cells)`: Extract density from lane
- `extract_flow_profile(lane, num_cells)`: Extract flow from lane
- `extract_speed_profile(lane, num_cells)`: Extract speed from lane
- `initialize_lwr_from_lane(lane, lwr)`: Initialize LWR from lane
- `initialize_ctm_from_lane(lane, ctm)`: Initialize CTM from lane
- `calibrate_fundamental_diagram(lane, num_samples)`: Calibrate FD
- `estimate_free_flow_speed(lane)`: Estimate v_f
- `estimate_jam_density(lane, vehicle_length)`: Estimate ρ_jam
- `calculate_aggregate_stats(lane)`: Get aggregate statistics

## Examples

See `examples/python/macroscopic_example.py` for comprehensive examples including:
- LWR simulation with traffic waves
- CTM simulation with congestion
- Fundamental diagram visualization
- Micro-macro bridge demonstration

Run with:
```bash
cd examples/python
python macroscopic_example.py
```

## References

1. Lighthill, M. J., & Whitham, G. B. (1955). "On kinematic waves II. A theory of traffic flow on long crowded roads." Proceedings of the Royal Society of London.

2. Daganzo, C. F. (1994). "The cell transmission model: A dynamic representation of highway traffic consistent with the hydrodynamic theory." Transportation Research Part B.

3. Treiber, M., & Kesting, A. (2013). "Traffic Flow Dynamics: Data, Models and Simulation." Springer.

## Philosophy

The JamFree macroscopic implementation follows the same design principles as the microscopic models:

1. **Modularity**: Each model is self-contained and can be used independently
2. **Extensibility**: Easy to add new macroscopic models (e.g., ARZ, Aw-Rascle)
3. **Integration**: Seamless bridge between scales via `MicroMacroBridge`
4. **Performance**: Optimized C++ core with Python bindings
5. **Validation**: Models implement well-established, peer-reviewed formulations

This multi-scale approach enables:
- **Flexibility**: Choose the right model for the task
- **Efficiency**: Use macroscopic where appropriate, microscopic where needed
- **Accuracy**: Calibrate macroscopic from microscopic observations
- **Scalability**: Simulate large networks efficiently
