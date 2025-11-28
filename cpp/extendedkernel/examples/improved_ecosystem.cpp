/**
 * SIMILAR C++ - Improved Predator-Prey Example
 *
 * This example demonstrates the use of extendedlibs utilities:
 * - AbstractAgtPerceptionModel and AbstractAgtDecisionModel base classes
 * - PeriodicTimeModel for regular time steps
 * - Cleaner, more maintainable code structure
 */

#include "../include/agents/ExtendedAgent.h"
#include "../include/agents/IAgtGlobalStateRevisionModel.h"
#include "../include/libs/abstractimpl/AbstractAgtDecisionModel.h"
#include "../include/libs/abstractimpl/AbstractAgtPerceptionModel.h"
#include "../include/libs/timemodel/PeriodicTimeModel.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

using namespace fr::univ_artois::lgi2a::similar;

// Random number generator
std::random_device rd;
std::mt19937 gen(rd());

// ===== Position Helper =====
struct Position {
  double x, y;
  Position(double x = 0, double y = 0) : x(x), y(y) {}

  double distanceTo(const Position &other) const {
    double dx = x - other.x;
    double dy = y - other.y;
    return std::sqrt(dx * dx + dy * dy);
  }

  Position moveTowards(const Position &target, double speed) const {
    double dx = target.x - x;
    double dy = target.y - y;
    double dist = std::sqrt(dx * dx + dy * dy);
    if (dist < 0.001)
      return *this;
    return Position(x + (dx / dist) * speed, y + (dy / dist) * speed);
  }

  Position randomMove(double speed) const {
    std::uniform_real_distribution<> angle(0, 2 * M_PI);
    double theta = angle(gen);
    return Position(x + speed * std::cos(theta), y + speed * std::sin(theta));
  }
};

// ===== Agent Global State =====
class AnimalGlobalState : public microkernel::agents::IGlobalState {
public:
  double energy;
  Position position;
  int age;
  bool isAlive;

  AnimalGlobalState(double initialEnergy, const Position &pos)
      : energy(initialEnergy), position(pos), age(0), isAlive(true) {}

  virtual ~AnimalGlobalState() = default;

  std::shared_ptr<microkernel::agents::IGlobalState> clone() const override {
    return std::make_shared<AnimalGlobalState>(*this);
  }
};

// ===== Improved Perception Model using AbstractAgtPerceptionModel =====
class AnimalPerceptionModel
    : public extendedkernel::libs::AbstractAgtPerceptionModel {
private:
  double perceptionRadius;

public:
  AnimalPerceptionModel(const microkernel::LevelIdentifier &level,
                        double radius)
      : AbstractAgtPerceptionModel(level), perceptionRadius(radius) {}

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

    // Simplified perception - returns nullptr for this example
    return nullptr;
  }
};

// ===== Improved Prey Decision Model using AbstractAgtDecisionModel =====
class PreyDecisionModel
    : public extendedkernel::libs::AbstractAgtDecisionModel {
private:
  double moveSpeed;

public:
  PreyDecisionModel(const microkernel::LevelIdentifier &level, double speed)
      : AbstractAgtDecisionModel(level), moveSpeed(speed) {}

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

    auto state = std::dynamic_pointer_cast<AnimalGlobalState>(globalState);
    if (!state || !state->isAlive)
      return;

    // Graze and move
    state->position = state->position.randomMove(moveSpeed);
    state->energy += 8; // Grazing gains energy
    state->energy -= 1; // Living costs energy
  }
};

// ===== Improved Predator Decision Model using AbstractAgtDecisionModel =====
class PredatorDecisionModel
    : public extendedkernel::libs::AbstractAgtDecisionModel {
private:
  double moveSpeed;

public:
  PredatorDecisionModel(const microkernel::LevelIdentifier &level, double speed)
      : AbstractAgtDecisionModel(level), moveSpeed(speed) {}

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

    auto state = std::dynamic_pointer_cast<AnimalGlobalState>(globalState);
    if (!state || !state->isAlive)
      return;

    // Hunt (simplified - just wander and lose energy)
    state->position = state->position.randomMove(moveSpeed);
    state->energy -= 3; // Hunting costs more energy
  }
};

// ===== Global State Revision Model =====
class AnimalGlobalStateRevisionModel
    : public extendedkernel::agents::IAgtGlobalStateRevisionModel {
private:
  std::string agentType;

public:
  AnimalGlobalStateRevisionModel(const std::string &type) : agentType(type) {}

  void reviseGlobalState(
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      const std::map<microkernel::LevelIdentifier,
                     std::shared_ptr<microkernel::agents::IPerceivedData>>
          &perceivedData,
      std::shared_ptr<microkernel::agents::IGlobalState> globalState) override {

    auto state = std::dynamic_pointer_cast<AnimalGlobalState>(globalState);
    if (!state)
      return;

    state->age++;

    // Check death conditions
    if (state->energy <= 0) {
      state->isAlive = false;
      std::cout << "    💀 " << agentType << " died (age: " << state->age << ")"
                << std::endl;
    }

    // Cap energy
    if (state->energy > 100)
      state->energy = 100;
  }
};

// ===== Helper Functions =====
std::shared_ptr<extendedkernel::agents::ExtendedAgent>
createAnimal(const std::string &type, const microkernel::LevelIdentifier &level,
             const Position &pos, double initialEnergy, double moveSpeed) {

  microkernel::AgentCategory category(type);
  auto animal =
      std::make_shared<extendedkernel::agents::ExtendedAgent>(category);

  // Initialize global state
  auto globalState = std::make_shared<AnimalGlobalState>(initialEnergy, pos);
  animal->initializeGlobalState(globalState);

  // Specify behavior using the abstract base classes
  auto perceptionModel = std::make_shared<AnimalPerceptionModel>(level, 15.0);
  std::shared_ptr<extendedkernel::libs::AbstractAgtDecisionModel> decisionModel;

  if (type == "Prey") {
    decisionModel = std::make_shared<PreyDecisionModel>(level, moveSpeed);
  } else {
    decisionModel = std::make_shared<PredatorDecisionModel>(level, moveSpeed);
  }

  auto revisionModel = std::make_shared<AnimalGlobalStateRevisionModel>(type);

  animal->specifyGlobalStateRevisionModel(revisionModel);
  animal->specifyBehaviorForLevel(level, perceptionModel, decisionModel);

  return animal;
}

// ===== Main Simulation =====
int main() {
  std::cout << "╔═══════════════════════════════════════════════════════════╗"
            << std::endl;
  std::cout << "║  SIMILAR C++ - Improved Ecosystem with ExtendedLibs      ║"
            << std::endl;
  std::cout << "╚═══════════════════════════════════════════════════════════╝"
            << std::endl;
  std::cout << std::endl;

  // Create level with periodic time model
  microkernel::LevelIdentifier ecosystem("Ecosystem");
  microkernel::SimulationTimeStamp initialTime(0);

  // Use PeriodicTimeModel from extendedlibs
  auto timeModel = std::make_shared<extendedkernel::libs::PeriodicTimeModel>(
      1, // period: advance by 1 each step
      0, // phase shift: no offset
      initialTime);

  std::cout << "⏰ Time Model: Periodic (period=" << timeModel->getPeriod()
            << ", phase=" << timeModel->getPhaseShift() << ")" << std::endl;
  std::cout << std::endl;

  // Create initial population
  std::vector<std::shared_ptr<extendedkernel::agents::ExtendedAgent>> agents;

  std::cout << "🌱 Initializing ecosystem..." << std::endl;

  std::uniform_real_distribution<> posDist(0, 100);

  // Create 6 prey
  for (int i = 0; i < 6; i++) {
    Position pos(posDist(gen), posDist(gen));
    agents.push_back(createAnimal("Prey", ecosystem, pos, 60.0, 2.0));
  }
  std::cout << "  Created 6 prey" << std::endl;

  // Create 3 predators
  for (int i = 0; i < 3; i++) {
    Position pos(posDist(gen), posDist(gen));
    agents.push_back(createAnimal("Predator", ecosystem, pos, 80.0, 3.0));
  }
  std::cout << "  Created 3 predators" << std::endl;
  std::cout << std::endl;

  // Run simulation
  const int NUM_STEPS = 15;
  std::cout << "🔄 Running simulation for " << NUM_STEPS << " time steps..."
            << std::endl;
  std::cout << std::endl;

  microkernel::SimulationTimeStamp currentTime = initialTime;

  for (int t = 0; t < NUM_STEPS; t++) {
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
              << std::endl;
    std::cout << "⏰ Time Step " << currentTime.getIdentifier() << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
              << std::endl;

    microkernel::SimulationTimeStamp nextTime =
        timeModel->getNextTime(currentTime);

    int preyCount = 0, predatorCount = 0;
    double totalPreyEnergy = 0, totalPredatorEnergy = 0;

    // Process each agent
    for (auto &agent : agents) {
      auto globalState =
          std::dynamic_pointer_cast<AnimalGlobalState>(agent->getGlobalState());
      if (!globalState || !globalState->isAlive)
        continue;

      // Global state revision
      std::map<microkernel::LevelIdentifier,
               std::shared_ptr<microkernel::agents::IPerceivedData>>
          emptyPerceived;
      agent->reviseGlobalState(currentTime, nextTime, emptyPerceived,
                               globalState);

      // Decision
      if (globalState->isAlive) {
        auto influences =
            std::make_shared<microkernel::influences::InfluencesMap>();
        agent->decide(ecosystem, currentTime, nextTime, globalState, nullptr,
                      nullptr, nullptr, influences);

        // Count population
        if (agent->getCategory().toString() == "Prey") {
          preyCount++;
          totalPreyEnergy += globalState->energy;
        } else {
          predatorCount++;
          totalPredatorEnergy += globalState->energy;
        }
      }
    }

    // Display statistics
    std::cout << std::endl;
    std::cout << "📊 Population:" << std::endl;
    std::cout << "  🐰 Prey:      " << preyCount << " agents";
    if (preyCount > 0) {
      std::cout << " (avg energy: " << std::fixed << std::setprecision(1)
                << (totalPreyEnergy / preyCount) << ")";
    }
    std::cout << std::endl;

    std::cout << "  🦁 Predators: " << predatorCount << " agents";
    if (predatorCount > 0) {
      std::cout << " (avg energy: " << std::fixed << std::setprecision(1)
                << (totalPredatorEnergy / predatorCount) << ")";
    }
    std::cout << std::endl;
    std::cout << std::endl;

    // Check extinction
    if (preyCount == 0 && predatorCount == 0) {
      std::cout << "⚠️  All agents have died. Simulation ending." << std::endl;
      break;
    }

    currentTime = nextTime;
  }

  std::cout << "╔═══════════════════════════════════════════════════════════╗"
            << std::endl;
  std::cout << "║              Simulation Complete                          ║"
            << std::endl;
  std::cout << "╚═══════════════════════════════════════════════════════════╝"
            << std::endl;

  return 0;
}
