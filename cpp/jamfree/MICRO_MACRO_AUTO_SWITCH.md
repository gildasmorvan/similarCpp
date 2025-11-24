# JamFree Automatic Micro-Macro Mode Assignment

## What Was Implemented

The simulation now **automatically assigns simulation modes** to lanes based on their geometry:

- **Short lanes (< 100m)**: Forced to **MICROSCOPIC** mode
  - Typically crossings, intersections, ramps
  - Individual vehicle tracking
  - More accurate but slower

- **Long lanes (>= 100m)**: Forced to **MACROSCOPIC** mode
  - Typically highways, main roads
  - Continuum flow model
  - Faster simulation

## How to See It Working

### 1. Start the Server
The server should already be running on http://localhost:5001

### 2. Load a Map
- Open the web UI in your browser
- Either upload an OSM file or download an area from the map

### 3. Start Simulation
- Click "▶️ Start"
- **Check the server console** - you should see:
  ```
  Mode assignment: X Micro (short/crossing), Y Macro (long)
  ```

### 4. Visual Feedback

On the map, you should see lanes colored by mode:
- **🟢 Green lanes** = MICROSCOPIC (short roads, crossings)
- **🔵 Blue lanes** = MACROSCOPIC (long roads, highways)
- **🟠 Orange lanes** = TRANSITIONING (rare, only during mode switches)

Blue lanes will also be **thicker** than green lanes.

### 5. Browser Console
Open your browser's developer console (F12) and you'll see:
```
🎯 Lane Modes: X Micro (green), Y Macro (blue), 0 Transitioning (orange)
```

## Code Changes

### Python (`app.py`)
- Modified lane registration loop in `start_simulation()`
- Added geometry-based mode forcing:
  - `force_microscopic()` for lanes < 100m
  - `force_macroscopic()` for lanes >= 100m

### JavaScript (`index.html`)
- Added console logging to show mode distribution
- Existing visualization already supported mode-based coloring

## No Recompilation Needed

This feature works **without recompiling** because:
- The C++ `AdaptiveSimulator` already has `force_microscopic()` and `force_macroscopic()` methods
- These are already exposed via Python bindings
- We're just calling them during initialization

## Testing

1. Load a real-world map with varied road types
2. Start simulation
3. Observe:
   - Short connector roads → Green
   - Long highways → Blue
   - Performance improvement from macro mode on highways
