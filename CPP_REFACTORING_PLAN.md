# C++ Backend Refactoring Plan

## Current Status

### What's Using C++ ✅
- **Environment**: Core environment methods (get_distance, get_direction, set/get_pheromone)
- **Point2D**: All geometric operations
- **MathUtil**: Math utilities (toroidal_distance, angle normalization, etc.)
- **FastMath**: Optimized sin/cos/sqrt

### What's Still in Python ❌
- **Reaction Model**: All influence processing (ChangePosition, EmitPheromone, etc.)
- **Pheromone Dynamics**: Diffusion and evaporation calculations
- **Spatial Indexing**: SpatialHashGrid for neighbor queries
- **Simulation Loop**: Main step() logic in LogoSimulation
- **Perception Building**: Creating perception dictionaries for agents

## Issues Found

1. **C++ Pheromone Return Type**: The C++ `Environment.add_pheromone()` returns a C++ Pheromone object that isn't properly registered with pybind11
   - **Fix**: Wrapped to catch TypeError and create Python Pheromone instead

2. **Missing Properties**: C++ Environment doesn't expose `width`, `height`, `toroidal` as properties
   - **Fix**: Created Python wrapper class that stores these

3. **Pheromone Grids**: Python reaction model expects `pheromone_grids` dict which doesn't exist in C++
   - **Status**: NEEDS FIX - Either implement C++ Reaction or make Python reaction work with C++ environment

## Refactoring Strategy

### Phase 1: Fix Current Issues ⚠️ IN PROGRESS
- [x] Wrap C++ Environment to add Python properties
- [x] Handle Pheromone return type issue
- [ ] Make reaction model work with C++ environment
  - Option A: Check if using C++ env and call C++ methods directly
  - Option B: Implement full C++ Reaction model

### Phase 2: Implement C++ Reaction Model
The C++ Reaction class exists but is just a stub. Need to implement:
- `_process_change_position_influences()`
- `_process_change_direction_influences()`
- `_process_change_speed_influences()`
- `_process_emit_pheromone_influences()`
- `_pheromone_diffusion()`
- `_pheromone_evaporation()`
- System influences (add/remove agents)

### Phase 3: Move Spatial Indexing to C++
- Implement `SpatialHashGrid` in C++
- Bind to Python
- Update `LogoSimulation` to use C++ version

### Phase 4: Optimize Simulation Loop
- Move main `step()` logic to C++
- Keep Python DSL as thin wrapper
- Perception building in C++

## Files to Refactor

### High Priority (Core Performance)
1. **`reaction.py`** (314 lines) → Implement C++ `Reaction::apply()`
   - Most CPU-intensive part of simulation
   - Processes all influences every step
   - Pheromone dynamics (diffusion/evaporation)

2. **`spatial.py`** (109 lines) → Implement C++ `SpatialHashGrid`
   - O(N) neighbor queries without it
   - Critical for large simulations

### Medium Priority
3. **`model.py`** (481 lines) → Move `step()` to C++
   - Main simulation loop
   - Perception building
   - Influence collection

### Low Priority (Already Thin)
4. **`environment.py`** (166 lines → 60 lines) ✅ DONE
   - Now just a thin wrapper
   - All heavy computation in C++

5. **`tools.py`** - Already using C++ Point2D and MathUtil

## Performance Impact Estimate

| Component | Current | After C++ | Speedup |
|-----------|---------|-----------|---------|
| Environment ops | Mixed | 100% C++ | 2-3x |
| Reaction model | 100% Python | 100% C++ | 5-10x |
| Spatial indexing | 100% Python | 100% C++ | 3-5x |
| Overall simulation | ~30% C++ | ~90% C++ | 4-8x |

## Next Steps

1. **Immediate**: Fix pheromone_grids issue in reaction.py
2. **Short-term**: Implement C++ Reaction model
3. **Medium-term**: Implement C++ SpatialHashGrid
4. **Long-term**: Move simulation loop to C++

## Code Reduction Achieved

- `environment.py`: 166 lines → ~140 lines (mostly wrapper/fallback)
- More reduction possible after implementing C++ Reaction

## Testing

All examples should continue to work:
- ✅ boids.py
- ⚠️ forest_fire.py (pheromone_grids issue)
- ✅ randomwalk_1d.py
- ✅ passive_turtle.py
- ✅ circle.py
