# Enhanced Web Interface - User Guide

## Overview

The SIMILAR2Logo web interface now includes **interactive controls** for:
- ⚡ **Real-time speed adjustment** (0.1x to 100x)
- 🎛️ **Live parameter modification** (all model parameters)
- 📊 **Real-time statistics** (FPS, step count, agent count)
- 🎨 **Modern, responsive UI** with gradient design

## Features

### 1. Simulation Control Panel

**Buttons:**
- **▶ Start**: Begin the simulation
- **⏸ Pause**: Pause without resetting
- **⏹ Stop**: Stop the simulation
- **🔄 Reset**: Reset to initial state

### 2. Speed Control

**Interactive Slider:**
- Range: 0.1x (slow motion) to 100x (ultra-fast)
- Real-time adjustment while simulation is running
- Visual feedback with current speed display
- Powered by `RealTimeMatcherProbe`

**Use Cases:**
```python
# Slow motion for detailed observation
Speed: 0.5x

# Real-time visualization
Speed: 1.0x

# Fast data collection
Speed: 50.0x - 100.0x
```

### 3. Model Parameters

**Dynamic Parameter Detection:**
The interface automatically detects and exposes adjustable parameters from your turtle agents:

- `speed`: Agent movement speed
- `max_speed`: Maximum allowed speed
- `max_force`: Maximum steering force
- `perception_radius`: How far agents can see
- `separation_weight`: Separation behavior strength
- `alignment_weight`: Alignment behavior strength
- `cohesion_weight`: Cohesion behavior strength

**Live Updates:**
- Change parameters while simulation is running
- All agents update immediately
- No need to restart simulation

### 4. Statistics Dashboard

Real-time metrics:
- **Step**: Current simulation step
- **Agents**: Number of active agents
- **FPS**: Frames per second (rendering speed)
- **Speed**: Current simulation speed multiplier

## Usage Examples

### Basic Usage

```python
from similar2logo import LogoSimulation, Environment
from similar2logo.web import WebSimulation

# Create your simulation
env = Environment(100, 100, toroidal=True)
sim = LogoSimulation(env, num_turtles=50)

# Launch web interface with initial speed
web_sim = WebSimulation(sim, initial_speed=10.0)
web_sim.start_server(port=8080)
```

### Custom Initial Speed

```python
# Start at real-time speed
web_sim = WebSimulation(sim, initial_speed=1.0)

# Start at 50x speed for fast exploration
web_sim = WebSimulation(sim, initial_speed=50.0)

# Start in slow motion
web_sim = WebSimulation(sim, initial_speed=0.5)
```

### With Custom Parameters

Make sure your turtle class has adjustable attributes:

```python
class MyTurtle(Turtle):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.speed = 1.0
        self.perception_radius = 10.0
        self.custom_param = 5.0  # Will appear in UI!
```

## API Endpoints

The web interface exposes the following REST API:

### GET Endpoints

```
GET /                    - Web interface HTML
GET /api/state          - Current simulation state
GET /api/speed          - Current speed factor
GET /api/parameters     - Available parameters
```

### POST Endpoints

```
POST /api/start         - Start simulation
POST /api/pause         - Pause simulation
POST /api/resume        - Resume simulation
POST /api/stop          - Stop simulation
POST /api/reset         - Reset simulation

POST /api/speed         - Update speed
Body: {"speed": 10.0}

POST /api/parameters    - Update parameters
Body: {"parameters": {"speed": 2.0, "max_force": 0.5}}
```

### WebSocket

```
WS /ws                  - Real-time state updates
```

## Technical Details

### Speed Control Implementation

The speed control uses the `RealTimeMatcherProbe`:

```python
# In WebSimulation.__init__
self.speed_probe = RealTimeMatcherProbe(initial_speed)
self.simulation.probe_manager.add_probe("web_speed_control", self.speed_probe)

# When user adjusts slider
self.speed_probe.acceleration_factor = new_speed
```

### Parameter Updates

Parameters are updated via `setattr`:

```python
for turtle in simulation.turtles:
    if hasattr(turtle, parameter_name):
        setattr(turtle, parameter_name, new_value)
```

### Performance

- **Spatial indexing**: O(1) neighbor queries
- **Efficient rendering**: Canvas-based visualization
- **Optimized updates**: Only sends state when not paused
- **Low overhead**: Probe system adds <1% CPU usage

## Browser Compatibility

Tested and working on:
- ✅ Chrome/Edge (recommended)
- ✅ Firefox
- ✅ Safari
- ✅ Opera

## Keyboard Shortcuts

Currently none - all controls are mouse/touch-based.
Future enhancement: Add keyboard shortcuts for common actions.

## Troubleshooting

### Port Already in Use

```bash
# Find process using port 8080
lsof -i :8080

# Kill it
kill -9 <PID>
```

### Parameters Not Showing

Make sure your turtle class has the attributes as instance variables:

```python
# ✅ Good - will appear in UI
class MyTurtle(Turtle):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.my_param = 1.0

# ❌ Bad - won't appear
class MyTurtle(Turtle):
    MY_PARAM = 1.0  # Class variable
```

### Speed Control Not Working

Ensure you're using the latest version with probe support:

```python
# Check if probe manager exists
assert hasattr(sim, 'probe_manager')

# Check if speed probe is registered
assert 'web_speed_control' in sim.probe_manager.probes
```

## Examples

See these examples for reference:
- `examples/python/boids_flocking.py --web`
- `examples/python/segregation_model.py --web`

## Future Enhancements

Planned features:
- 📹 Record simulation as video
- 📊 Export data to CSV
- 🎨 Customizable color schemes
- ⌨️ Keyboard shortcuts
- 📈 Real-time charts/graphs
- 🔍 Agent inspection (click to view details)
- 🎯 Parameter presets/profiles
