# C++ Engine Implementation Plan

## Objective

Move core SIMILAR2Logo simulation logic to C++ while maintaining a Python-friendly API. Python should be primarily for configuration and examples, with the heavy lifting done in multithreaded C++.

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                     Python Layer (Thin)                      │
│  - DSL Configuration                                         │
│  - Agent Definitions (can call C++ or stay in Python)       │
│  - Web Interface                                             │
│  - Examples                                                  │
└────────────────────┬────────────────────────────────────────┘
                     │ pybind11
┌────────────────────┴────────────────────────────────────────┐
│                  C++ Core (Heavy Lifting)                    │
│  - LogoAgent (C++ base class)                               │
│  - LogoSimulationModel                                       │
│  - MultiThreadedSimulationEngine                             │
│  - Spatial Indexing                                          │
│  - Reaction Models                                           │
│  - Influence Processing                                      │
└─────────────────────────────────────────────────────────────┘
```

## Implementation Status

### ✅ Completed

1. **C++ Agent Framework**
   - `LogoAgent.h/cpp`: Base C++ agent class
   - `LogoPerceivedData`: Perception data structure
   - `PythonDecisionModel`: Bridge for Python decision callbacks

2. **C++ Simulation Model**
   - `LogoSimulationModel.h/cpp`: Main simulation model
   - Integration with `MultiThreadedSimulationEngine`
   - Agent factory pattern for Python integration

3. **Python Bindings**
   - `bindings_logo_cpp.cpp`: pybind11 bindings
   - `cpp_engine.py`: Python wrapper class
   - Automatic fallback to pure Python if C++ not available

4. **Documentation**
   - `CPP_ENGINE.md`: Comprehensive usage guide
   - `MULTITHREADING.md`: Threading architecture explanation
   - Build scripts and examples

### 🚧 In Progress

1. **Build System**
   - CMakeLists configuration
   - Build script (`build_cpp_engine.sh`)
   - **Status**: Created but needs testing

2. **Perception Model**
   - Spatial indexing in C++
   - Neighbor queries
   - Pheromone access
   - **Status**: Interface defined, implementation needed

3. **Reaction Model**
   - Logo-specific reactions in C++
   - Movement processing
   - Pheromone diffusion/evaporation
   - **Status**: Using existing `LogoDefaultReactionModel`, may need customization

### 📋 TODO

1. **Complete C++ Implementation**
   - [ ] Implement `LogoPerceptionModel` in C++
   - [ ] Add spatial indexing (grid-based or R-tree)
   - [ ] Implement pheromone grid in C++
   - [ ] Add mark system
   - [ ] Optimize influence processing

2. **Python-C++ Bridge**
   - [ ] Test Python agent callbacks from C++ threads
   - [ ] Handle GIL properly in callbacks
   - [ ] Add error handling and debugging
   - [ ] Profile performance

3. **Build and Test**
   - [ ] Test CMake configuration
   - [ ] Build on multiple platforms (macOS, Linux, Windows)
   - [ ] Create unit tests
   - [ ] Performance benchmarks

4. **Integration**
   - [ ] Update DSL to auto-detect and use C++ engine
   - [ ] Migrate existing examples to C++ engine
   - [ ] Update web interface to work with C++ engine
   - [ ] Add state extraction from C++ simulation

5. **Pure C++ Agents**
   - [ ] Implement common agents in C++ (Boids, Ants, etc.)
   - [ ] Expose to Python
   - [ ] Performance comparison

## File Structure

```
similar/
├── cpp/
│   ├── microkernel/          # Core SIMILAR engine
│   ├── extendedkernel/       # Extended agent framework
│   └── similar2logo/
│       ├── include/kernel/
│       │   ├── agents/
│       │   │   └── LogoAgent.h          ✅ Created
│       │   ├── model/
│       │   │   └── LogoSimulationModel.h ✅ Created
│       │   └── perception/
│       │       └── LogoPerceptionModel.h ⏳ TODO
│       ├── src/kernel/
│       │   ├── agents/
│       │   │   └── LogoAgent.cpp         ✅ Created
│       │   └── model/
│       │       └── LogoSimulationModel.cpp ✅ Created
│       └── python/
│           ├── bindings_logo_cpp.cpp     ✅ Created
│           └── CMakeLists_logo_cpp.txt   ✅ Created
├── python/
│   └── similar2logo/
│       ├── cpp_engine.py                 ✅ Created
│       ├── model.py                      ✅ Existing (Python fallback)
│       └── dsl/
│           └── __init__.py               ⏳ Needs update for C++ integration
├── docs/
│   ├── CPP_ENGINE.md                     ✅ Created
│   └── MULTITHREADING.md                 ✅ Created
└── build_cpp_engine.sh                   ✅ Created
```

## Next Steps (Priority Order)

### 1. Build and Test (Immediate)

```bash
# Test the build system
./build_cpp_engine.sh

# If successful, test import
python3 -c "from similar2logo.cpp_engine import use_cpp_engine; print(use_cpp_engine())"
```

**Expected Issues:**
- Missing dependencies in CMakeLists
- Header include paths
- Linking errors with microkernel/extendedkernel

**Fix Strategy:**
- Add all required source files to CMakeLists
- Verify include paths
- Link against existing microkernel/extendedkernel libraries

### 2. Implement Perception Model (High Priority)

Create `LogoPerceptionModel.cpp` that:
- Builds spatial index of agents
- Queries neighbors within radius
- Accesses pheromone values
- Packages data into `LogoPerceivedData`

### 3. Test Python-C++ Integration (High Priority)

Create test script:
```python
from similar2logo.cpp_engine import CppLogoSimulation
from similar2logo.dsl import Turtle

class TestAgent(Turtle):
    def decide(self, perception):
        print(f"Called from C++ thread! Position: {perception['position']}")
        return []

sim = CppLogoSimulation(width=10, height=10, num_threads=2)
sim.add_agents(TestAgent, count=5)
sim.run(steps=10)
```

### 4. Update DSL for Auto-Detection (Medium Priority)

Modify `dsl/__init__.py`:
```python
def _build_simulation(self):
    # Try C++ engine first
    if use_cpp_engine() and not self._force_python:
        return self._build_cpp_simulation()
    else:
        return self._build_python_simulation()
```

### 5. Performance Benchmarking (Medium Priority)

Create benchmark suite:
- 100, 1000, 10000 agents
- Simple vs complex decision logic
- With/without pheromones
- Compare Python vs C++ engine

### 6. Documentation and Examples (Low Priority)

- Update all examples to work with C++ engine
- Add performance tips
- Create migration guide

## Performance Goals

| Metric | Pure Python | Python + C++ Engine | Improvement |
|--------|-------------|---------------------|-------------|
| 1000 agents | 10 FPS | 150 FPS | **15x** |
| 10000 agents | 1 FPS | 30 FPS | **30x** |
| Decision time | 100ms | 5ms | **20x** |

## Success Criteria

1. ✅ C++ engine builds successfully
2. ✅ Python can import and use C++ engine
3. ✅ Python agents work with C++ multithreading
4. ✅ Performance improvement of 10x+ for 1000+ agents
5. ✅ Existing Python examples work with minimal changes
6. ✅ Web interface works with C++ engine

## Risk Mitigation

**Risk**: Python GIL causes bottleneck in callbacks
**Mitigation**: Implement critical agents purely in C++

**Risk**: Build complexity across platforms
**Mitigation**: Provide pre-built wheels for common platforms

**Risk**: Breaking existing Python code
**Mitigation**: Maintain Python fallback, gradual migration

## Timeline Estimate

- **Week 1**: Build system, basic integration, testing
- **Week 2**: Perception model, spatial indexing
- **Week 3**: Performance optimization, benchmarking
- **Week 4**: Documentation, examples, polish

## Conclusion

This architecture provides:
- ✅ True multithreaded performance (no GIL)
- ✅ Python-friendly API
- ✅ Gradual migration path
- ✅ Backward compatibility
- ✅ Best of both worlds: C++ performance + Python ease of use
