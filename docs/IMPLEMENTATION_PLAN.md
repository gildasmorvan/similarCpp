# Implementation Plan: Java-C++ Alignment & Testing

## Overview
Fix C++ implementations to match Java reference and create comprehensive unit tests.

## Phase 1: Fix Influence Classes ✅ READY TO START

### 1.1 Update ChangeDirection
**File**: `cpp/similar2logo/include/kernel/influences/ChangeDirection.h`

**Changes Needed**:
- Match Java constructor signature
- Add `CATEGORY` constant
- Add `dd` and `target` fields
- Remove `absolute` and `targetHeading` fields
- Update getters to match Java

**Java Reference**:
```java
public class ChangeDirection extends RegularInfluence {
    public static final String CATEGORY = "change direction";
    private final double dd;
    private final TurtlePLSInLogo target;
    
    public ChangeDirection(
        LevelIdentifier levelIdentifier,
        SimulationTimeStamp timeLowerBound,
        SimulationTimeStamp timeUpperBound,
        double dd,
        TurtlePLSInLogo target
    )
}
```

### 1.2 Update ChangePosition
Similar changes as ChangeDirection

### 1.3 Update ChangeSpeed
Similar changes as ChangeDirection

### 1.4 Update Stop
Similar changes as ChangeDirection

### 1.5 Update ChangeAcceleration
Similar changes as ChangeDirection

### 1.6 Verify DropMark, EmitPheromone, RemoveMark, RemoveMarks
Check against Java and update if needed

## Phase 2: Create Unit Tests 🎯 PRIMARY GOAL

### 2.1 Find Java Unit Tests
**Location**: `similar2logo-examples/src/main/test/`

### 2.2 Create C++ Test Framework
**File**: `cpp/similar2logo/tests/CMakeLists.txt` (already created)

### 2.3 Port Java Tests to C++

#### Test Categories:
1. **Pheromone Tests**
   - Construction
   - Validation
   - Equality/hashing
   
2. **Influence Tests**
   - ChangeDirection creation
   - ChangePosition creation
   - ChangeSpeed creation
   - Stop creation
   - ChangeAcceleration creation
   - DropMark creation
   - EmitPheromone creation
   
3. **Environment Tests**
   - Pheromone management
   - Mark management
   - Distance calculations
   - Direction calculations
   
4. **Agent Tests**
   - TurtlePLSInLogo creation
   - State management
   
5. **Reaction Tests**
   - Influence processing
   - State updates

### 2.4 Test File Structure
```
cpp/similar2logo/tests/
├── CMakeLists.txt
├── test_pheromone.cpp
├── test_influences.cpp
├── test_environment.cpp
├── test_agents.cpp
├── test_reaction.cpp
└── test_integration.cpp
```

## Phase 3: Port Jython Examples 📚 COMPREHENSIVE GOAL

### 3.1 Identify All Jython Examples
**Location**: `similar2logo-examples/src/main/python/fr/univ_artois/lgi2a/similar2logo/examples/`

#### Examples Found:
1. **boids/** - ✅ Already ported (corrected)
2. **passive/** - Passive turtles
3. **multiturmite/** - Multi-turmite simulation
4. **segregation/** - Schelling segregation
5. **predation/** - Predator-prey
6. **fire/** - Forest fire
7. **diffusion/** - Diffusion processes
8. **wator/** - Wa-Tor world

### 3.2 Porting Strategy

For each example:
1. **Analyze** Jython implementation
2. **Create** Python DSL version
3. **Match** algorithm exactly
4. **Test** behavior matches
5. **Document** differences

### 3.3 Example Template
```python
"""
[Example Name] - DSL Implementation

This implementation matches the Jython version from:
similar2logo-examples/src/main/python/.../[ExampleName].py

Key differences:
- [List any intentional differences]

Parameters match Jython defaults.
"""

from similar2logo.dsl import Simulation, Turtle
from similar2logo.fastmath import ...
from dataclasses import dataclass

@dataclass
class [Example]Params:
    # Match Jython parameters exactly
    ...

class [Example]Agent(Turtle):
    def decide(self, perception):
        # Match Jython algorithm exactly
        ...

def main():
    ...
```

## Phase 4: Validation & Documentation

### 4.1 Create Comparison Tests
- Run Java version
- Run Python DSL version
- Compare outputs

### 4.2 Performance Benchmarks
- Measure Python vs C++ backend
- Document speedups

### 4.3 Documentation
- Update README with all examples
- Create example gallery
- Document API differences

## Execution Order

### Week 1: Fixes & Core Tests
1. ✅ Fix ChangeDirection (Day 1)
2. ✅ Fix ChangePosition (Day 1)
3. ✅ Fix ChangeSpeed (Day 1)
4. ✅ Fix Stop (Day 1)
5. ✅ Fix ChangeAcceleration (Day 1)
6. ✅ Create influence unit tests (Day 2)
7. ✅ Create pheromone unit tests (Day 2)
8. ✅ Create environment unit tests (Day 3)

### Week 2: Advanced Tests & Examples
9. ✅ Create agent tests (Day 4)
10. ✅ Create reaction tests (Day 5)
11. ✅ Port passive turtle example (Day 6)
12. ✅ Port multiturmite example (Day 7)

### Week 3: Remaining Examples
13. ✅ Port segregation example
14. ✅ Port predation example
15. ✅ Port fire example
16. ✅ Port diffusion example
17. ✅ Port wator example

## Success Criteria

### Must Have ✅
- [ ] All influence classes match Java signatures
- [ ] All unit tests pass
- [ ] At least 3 Jython examples ported and verified

### Should Have 🎯
- [ ] All Jython examples ported
- [ ] Performance benchmarks documented
- [ ] Integration tests passing

### Nice to Have 🌟
- [ ] Visual comparison tool
- [ ] Automated regression testing
- [ ] CI/CD pipeline

## Files to Create/Modify

### To Fix (Phase 1)
- `cpp/similar2logo/include/kernel/influences/ChangeDirection.h`
- `cpp/similar2logo/include/kernel/influences/ChangePosition.h`
- `cpp/similar2logo/include/kernel/influences/ChangeSpeed.h`
- `cpp/similar2logo/include/kernel/influences/Stop.h`
- `cpp/similar2logo/include/kernel/influences/ChangeAcceleration.h`

### To Create (Phase 2)
- `cpp/similar2logo/tests/test_pheromone.cpp`
- `cpp/similar2logo/tests/test_influences.cpp`
- `cpp/similar2logo/tests/test_environment.cpp`
- `cpp/similar2logo/tests/test_agents.cpp`
- `cpp/similar2logo/tests/test_reaction.cpp`
- `cpp/similar2logo/tests/test_integration.cpp`

### To Create (Phase 3)
- `examples/python/passive_turtle_dsl.py`
- `examples/python/multiturmite_dsl.py`
- `examples/python/segregation_dsl.py`
- `examples/python/predation_dsl.py`
- `examples/python/fire_dsl.py`
- `examples/python/diffusion_dsl.py`
- `examples/python/wator_dsl.py`

## Next Immediate Actions

1. **START**: Fix ChangeDirection.h
2. **THEN**: Fix remaining influence classes
3. **THEN**: Create unit tests
4. **THEN**: Port examples

---

**Status**: 📋 Plan Complete - Ready to Execute  
**Priority**: Phase 1 (Fixes) → Phase 2 (Tests) → Phase 3 (Examples)
