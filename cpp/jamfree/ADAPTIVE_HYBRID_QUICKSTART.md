# Quick Start: Adaptive Hybrid Simulation

## Installation

```bash
cd /Users/morvan/Antigravity/similar/cpp/jamfree
rm -rf build
python3 setup.py build_ext --inplace
```

## Basic Usage

```python
import jamfree as jf

# 1. Create configuration
config = jf.AdaptiveSimulatorConfig()
config.micro_to_macro_density = 0.08  # Switch to macro at 80 veh/km
config.macro_to_micro_density = 0.04  # Switch to micro at 40 veh/km

# 2. Create simulator
simulator = jf.AdaptiveSimulator(config)

# 3. Register lanes
for lane in lanes:
    is_critical = lane.get_length() < 50  # Mark short lanes as critical
    simulator.register_lane(lane, is_critical)

# 4. Run simulation
idm = jf.IDM(desired_speed=30.0)
for step in range(num_steps):
    simulator.update(dt=0.1, idm=idm)
    
    # Monitor performance
    stats = simulator.get_statistics()
    print(f"Speedup: {stats.speedup_factor:.2f}x")
```

## Run Example

```bash
cd /Users/morvan/Antigravity/similar/cpp/jamfree
PYTHONPATH=python:$PYTHONPATH python3 examples/python/adaptive_hybrid_example.py
```

## Configuration Presets

### Conservative (Prefer Accuracy)
```python
config.micro_to_macro_density = 0.10  # 100 veh/km
config.macro_to_micro_density = 0.03  # 30 veh/km
```

### Balanced (Default)
```python
config.micro_to_macro_density = 0.08  # 80 veh/km
config.macro_to_micro_density = 0.04  # 40 veh/km
```

### Aggressive (Prefer Speed)
```python
config.micro_to_macro_density = 0.06  # 60 veh/km
config.macro_to_micro_density = 0.05  # 50 veh/km
```

## Key Concepts

- **Microscopic**: Individual vehicle tracking (IDM model)
- **Macroscopic**: Continuum flow model (LWR model)
- **Adaptive**: Automatic switching based on conditions
- **Critical Areas**: Always use microscopic (intersections, ramps)
- **Hysteresis**: Prevent rapid mode oscillation

## Performance

- **Speedup**: 4-10x vs pure microscopic
- **Scalability**: Handle 100,000+ vehicles
- **Accuracy**: Preserved at critical points

## Documentation

See `MICRO_MACRO_INTERFACE_COMPLETE.md` for full details.
