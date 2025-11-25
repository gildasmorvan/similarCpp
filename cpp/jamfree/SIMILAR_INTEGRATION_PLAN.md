# JamFree C++ Refactoring Plan: SIMILAR Extended Kernel Integration

## Current Status

The current C++ implementation of JamFree is a **standalone traffic simulation** that does NOT use the SIMILAR extended kernel architecture (influences/reaction, multi-level approach). This is inconsistent with the Java implementation.

### Current C++ Architecture
```
Vehicle (standalone class)
  ├─ IDM (car-following)
  ├─ MOBIL (lane-changing)
  └─ Direct state updates
```

### Java Architecture (Target)
```
VehicleAgent (SIMILAR Agent)
  ├─ Perception Model → VehicleAgtPerceptMicro
  ├─ Decision Model → VehicleAgtDecisionMicro
  │   └─ Decision Sub-Models (DMS)
  │       ├─ Forward Acceleration DMS
  │       ├─ Lane Change DMS
  │       └─ Collision Avoidance DMS
  └─ Influences → InfluencesMap
      ├─ ChangeAcceleration
      ├─ ChangeLane
      └─ etc.
```

## Required Changes

### 1. Agent-Based Architecture

#### Create Vehicle Agent Class
```cpp
// jamfree/kernel/include/agents/VehicleAgent.h
class VehicleAgent : public similar::extendedkernel::Agent {
public:
    VehicleAgent(const std::string& id);
    
    // Multi-level support
    void addLevel(LevelIdentifier level);
    
    // State management
    PublicLocalState& getPublicLocalState(LevelIdentifier level);
    PrivateLocalState& getPrivateLocalState(LevelIdentifier level);
    
private:
    std::map<LevelIdentifier, std::shared_ptr<PublicLocalState>> m_public_states;
    std::map<LevelIdentifier, std::shared_ptr<PrivateLocalState>> m_private_states;
};
```

#### Vehicle Public Local State (Microscopic Level)
```cpp
// jamfree/microscopic/include/agents/VehiclePublicLocalStateMicro.h
class VehiclePublicLocalStateMicro : public similar::microkernel::PublicLocalState {
public:
    // Observable state
    Point2D getPosition() const;
    double getSpeed() const;
    double getHeading() const;
    Lane* getCurrentLane() const;
    double getLanePosition() const;
    
    // Setters (for reaction model)
    void setPosition(const Point2D& pos);
    void setSpeed(double speed);
    // ...
    
private:
    Point2D m_position;
    double m_speed;
    double m_heading;
    Lane* m_current_lane;
    double m_lane_position;
};
```

#### Vehicle Private Local State (Hidden State)
```cpp
// jamfree/microscopic/include/agents/VehiclePrivateLocalStateMicro.h
class VehiclePrivateLocalStateMicro : public similar::microkernel::PrivateLocalState {
public:
    // Hidden state (decision-making parameters)
    double getDesiredSpeed() const;
    double getTimeHeadway() const;
    double getMinGap() const;
    double getPoliteness() const;
    
    // IDM parameters
    IDMParameters& getIDMParameters();
    
    // MOBIL parameters
    MOBILParameters& getMOBILParameters();
    
private:
    IDMParameters m_idm_params;
    MOBILParameters m_mobil_params;
    // Route information
    std::vector<std::string> m_route;
    int m_current_route_index;
};
```

### 2. Perception Model

```cpp
// jamfree/microscopic/include/perceptionmodel/VehiclePerceptionModelMicro.h
class VehiclePerceptionModelMicro : public similar::extendedkernel::AbstractPerceptionModel {
public:
    VehiclePerceptionModelMicro();
    
    void perceive(
        SimulationTimeStamp timeLowerBound,
        SimulationTimeStamp timeUpperBound,
        const GlobalState& globalState,
        const PublicLocalState& publicLocalState,
        const PrivateLocalState& privateLocalState,
        PerceivedData& perceivedData
    ) override;
    
private:
    // Sub-models
    void perceiveLeader(/* ... */);
    void perceiveFollower(/* ... */);
    void perceiveAdjacentLanes(/* ... */);
    void perceiveTrafficSigns(/* ... */);
};
```

#### Perceived Data
```cpp
// jamfree/microscopic/include/perceptionmodel/VehiclePerceivedDataMicro.h
class VehiclePerceivedDataMicro : public similar::microkernel::PerceivedData {
public:
    // Leader information
    Vehicle* getLeader() const;
    double getGapToLeader() const;
    
    // Adjacent lanes
    Lane* getLeftLane() const;
    Lane* getRightLane() const;
    Vehicle* getLeftLeader() const;
    Vehicle* getLeftFollower() const;
    Vehicle* getRightLeader() const;
    Vehicle* getRightFollower() const;
    
    // Traffic signs
    std::vector<TrafficSign*> getVisibleSigns() const;
    
private:
    Vehicle* m_leader;
    double m_gap_to_leader;
    Lane* m_left_lane;
    Lane* m_right_lane;
    // ...
};
```

### 3. Decision Model with Decision Sub-Models (DMS)

```cpp
// jamfree/microscopic/include/decisionmodel/VehicleDecisionModelMicro.h
class VehicleDecisionModelMicro : public similar::extendedkernel::AbstractDecisionModel {
public:
    VehicleDecisionModelMicro(IDecisionMicroSubmodel* rootDMS);
    
    void decide(
        SimulationTimeStamp timeLowerBound,
        SimulationTimeStamp timeUpperBound,
        const GlobalState& globalState,
        const PublicLocalState& publicLocalState,
        const PrivateLocalState& privateLocalState,
        const PerceivedData& perceivedData,
        InfluencesMap& producedInfluences
    ) override;
    
private:
    std::unique_ptr<IDecisionMicroSubmodel> m_root_dms;
};
```

#### Decision Sub-Models
```cpp
// jamfree/microscopic/include/decisionmodel/dms/IDecisionMicroSubmodel.h
class IDecisionMicroSubmodel {
public:
    virtual ~IDecisionMicroSubmodel() = default;
    
    virtual bool manageDecision(
        SimulationTimeStamp timeLowerBound,
        SimulationTimeStamp timeUpperBound,
        const VehiclePublicLocalStateMicro& publicState,
        const VehiclePrivateLocalStateMicro& privateState,
        const VehiclePerceivedDataMicro& perceivedData,
        const GlobalState& globalState,
        InfluencesMap& producedInfluences
    ) = 0;
};

// Forward Acceleration DMS (uses IDM)
class ForwardAccelerationDMS : public IDecisionMicroSubmodel {
public:
    bool manageDecision(/* ... */) override {
        // Calculate IDM acceleration
        double acc = calculateIDMAcceleration(/* ... */);
        
        // Create influence
        auto influence = std::make_shared<ChangeAcceleration>(
            timeLowerBound,
            timeUpperBound,
            acc
        );
        producedInfluences.add(influence);
        
        return true;
    }
};

// Lane Change DMS (uses MOBIL)
class LaneChangeDMS : public IDecisionMicroSubmodel {
public:
    bool manageDecision(/* ... */) override {
        // Check if lane change possible
        if (!perceivedData.getLeftLane() && !perceivedData.getRightLane()) {
            return false;  // Cannot handle
        }
        
        // Use MOBIL to decide
        Direction decision = evaluateLaneChange(/* ... */);
        
        if (decision != Direction::NONE) {
            auto influence = std::make_shared<ChangeLane>(
                timeLowerBound,
                timeUpperBound,
                decision
            );
            producedInfluences.add(influence);
        }
        
        return true;
    }
};

// Conjunction DMS (combines multiple DMS)
class ConjunctionDMS : public IDecisionMicroSubmodel {
public:
    void addSubModel(std::shared_ptr<IDecisionMicroSubmodel> dms) {
        m_submodels.push_back(dms);
    }
    
    bool manageDecision(/* ... */) override {
        bool handled = false;
        for (auto& dms : m_submodels) {
            if (dms->manageDecision(/* ... */)) {
                handled = true;
            }
        }
        return handled;
    }
    
private:
    std::vector<std::shared_ptr<IDecisionMicroSubmodel>> m_submodels;
};
```

### 4. Influences

```cpp
// jamfree/microscopic/include/influences/ChangeAcceleration.h
class ChangeAcceleration : public similar::extendedkernel::RegularInfluence {
public:
    ChangeAcceleration(
        SimulationTimeStamp timeLowerBound,
        SimulationTimeStamp timeUpperBound,
        double acceleration
    );
    
    double getAcceleration() const { return m_acceleration; }
    
private:
    double m_acceleration;
};

// jamfree/microscopic/include/influences/ChangeLane.h
class ChangeLane : public similar::extendedkernel::RegularInfluence {
public:
    enum class Direction { LEFT, RIGHT };
    
    ChangeLane(
        SimulationTimeStamp timeLowerBound,
        SimulationTimeStamp timeUpperBound,
        Direction direction
    );
    
    Direction getDirection() const { return m_direction; }
    
private:
    Direction m_direction;
};

// jamfree/microscopic/include/influences/ChangePosition.h
class ChangePosition : public similar::extendedkernel::RegularInfluence {
public:
    ChangePosition(
        SimulationTimeStamp timeLowerBound,
        SimulationTimeStamp timeUpperBound,
        const Point2D& newPosition
    );
    
    const Point2D& getNewPosition() const { return m_new_position; }
    
private:
    Point2D m_new_position;
};
```

### 5. Reaction Model

```cpp
// jamfree/microscopic/include/reactionmodel/MicroscopicReactionModel.h
class MicroscopicReactionModel : public similar::extendedkernel::AbstractReactionModel {
public:
    MicroscopicReactionModel();
    
    void react(
        SimulationTimeStamp timeLowerBound,
        SimulationTimeStamp timeUpperBound,
        ConsistentPublicLocalDynamicState& consistentState,
        const Set<IInfluence>& regularInfluencesOftransitoryStateDynamics,
        InfluencesMap& remainingInfluences
    ) override;
    
private:
    void applyChangeAcceleration(/* ... */);
    void applyChangeLane(/* ... */);
    void applyChangePosition(/* ... */);
    void updateVehiclePhysics(double dt);
};
```

### 6. Multi-Level Architecture

```cpp
// jamfree/kernel/include/levels/LevelIdentifiers.h
namespace jamfree {
namespace levels {

class LevelIdentifiers {
public:
    static const LevelIdentifier MICROSCOPIC;
    static const LevelIdentifier MACROSCOPIC;
    static const LevelIdentifier CONTROL;
};

} // namespace levels
} // namespace jamfree
```

### 7. Simulation Engine Integration

```cpp
// Use SIMILAR's simulation engine
#include <similar/extendedkernel/SimulationEngine.h>

// Create simulation
auto simulation = std::make_shared<similar::extendedkernel::Simulation>();

// Add levels
simulation->addLevel(LevelIdentifiers::MICROSCOPIC);
simulation->addLevel(LevelIdentifiers::MACROSCOPIC);
simulation->addLevel(LevelIdentifiers::CONTROL);

// Create vehicle agents
for (int i = 0; i < num_vehicles; ++i) {
    auto vehicle = std::make_shared<VehicleAgent>("vehicle_" + std::to_string(i));
    
    // Add to microscopic level
    vehicle->addLevel(LevelIdentifiers::MICROSCOPIC);
    
    // Set perception model
    vehicle->setPerceptionModel(
        LevelIdentifiers::MICROSCOPIC,
        std::make_shared<VehiclePerceptionModelMicro>()
    );
    
    // Set decision model with DMS architecture
    auto forwardAccDMS = std::make_shared<ForwardAccelerationDMS>();
    auto laneChangeDMS = std::make_shared<LaneChangeDMS>();
    auto conjunctionDMS = std::make_shared<ConjunctionDMS>();
    conjunctionDMS->addSubModel(forwardAccDMS);
    conjunctionDMS->addSubModel(laneChangeDMS);
    
    vehicle->setDecisionModel(
        LevelIdentifiers::MICROSCOPIC,
        std::make_shared<VehicleDecisionModelMicro>(conjunctionDMS.get())
    );
    
    simulation->addAgent(vehicle);
}

// Set reaction model for microscopic level
simulation->setReactionModel(
    LevelIdentifiers::MICROSCOPIC,
    std::make_shared<MicroscopicReactionModel>()
);

// Run simulation
simulation->run();
```

## Benefits of Refactoring

### 1. Consistency with Java Implementation
- Same architecture
- Same concepts (agents, influences, reaction)
- Easier to maintain both versions

### 2. Multi-Level Support
- Microscopic level
- Macroscopic level
- Control level
- Seamless transitions

### 3. Modularity
- Decision Sub-Models (DMS) are composable
- Easy to add new behaviors
- Clear separation of concerns

### 4. Extensibility
- New influence types
- New decision models
- New perception models
- Custom reaction logic

### 5. Debugging
- Clear influence trace
- Explicit decision process
- Reaction model validation

## Implementation Plan

### Phase 1: Core Infrastructure
1. Create agent base classes
2. Implement public/private local states
3. Create level identifiers
4. Basic simulation engine integration

### Phase 2: Perception
1. Implement perception model
2. Create perceived data structures
3. Add perception sub-models

### Phase 3: Decision
1. Create decision model base
2. Implement DMS interface
3. Port IDM to ForwardAccelerationDMS
4. Port MOBIL to LaneChangeDMS
5. Implement conjunction/subsumption DMS

### Phase 4: Influences
1. Define influence types
2. Implement ChangeAcceleration
3. Implement ChangeLane
4. Implement ChangePosition

### Phase 5: Reaction
1. Create reaction model
2. Implement influence application
3. Add physics updates
4. Validate state consistency

### Phase 6: Integration
1. Update simulation loop
2. Migrate existing code
3. Add tests
4. Update Python bindings

### Phase 7: Multi-Level
1. Add macroscopic level
2. Implement level transitions
3. Add control level
4. Hybrid simulation

## Migration Strategy

### Backward Compatibility
- Keep existing API temporarily
- Add adapter layer
- Gradual migration
- Deprecation warnings

### Testing
- Unit tests for each component
- Integration tests
- Comparison with Java implementation
- Performance benchmarks

## Timeline Estimate

- Phase 1-2: 2-3 weeks
- Phase 3-4: 2-3 weeks
- Phase 5-6: 2-3 weeks
- Phase 7: 1-2 weeks
- **Total**: 7-11 weeks

## Conclusion

This refactoring will align the C++ JamFree implementation with the Java version, using SIMILAR's extended kernel architecture. This provides:
- Better modularity
- Multi-level support
- Easier maintenance
- Consistent architecture across languages

The influence/reaction model provides a clean separation between decision-making and state updates, making the code more maintainable and extensible.

---

**Status**: Planning Phase
**Priority**: High (architectural alignment)
**Complexity**: High (major refactoring)
