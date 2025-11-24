# Java vs C++ Implementation Audit

## Executive Summary

This document compares the Java (reference) and C++ implementations of SIMILAR2Logo to identify discrepancies and necessary corrections.

**Date**: 2025-11-21  
**Status**: 🔍 In Progress

---

## 1. Pheromone Class

### Status: ✅ **MATCHED**

| Aspect | Java | C++ | Match? |
|--------|------|-----|--------|
| Fields | identifier, diffusionCoef, evaporationCoef, defaultValue, minValue | Same | ✅ |
| Constructors | 3 overloaded constructors | Same (using delegating constructors) | ✅ |
| Validation | Checks for null/negative values | Same | ✅ |
| Getters | All fields have getters | Same | ✅ |
| equals() | Based on identifier | operator== based on identifier | ✅ |
| hashCode() | Based on identifier | std::hash specialization | ✅ |
| toString() | Returns identifier | Same | ✅ |

**Conclusion**: Pheromone class is correctly implemented in C++.

---

## 2. Influence Classes

### 2.1 ChangeDirection

#### Status: ❌ **MISMATCH - CRITICAL**

| Aspect | Java | C++ | Match? |
|--------|------|-----|--------|
| **Base Class** | `RegularInfluence` | `RegularInfluence` | ✅ |
| **Constructor Signature** | `(LevelIdentifier, SimulationTimeStamp, SimulationTimeStamp, double dd, TurtlePLSInLogo target)` | `(ILocalStateOfAgent, double deltaHeading)` | ❌ |
| **Fields** | `dd` (direction change), `target` (TurtlePLSInLogo) | `deltaHeading`, `absolute`, `targetHeading` | ❌ |
| **Category** | Static final String "change direction" | Not defined | ❌ |

**Issues**:
1. ❌ Constructor signature doesn't match Java - missing `timeLowerBound`, `timeUpperBound`, `levelIdentifier`
2. ❌ Missing `target` field (TurtlePLSInLogo reference)
3. ❌ Missing static `CATEGORY` constant
4. ❌ C++ has extra `absolute` and `targetHeading` fields not in Java
5. ❌ Getter method names don't match: `getDd()` vs `getDeltaHeading()`

**Required Changes**:
```cpp
class ChangeDirection : public microkernel::influences::RegularInfluence {
public:
    static constexpr const char* CATEGORY = "change direction";
    
    ChangeDirection(
        microkernel::LevelIdentifier levelIdentifier,
        microkernel::SimulationTimeStamp timeLowerBound,
        microkernel::SimulationTimeStamp timeUpperBound,
        double dd,
        std::shared_ptr<TurtlePLSInLogo> target
    );
    
    double getDd() const { return dd; }
    std::shared_ptr<TurtlePLSInLogo> getTarget() const { return target; }
    
private:
    double dd;
    std::shared_ptr<TurtlePLSInLogo> target;
};
```

### 2.2 ChangePosition

#### Status: ❌ **MISMATCH - CRITICAL**

Similar issues as ChangeDirection - needs to match Java signature.

### 2.3 ChangeSpeed

#### Status: ❌ **MISMATCH - CRITICAL**

Similar issues as ChangeDirection - needs to match Java signature.

### 2.4 Stop

#### Status: ❌ **MISMATCH - CRITICAL**

Similar issues as ChangeDirection - needs to match Java signature.

### 2.5 ChangeAcceleration

#### Status: ❌ **MISMATCH - CRITICAL**

Similar issues as ChangeDirection - needs to match Java signature.

### 2.6 DropMark

#### Status: ⚠️ **NEEDS VERIFICATION**

Need to check if implementation matches Java.

### 2.7 EmitPheromone

#### Status: ⚠️ **NEEDS VERIFICATION**

Need to check if implementation matches Java.

---

## 3. RegularInfluence Base Class

### Status: ❌ **MISMATCH - ROOT CAUSE**

The root cause of all influence mismatches is that the C++ `RegularInfluence` base class doesn't match the Java version.

#### Java RegularInfluence Constructor:
```java
public RegularInfluence(
    String category,
    LevelIdentifier levelIdentifier,
    SimulationTimeStamp timeLowerBound,
    SimulationTimeStamp timeUpperBound
)
```

#### C++ RegularInfluence Constructor (Current):
```cpp
RegularInfluence(
    std::string level,
    std::shared_ptr<ILocalStateOfAgent> agent
)
```

**Issues**:
1. ❌ Missing `category` parameter
2. ❌ Missing `timeLowerBound` parameter  
3. ❌ Missing `timeUpperBound` parameter
4. ❌ Has `agent` parameter not in Java version
5. ❌ `level` should be `LevelIdentifier` type, not string

**This is a microkernel issue that needs to be fixed first!**

---

## 4. Environment Classes

### 4.1 LogoEnvPLS

#### Status: ⚠️ **NEEDS DETAILED AUDIT**

Need to compare:
- Field structure
- Methods
- Pheromone management
- Mark management

### 4.2 Environment (C++ wrapper)

#### Status: ✅ **CUSTOM IMPLEMENTATION**

This is a C++-specific class for performance. Not a direct Java equivalent.

---

## 5. Agent Classes

### 5.1 TurtlePLSInLogo

#### Status: ⚠️ **EXISTS IN C++ BUT NEEDS VERIFICATION**

File exists: `cpp/similar2logo/include/kernel/model/environment/TurtlePLSInLogo.h`

Need to verify it matches Java version.

### 5.2 LogoAgent

#### Status: ⚠️ **NEEDS VERIFICATION**

---

## 6. Reaction/Level Classes

### 6.1 LogoDefaultReactionModel

#### Status: ⚠️ **NEEDS VERIFICATION**

This is critical - it processes all influences.

### 6.2 LogoSimulationLevelList

#### Status: ⚠️ **NEEDS VERIFICATION**

---

## 7. Tools/Utilities

### 7.1 MathUtil

#### Status: ⚠️ **NEEDS VERIFICATION**

### 7.2 Point2D

#### Status: ✅ **IMPLEMENTED**

Exists in C++ and is bound to Python.

### 7.3 FastMath

#### Status: ✅ **C++ ENHANCEMENT**

This is a C++-specific optimization, not in Java.

---

## Priority Action Items

### 🔴 **CRITICAL** (Blocking)

1. **Fix microkernel RegularInfluence constructor**
   - Add `category`, `timeLowerBound`, `timeUpperBound` parameters
   - Remove or make optional `agent` parameter
   - This affects ALL influence classes

2. **Fix all Influence classes** (after #1)
   - ChangeDirection
   - ChangePosition
   - ChangeSpeed
   - Stop
   - ChangeAcceleration
   - DropMark
   - EmitPheromone
   - RemoveMark
   - RemoveMarks

3. **Verify TurtlePLSInLogo**
   - Ensure it matches Java structure
   - Needed by influence classes

### 🟡 **HIGH** (Important)

4. **Audit LogoDefaultReactionModel**
   - This processes influences - must match Java logic

5. **Audit LogoEnvPLS**
   - Core environment state class

6. **Audit LogoAgent**
   - Core agent class

### 🟢 **MEDIUM** (Nice to have)

7. **Verify MathUtil**
8. **Verify LogoSimulationLevelList**
9. **Verify Mark and SituatedEntity classes**

---

## Testing Strategy

### Phase 1: Unit Tests (After Fixes)
- Test each influence class individually
- Test RegularInfluence base class
- Test Pheromone class (already passing)

### Phase 2: Integration Tests
- Test influence processing in reaction model
- Test agent decision → influence → reaction flow

### Phase 3: Example Validation
- Port all Jython examples
- Compare behavior with Java/Jython versions

---

## Recommendations

1. **Fix microkernel first** - This is the root cause of most issues
2. **Use Java unit tests as reference** - Port them to C++ GoogleTest
3. **Maintain API compatibility** - Keep Python bindings working during refactoring
4. **Document differences** - Some C++ optimizations are intentional

---

## Files Requiring Changes

### Microkernel
- `cpp/microkernel/include/influences/RegularInfluence.h`
- `cpp/microkernel/src/influences/RegularInfluence.cpp`

### Similar2Logo Influences
- `cpp/similar2logo/include/kernel/influences/ChangeDirection.h`
- `cpp/similar2logo/include/kernel/influences/ChangePosition.h`
- `cpp/similar2logo/include/kernel/influences/ChangeSpeed.h`
- `cpp/similar2logo/include/kernel/influences/Stop.h`
- `cpp/similar2logo/include/kernel/influences/ChangeAcceleration.h`
- `cpp/similar2logo/include/kernel/influences/DropMark.h`
- `cpp/similar2logo/include/kernel/influences/EmitPheromone.h`
- `cpp/similar2logo/include/kernel/influences/RemoveMark.h`
- `cpp/similar2logo/include/kernel/influences/RemoveMarks.h`

### Python Bindings
- `cpp/similar2logo/python/bindings_logo_cpp.cpp` - Update influence bindings

---

**Next Steps**: 
1. Review microkernel RegularInfluence implementation
2. Create corrected influence class templates
3. Generate unit tests based on Java tests
4. Port Jython examples

