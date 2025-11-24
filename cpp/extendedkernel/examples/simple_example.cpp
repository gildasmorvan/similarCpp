/**
 * SIMILAR C++ - Simple Example
 *
 * This example demonstrates how to use the Extended Kernel to create
 * a simple multi-agent simulation with concrete .cpp implementations.
 */

#include "../include/agents/ExtendedAgent.h"
#include "../include/agents/IAgtDecisionModel.h"
#include "../include/agents/IAgtGlobalStateRevisionModel.h"
#include "../include/agents/IAgtPerceptionModel.h"
#include <iostream>
#include <memory>

using namespace fr::univ_artois::lgi2a::similar;

// ===== Example Global State =====
class SimpleGlobalState : public microkernel::agents::IGlobalState {
public:
  int energy = 100;
  std::string mood = "happy";

  virtual ~SimpleGlobalState() = default;
};

// ===== Example Perception Model =====
class SimplePerceptionModel
    : public extendedkernel::agents::IAgtPerceptionModel {
private:
  microkernel::LevelIdentifier level;

public:
  explicit SimplePerceptionModel(const microkernel::LevelIdentifier &level)
      : level(level) {}

  microkernel::LevelIdentifier getLevel() const override { return level; }

  std::shared_ptr<microkernel::agents::IPerceivedData> perceive(
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      const std::map<microkernel::LevelIdentifier,
                     std::shared_ptr<microkernel::agents::ILocalStateOfAgent>>
          &publicLocalStates,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent>
          privateLocalState,
      std::shared_ptr<microkernel::dynamicstate::IPublicDynamicStateMap>
          dynamicStates) override {

    std::cout << "  [Perception] Agent perceiving environment at time "
              << timeLowerBound.getIdentifier() << std::endl;
    return nullptr; // Simplified for example
  }
};

// ===== Example Decision Model =====
class SimpleDecisionModel : public extendedkernel::agents::IAgtDecisionModel {
private:
  microkernel::LevelIdentifier level;

public:
  explicit SimpleDecisionModel(const microkernel::LevelIdentifier &level)
      : level(level) {}

  microkernel::LevelIdentifier getLevel() const override { return level; }

  void decide(
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      std::shared_ptr<microkernel::agents::IGlobalState> globalState,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent> publicLocalState,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent>
          privateLocalState,
      std::shared_ptr<microkernel::agents::IPerceivedData> perceivedData,
      std::shared_ptr<microkernel::influences::InfluencesMap>
          producedInfluences) override {

    auto simpleState =
        std::dynamic_pointer_cast<SimpleGlobalState>(globalState);
    if (simpleState) {
      std::cout << "  [Decision] Agent deciding action. Energy: "
                << simpleState->energy << ", Mood: " << simpleState->mood
                << std::endl;
      simpleState->energy -= 5; // Consume energy
    }
  }
};

// ===== Example Global State Revision Model =====
class SimpleGlobalStateRevisionModel
    : public extendedkernel::agents::IAgtGlobalStateRevisionModel {
public:
  void reviseGlobalState(
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      const std::map<microkernel::LevelIdentifier,
                     std::shared_ptr<microkernel::agents::IPerceivedData>>
          &perceivedData,
      std::shared_ptr<microkernel::agents::IGlobalState> globalState) override {

    auto simpleState =
        std::dynamic_pointer_cast<SimpleGlobalState>(globalState);
    if (simpleState) {
      std::cout << "  [Revision] Updating global state. Current energy: "
                << simpleState->energy << std::endl;
      if (simpleState->energy < 50) {
        simpleState->mood = "tired";
      }
    }
  }
};

// ===== Main Example =====
int main() {
  std::cout << "=== SIMILAR C++ Extended Kernel Example ===" << std::endl;
  std::cout << std::endl;

  // Create agent category
  microkernel::AgentCategory robotCategory("Robot");
  std::cout << "1. Created agent category: " << robotCategory.toString()
            << std::endl;

  // Create an extended agent
  auto robot =
      std::make_shared<extendedkernel::agents::ExtendedAgent>(robotCategory);
  std::cout << "2. Created extended agent" << std::endl;

  // Initialize global state
  auto globalState = std::make_shared<SimpleGlobalState>();
  robot->initializeGlobalState(globalState);
  std::cout << "3. Initialized global state (Energy: " << globalState->energy
            << ")" << std::endl;

  // Create level
  microkernel::LevelIdentifier mainLevel("MainLevel");
  std::cout << "4. Created level: " << mainLevel.toString() << std::endl;

  // Create and specify behavior models
  auto perceptionModel = std::make_shared<SimplePerceptionModel>(mainLevel);
  auto decisionModel = std::make_shared<SimpleDecisionModel>(mainLevel);
  auto revisionModel = std::make_shared<SimpleGlobalStateRevisionModel>();

  robot->specifyGlobalStateRevisionModel(revisionModel);
  robot->specifyBehaviorForLevel(mainLevel, perceptionModel, decisionModel);
  std::cout << "5. Specified agent behavior models" << std::endl;
  std::cout << std::endl;

  // Simulate a few time steps
  std::cout << "=== Running Simulation ===" << std::endl;
  for (int t = 0; t < 3; t++) {
    std::cout << "\n--- Time Step " << t << " ---" << std::endl;

    microkernel::SimulationTimeStamp timeStamp(t);
    microkernel::SimulationTimeStamp nextTimeStamp(t + 1);

    // Perception phase
    std::map<microkernel::LevelIdentifier,
             std::shared_ptr<microkernel::agents::ILocalStateOfAgent>>
        emptyPublicStates;
    robot->perceive(mainLevel, timeStamp, nextTimeStamp, emptyPublicStates,
                    nullptr, nullptr);

    // Global state revision phase
    std::map<microkernel::LevelIdentifier,
             std::shared_ptr<microkernel::agents::IPerceivedData>>
        emptyPerceivedData;
    robot->reviseGlobalState(timeStamp, nextTimeStamp, emptyPerceivedData,
                             globalState);

    // Decision phase
    auto influences =
        std::make_shared<microkernel::influences::InfluencesMap>();
    robot->decide(mainLevel, timeStamp, nextTimeStamp, globalState, nullptr,
                  nullptr, nullptr, influences);
  }

  std::cout << "\n=== Simulation Complete ===" << std::endl;
  std::cout << "Final agent state - Energy: " << globalState->energy
            << ", Mood: " << globalState->mood << std::endl;

  return 0;
}
