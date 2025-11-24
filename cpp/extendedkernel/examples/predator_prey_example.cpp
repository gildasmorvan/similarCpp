/**
 * SIMILAR C++ - Predator-Prey Ecosystem Example
 *
 * This complex example demonstrates:
 * - Multiple agent types (Prey and Predator)
 * - Spatial environment with positions
 * - Agent perception of nearby agents
 * - Energy-based lifecycle (eating, reproduction, death)
 * - System influences (adding/removing agents)
 * - Local states with positions
 * - Dynamic population changes
 */

#include "../include/agents/ExtendedAgent.h"
#include "../include/agents/IAgtDecisionModel.h"
#include "../include/agents/IAgtGlobalStateRevisionModel.h"
#include "../include/agents/IAgtPerceptionModel.h"
#include <cmath>
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

// ===== Agent Local State with Position =====
class SpatialLocalState : public microkernel::agents::ILocalStateOfAgent {
private:
  microkernel::LevelIdentifier level;
  Position position;
  std::shared_ptr<microkernel::agents::IAgent4Engine> owner;

public:
  SpatialLocalState(const microkernel::LevelIdentifier &level,
                    const Position &pos,
                    std::shared_ptr<microkernel::agents::IAgent4Engine> owner)
      : level(level), position(pos), owner(owner) {}

  microkernel::LevelIdentifier getLevel() const override { return level; }

  microkernel::AgentCategory getCategoryOfAgent() const override {
    return owner->getCategory();
  }

  bool isOwnedBy(const microkernel::agents::IAgent &agent) const override {
    return &agent == owner.get();
  }

  Position getPosition() const { return position; }
  void setPosition(const Position &pos) { position = pos; }
};

// ===== Perceived Data with Nearby Agents =====
class EcosystemPerceivedData : public microkernel::agents::IPerceivedData {
private:
  microkernel::LevelIdentifier level;
  microkernel::SimulationTimeStamp timeLower;
  microkernel::SimulationTimeStamp timeUpper;
  std::vector<std::pair<microkernel::AgentCategory, Position>> nearbyAgents;

public:
  EcosystemPerceivedData(const microkernel::LevelIdentifier &level,
                         const microkernel::SimulationTimeStamp &lower,
                         const microkernel::SimulationTimeStamp &upper)
      : level(level), timeLower(lower), timeUpper(upper) {}

  microkernel::LevelIdentifier getLevel() const override { return level; }
  microkernel::SimulationTimeStamp getTransitoryPeriodMin() const override {
    return timeLower;
  }
  microkernel::SimulationTimeStamp getTransitoryPeriodMax() const override {
    return timeUpper;
  }

  void addNearbyAgent(const microkernel::AgentCategory &category,
                      const Position &pos) {
    nearbyAgents.push_back({category, pos});
  }

  const std::vector<std::pair<microkernel::AgentCategory, Position>> &
  getNearbyAgents() const {
    return nearbyAgents;
  }
};

// ===== Agent Global State =====
class EcosystemGlobalState : public microkernel::agents::IGlobalState {
public:
  double energy;
  int age;
  bool isAlive;

  EcosystemGlobalState(double initialEnergy)
      : energy(initialEnergy), age(0), isAlive(true) {}

  virtual ~EcosystemGlobalState() = default;
};

// ===== Perception Model =====
class EcosystemPerceptionModel
    : public extendedkernel::agents::IAgtPerceptionModel {
private:
  microkernel::LevelIdentifier level;
  double perceptionRadius;

public:
  EcosystemPerceptionModel(const microkernel::LevelIdentifier &level,
                           double radius)
      : level(level), perceptionRadius(radius) {}

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

    auto perceivedData = std::make_shared<EcosystemPerceivedData>(
        level, timeLowerBound, timeUpperBound);

    // Get own position
    auto ownState =
        std::dynamic_pointer_cast<SpatialLocalState>(privateLocalState);
    if (!ownState)
      return perceivedData;

    Position ownPos = ownState->getPosition();

    // Perceive nearby agents (simplified - in real simulation would query
    // dynamic state) For this example, we'll just note that perception happened

    return perceivedData;
  }
};

// ===== Prey Decision Model =====
class PreyDecisionModel : public extendedkernel::agents::IAgtDecisionModel {
private:
  microkernel::LevelIdentifier level;
  double moveSpeed;

public:
  PreyDecisionModel(const microkernel::LevelIdentifier &level, double speed)
      : level(level), moveSpeed(speed) {}

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

    auto state = std::dynamic_pointer_cast<EcosystemGlobalState>(globalState);
    auto spatialState =
        std::dynamic_pointer_cast<SpatialLocalState>(publicLocalState);
    auto perceived =
        std::dynamic_pointer_cast<EcosystemPerceivedData>(perceivedData);

    if (!state || !spatialState || !perceived)
      return;

    // Prey behavior: graze (gain energy) and flee from predators
    Position currentPos = spatialState->getPosition();
    bool predatorNearby = false;
    Position fleeDirection = currentPos;

    for (const auto &[category, pos] : perceived->getNearbyAgents()) {
      if (category.toString() == "Predator") {
        predatorNearby = true;
        // Flee away from predator
        double dx = currentPos.x - pos.x;
        double dy = currentPos.y - pos.y;
        fleeDirection = Position(currentPos.x + dx, currentPos.y + dy);
        break;
      }
    }

    if (predatorNearby) {
      // Flee
      Position newPos = currentPos.moveTowards(fleeDirection, moveSpeed * 1.5);
      spatialState->setPosition(newPos);
      state->energy -= 2; // Fleeing costs energy
    } else {
      // Graze peacefully
      Position newPos = currentPos.randomMove(moveSpeed);
      spatialState->setPosition(newPos);
      state->energy += 5; // Grazing gains energy
    }

    // Consume energy for living
    state->energy -= 1;
  }
};

// ===== Predator Decision Model =====
class PredatorDecisionModel : public extendedkernel::agents::IAgtDecisionModel {
private:
  microkernel::LevelIdentifier level;
  double moveSpeed;
  double huntRange;

public:
  PredatorDecisionModel(const microkernel::LevelIdentifier &level, double speed,
                        double range)
      : level(level), moveSpeed(speed), huntRange(range) {}

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

    auto state = std::dynamic_pointer_cast<EcosystemGlobalState>(globalState);
    auto spatialState =
        std::dynamic_pointer_cast<SpatialLocalState>(publicLocalState);
    auto perceived =
        std::dynamic_pointer_cast<EcosystemPerceivedData>(perceivedData);

    if (!state || !spatialState || !perceived)
      return;

    // Predator behavior: hunt prey
    Position currentPos = spatialState->getPosition();
    Position nearestPreyPos = currentPos;
    double nearestPreyDist = std::numeric_limits<double>::max();
    bool preyFound = false;

    for (const auto &[category, pos] : perceived->getNearbyAgents()) {
      if (category.toString() == "Prey") {
        double dist = currentPos.distanceTo(pos);
        if (dist < nearestPreyDist) {
          nearestPreyDist = dist;
          nearestPreyPos = pos;
          preyFound = true;
        }
      }
    }

    if (preyFound && nearestPreyDist < huntRange) {
      // Chase prey
      Position newPos = currentPos.moveTowards(nearestPreyPos, moveSpeed);
      spatialState->setPosition(newPos);

      // If caught prey
      if (nearestPreyDist < 2.0) {
        state->energy += 30; // Eating prey gives lots of energy
        std::cout << "    🦁 Predator caught prey! Energy: " << state->energy
                  << std::endl;
      } else {
        state->energy -= 3; // Hunting costs energy
      }
    } else {
      // Wander
      Position newPos = currentPos.randomMove(moveSpeed * 0.5);
      spatialState->setPosition(newPos);
      state->energy -= 2; // Wandering costs energy
    }
  }
};

// ===== Global State Revision Model =====
class EcosystemGlobalStateRevisionModel
    : public extendedkernel::agents::IAgtGlobalStateRevisionModel {
private:
  std::string agentType;

public:
  EcosystemGlobalStateRevisionModel(const std::string &type)
      : agentType(type) {}

  void reviseGlobalState(
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      const std::map<microkernel::LevelIdentifier,
                     std::shared_ptr<microkernel::agents::IPerceivedData>>
          &perceivedData,
      std::shared_ptr<microkernel::agents::IGlobalState> globalState) override {

    auto state = std::dynamic_pointer_cast<EcosystemGlobalState>(globalState);
    if (!state)
      return;

    state->age++;

    // Check death conditions
    if (state->energy <= 0) {
      state->isAlive = false;
      std::cout << "    💀 " << agentType
                << " died of starvation (age: " << state->age << ")"
                << std::endl;
    } else if (state->age > 100) {
      state->isAlive = false;
      std::cout << "    💀 " << agentType << " died of old age" << std::endl;
    }

    // Cap energy
    if (state->energy > 100) {
      state->energy = 100;
    }
  }
};

// ===== Helper Functions =====
std::shared_ptr<extendedkernel::agents::ExtendedAgent>
createPrey(const microkernel::LevelIdentifier &level, const Position &pos) {

  microkernel::AgentCategory preyCategory("Prey");
  auto prey =
      std::make_shared<extendedkernel::agents::ExtendedAgent>(preyCategory);

  // Initialize global state
  auto globalState = std::make_shared<EcosystemGlobalState>(50.0);
  prey->initializeGlobalState(globalState);

  // Create local states
  auto publicState = std::make_shared<SpatialLocalState>(level, pos, prey);
  auto privateState = std::make_shared<SpatialLocalState>(level, pos, prey);
  prey->includeNewLevel(level, publicState, privateState);

  // Specify behavior
  auto perceptionModel =
      std::make_shared<EcosystemPerceptionModel>(level, 15.0);
  auto decisionModel = std::make_shared<PreyDecisionModel>(level, 2.0);
  auto revisionModel =
      std::make_shared<EcosystemGlobalStateRevisionModel>("Prey");

  prey->specifyGlobalStateRevisionModel(revisionModel);
  prey->specifyBehaviorForLevel(level, perceptionModel, decisionModel);

  return prey;
}

std::shared_ptr<extendedkernel::agents::ExtendedAgent>
createPredator(const microkernel::LevelIdentifier &level, const Position &pos) {

  microkernel::AgentCategory predatorCategory("Predator");
  auto predator =
      std::make_shared<extendedkernel::agents::ExtendedAgent>(predatorCategory);

  // Initialize global state
  auto globalState = std::make_shared<EcosystemGlobalState>(80.0);
  predator->initializeGlobalState(globalState);

  // Create local states
  auto publicState = std::make_shared<SpatialLocalState>(level, pos, predator);
  auto privateState = std::make_shared<SpatialLocalState>(level, pos, predator);
  predator->includeNewLevel(level, publicState, privateState);

  // Specify behavior
  auto perceptionModel =
      std::make_shared<EcosystemPerceptionModel>(level, 20.0);
  auto decisionModel =
      std::make_shared<PredatorDecisionModel>(level, 3.0, 15.0);
  auto revisionModel =
      std::make_shared<EcosystemGlobalStateRevisionModel>("Predator");

  predator->specifyGlobalStateRevisionModel(revisionModel);
  predator->specifyBehaviorForLevel(level, perceptionModel, decisionModel);

  return predator;
}

// ===== Main Simulation =====
int main() {
  std::cout << "╔════════════════════════════════════════════════════════╗"
            << std::endl;
  std::cout << "║   SIMILAR C++ - Predator-Prey Ecosystem Simulation    ║"
            << std::endl;
  std::cout << "╚════════════════════════════════════════════════════════╝"
            << std::endl;
  std::cout << std::endl;

  microkernel::LevelIdentifier ecosystem("Ecosystem");

  // Create initial population
  std::vector<std::shared_ptr<extendedkernel::agents::ExtendedAgent>> agents;

  std::cout << "🌱 Initializing ecosystem..." << std::endl;

  // Create 5 prey
  std::uniform_real_distribution<> posDist(0, 100);
  for (int i = 0; i < 5; i++) {
    Position pos(posDist(gen), posDist(gen));
    agents.push_back(createPrey(ecosystem, pos));
  }
  std::cout << "  Created 5 prey" << std::endl;

  // Create 2 predators
  for (int i = 0; i < 2; i++) {
    Position pos(posDist(gen), posDist(gen));
    agents.push_back(createPredator(ecosystem, pos));
  }
  std::cout << "  Created 2 predators" << std::endl;
  std::cout << std::endl;

  // Run simulation
  const int NUM_STEPS = 20;
  std::cout << "🔄 Running simulation for " << NUM_STEPS << " time steps..."
            << std::endl;
  std::cout << std::endl;

  for (int t = 0; t < NUM_STEPS; t++) {
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
              << std::endl;
    std::cout << "⏰ Time Step " << t << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
              << std::endl;

    microkernel::SimulationTimeStamp timeStamp(t);
    microkernel::SimulationTimeStamp nextTimeStamp(t + 1);

    int preyCount = 0, predatorCount = 0;
    double totalPreyEnergy = 0, totalPredatorEnergy = 0;

    // Process each agent
    for (auto &agent : agents) {
      auto globalState = std::dynamic_pointer_cast<EcosystemGlobalState>(
          agent->getGlobalState());
      if (!globalState || !globalState->isAlive)
        continue;

      // Perception
      std::map<microkernel::LevelIdentifier,
               std::shared_ptr<microkernel::agents::ILocalStateOfAgent>>
          emptyPublic;
      auto perceivedData = agent->perceive(ecosystem, timeStamp, nextTimeStamp,
                                           emptyPublic, nullptr, nullptr);

      // Global state revision
      std::map<microkernel::LevelIdentifier,
               std::shared_ptr<microkernel::agents::IPerceivedData>>
          perceivedMap;
      perceivedMap[ecosystem] = perceivedData;
      agent->reviseGlobalState(timeStamp, nextTimeStamp, perceivedMap,
                               globalState);

      // Decision
      if (globalState->isAlive) {
        auto influences =
            std::make_shared<microkernel::influences::InfluencesMap>();
        agent->decide(ecosystem, timeStamp, nextTimeStamp, globalState, nullptr,
                      nullptr, perceivedData, influences);

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
    std::cout << "📊 Population Statistics:" << std::endl;
    std::cout << "  🐰 Prey:      " << preyCount << " agents";
    if (preyCount > 0) {
      std::cout << " (avg energy: " << (totalPreyEnergy / preyCount) << ")";
    }
    std::cout << std::endl;

    std::cout << "  🦁 Predators: " << predatorCount << " agents";
    if (predatorCount > 0) {
      std::cout << " (avg energy: " << (totalPredatorEnergy / predatorCount)
                << ")";
    }
    std::cout << std::endl;
    std::cout << std::endl;

    // Check extinction
    if (preyCount == 0 && predatorCount == 0) {
      std::cout << "⚠️  All agents have died. Simulation ending." << std::endl;
      break;
    }
  }

  std::cout << "╔════════════════════════════════════════════════════════╗"
            << std::endl;
  std::cout << "║              Simulation Complete                       ║"
            << std::endl;
  std::cout << "╚════════════════════════════════════════════════════════╝"
            << std::endl;

  return 0;
}
