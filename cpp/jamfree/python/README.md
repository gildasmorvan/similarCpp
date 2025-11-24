# JamFree Python Bindings

Python bindings for the JamFree C++ traffic simulation library.

## Features

- **Microscopic Traffic Simulation**: Individual vehicle dynamics
- **IDM Car-Following**: Intelligent Driver Model (IDM and IDM+)
- **MOBIL Lane-Changing**: Realistic lane-changing decisions
- **Multi-Lane Highways**: Support for multiple lanes
- **High Performance**: C++ backend with Python interface

## Installation

### Prerequisites

- Python 3.7+
- C++17 compiler
- pybind11

### Build from source

```bash
cd cpp/jamfree
pip install pybind11
python setup.py build_ext --inplace
python setup.py install
```

Or for development:

```bash
pip install -e .
```

## Quick Start

```python
import jamfree

# Create a highway
highway = jamfree.Road(
    "highway_1",
    jamfree.Point2D(0, 0),
    jamfree.Point2D(1000, 0),  # 1 km
    2,    # 2 lanes
    3.5   # 3.5m lane width
)

# Create a vehicle
vehicle = jamfree.Vehicle("car_1")
vehicle.set_current_lane(highway.get_lane(0))
vehicle.set_speed(jamfree.kmh_to_ms(100))  # 100 km/h

# Create IDM car-following model
idm = jamfree.IDM(
    desired_speed=jamfree.kmh_to_ms(120),
    time_headway=1.5,
    min_gap=2.0
)

# Calculate acceleration
accel = idm.calculate_acceleration(vehicle, leader=None)

# Update vehicle
dt = 0.1  # 0.1 second time step
vehicle.update(dt, accel)

print(f"Speed: {jamfree.ms_to_kmh(vehicle.get_speed()):.1f} km/h")
```

## API Reference

### Core Classes

#### `Point2D(x, y)`
2D point/vector.

**Methods:**
- `distance_to(other)` - Distance to another point
- `dot(other)` - Dot product
- `length()` - Vector length
- `normalized()` - Normalized vector

#### `Road(id, start, end, num_lanes, lane_width)`
Road segment with multiple lanes.

**Methods:**
- `get_lane(index)` - Get lane by index
- `get_length()` - Get road length
- `get_num_lanes()` - Get number of lanes

#### `Lane(id, index, width, length)`
Individual lane within a road.

**Methods:**
- `get_index()` - Get lane index
- `get_speed_limit()` - Get speed limit
- `get_vehicles()` - Get all vehicles in lane

#### `Vehicle(id, length=5.0, max_speed=55.0, max_accel=3.0, max_decel=6.0)`
Individual vehicle.

**Methods:**
- `get_speed()` - Get current speed (m/s)
- `get_position()` - Get 2D position
- `get_lane_position()` - Get position along lane
- `set_speed(speed)` - Set speed
- `update(dt, acceleration)` - Update state
- `get_gap_to(leader)` - Get gap to vehicle ahead

### Car-Following Models

#### `IDM(desired_speed, time_headway, min_gap, max_accel, comfortable_decel, accel_exponent=4.0)`
Intelligent Driver Model.

**Methods:**
- `calculate_acceleration(vehicle, leader=None)` - Calculate acceleration
- `calculate_desired_gap(speed, speed_diff)` - Calculate desired gap

#### `IDMPlus(...)`
Enhanced IDM with improved emergency braking.

### Lane-Changing Models

#### `MOBIL(politeness=0.5, threshold=0.1, max_safe_decel=4.0, bias_right=0.3)`
Minimizing Overall Braking Induced by Lane changes.

**Methods:**
- `decide_lane_change(vehicle, current_lane, left_lane, right_lane, car_following_model)` - Decide lane change

**Returns:** `LaneChangeDirection` (NONE, LEFT, or RIGHT)

### Utility Functions

- `kmh_to_ms(kmh)` - Convert km/h to m/s
- `ms_to_kmh(ms)` - Convert m/s to km/h

## Examples

See `python/examples/` for complete examples:

- `highway_simulation.py` - Multi-lane highway with lane changing

## Performance

The Python bindings provide near-native C++ performance:

- **Simulation speed**: ~10,000 vehicles/second on typical hardware
- **Memory overhead**: Minimal (shared pointers)
- **GIL**: Released during computation-heavy operations

## License

CeCILL-B Free Software License Agreement

## Citation

If you use JamFree in your research, please cite:

```
@software{jamfree2025,
  title={JamFree: Traffic Simulation Library},
  author={Morvan, Gildas},
  year={2025},
  url={https://github.com/gildasmorvan/similar}
}
```

## References

- **IDM**: Treiber, M., Hennecke, A., & Helbing, D. (2000). "Congested traffic states in empirical observations and microscopic simulations." Physical Review E, 62(2), 1805.
- **MOBIL**: Kesting, A., Treiber, M., & Helbing, D. (2007). "General lane-changing model MOBIL for car-following models." Transportation Research Record, 1999(1), 86-94.
