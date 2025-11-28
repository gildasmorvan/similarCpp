/**
 * SIMILAR C++ - Web-Enabled Predator-Prey Simulation
 *
 * This example demonstrates the complete web interface for SIMILAR C++
 * running a Predator-Prey simulation using the SequentialSimulationEngine.
 */

#include "../../microkernel/include/engine/MultiThreadedSimulationEngine.h"
#include "../../microkernel/include/engine/SequentialSimulationEngine.h"
#include "../../microkernel/include/levels/ILevel.h"
#include "../include/agents/ExtendedAgent.h"
#include "../include/agents/IAgtDecisionModel.h"
#include "../include/agents/IAgtGlobalStateRevisionModel.h"
#include "../include/agents/IAgtPerceptionModel.h"
#include "../include/libs/web/SimilarWebRunner.h"
#include "../include/simulationmodel/ISimulationParameters.h"
#include <cmath>
#include <iostream>
#include <memory>
#include <random>
#include <thread>
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

// ===== Agent Local State =====
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

  std::shared_ptr<microkernel::ILocalState> clone() const override {
    return std::make_shared<SpatialLocalState>(*this);
  }
};

// ===== Perceived Data =====
class EcosystemPerceivedData : public microkernel::agents::IPerceivedData {
private:
  microkernel::LevelIdentifier level;
  microkernel::SimulationTimeStamp timeLower, timeUpper;
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

  std::shared_ptr<microkernel::agents::IPerceivedData> clone() const override {
    return std::make_shared<EcosystemPerceivedData>(*this);
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

  std::shared_ptr<microkernel::agents::IGlobalState> clone() const override {
    return std::make_shared<EcosystemGlobalState>(*this);
  }
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
    auto ownState =
        std::dynamic_pointer_cast<SpatialLocalState>(privateLocalState);
    if (!ownState)
      return perceivedData;

    // In a real implementation, we would query the dynamicStates or an
    // environment object to find neighbors. Since we don't have a spatial index
    // in the level yet, we'll skip actual neighbor detection for this demo OR
    // we can hack it if we had access to all agents. For now, agents are
    // "blind" but move randomly, which is enough to demonstrate the engine
    // running.

    return perceivedData;
  }
};

// ===== Decision Models =====
class PreyDecisionModel : public extendedkernel::agents::IAgtDecisionModel {
private:
  microkernel::LevelIdentifier level;
  double moveSpeed;

public:
  PreyDecisionModel(const microkernel::LevelIdentifier &level, double speed)
      : level(level), moveSpeed(speed) {}
  microkernel::LevelIdentifier getLevel() const override { return level; }
  void decide(
      const microkernel::SimulationTimeStamp &,
      const microkernel::SimulationTimeStamp &,
      std::shared_ptr<microkernel::agents::IGlobalState> globalState,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent> publicLocalState,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent>,
      std::shared_ptr<microkernel::agents::IPerceivedData>,
      std::shared_ptr<microkernel::influences::InfluencesMap>) override {
    auto state = std::dynamic_pointer_cast<EcosystemGlobalState>(globalState);
    auto spatialState =
        std::dynamic_pointer_cast<SpatialLocalState>(publicLocalState);
    if (state && spatialState && state->isAlive) {
      spatialState->setPosition(
          spatialState->getPosition().randomMove(moveSpeed));
      state->energy -= 1;
    }
  }
};

class PredatorDecisionModel : public extendedkernel::agents::IAgtDecisionModel {
private:
  microkernel::LevelIdentifier level;
  double moveSpeed;

public:
  PredatorDecisionModel(const microkernel::LevelIdentifier &level, double speed)
      : level(level), moveSpeed(speed) {}
  microkernel::LevelIdentifier getLevel() const override { return level; }
  void decide(
      const microkernel::SimulationTimeStamp &,
      const microkernel::SimulationTimeStamp &,
      std::shared_ptr<microkernel::agents::IGlobalState> globalState,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent> publicLocalState,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent>,
      std::shared_ptr<microkernel::agents::IPerceivedData>,
      std::shared_ptr<microkernel::influences::InfluencesMap>) override {
    auto state = std::dynamic_pointer_cast<EcosystemGlobalState>(globalState);
    auto spatialState =
        std::dynamic_pointer_cast<SpatialLocalState>(publicLocalState);
    if (state && spatialState && state->isAlive) {
      spatialState->setPosition(
          spatialState->getPosition().randomMove(moveSpeed));
      state->energy -= 2;
    }
  }
};

// ===== Global State Revision =====
class EcosystemGlobalStateRevisionModel
    : public extendedkernel::agents::IAgtGlobalStateRevisionModel {
private:
  std::string type;

public:
  EcosystemGlobalStateRevisionModel(const std::string &type) : type(type) {}
  void reviseGlobalState(
      const microkernel::SimulationTimeStamp &,
      const microkernel::SimulationTimeStamp &,
      const std::map<microkernel::LevelIdentifier,
                     std::shared_ptr<microkernel::agents::IPerceivedData>> &,
      std::shared_ptr<microkernel::agents::IGlobalState> globalState) override {
    auto state = std::dynamic_pointer_cast<EcosystemGlobalState>(globalState);
    if (state && state->isAlive) {
      state->age++;
      if (state->energy <= 0 || state->age > 100) {
        state->isAlive = false;
        // std::cout << type << " died." << std::endl;
      }
    }
  }
};

// ===== Level Implementation =====
class EcosystemLevel : public microkernel::levels::ILevel {
private:
  microkernel::LevelIdentifier id;
  std::shared_ptr<microkernel::dynamicstate::ConsistentPublicLocalDynamicState>
      consistentState;

public:
  EcosystemLevel(const microkernel::LevelIdentifier &id) : id(id) {
    consistentState = std::make_shared<
        microkernel::dynamicstate::ConsistentPublicLocalDynamicState>(
        microkernel::SimulationTimeStamp(0), id);
  }
  microkernel::LevelIdentifier getIdentifier() const override { return id; }
  std::set<microkernel::LevelIdentifier> getPerceptibleLevels() const override {
    return {id};
  }
  std::set<microkernel::LevelIdentifier>
  getInfluenceableLevels() const override {
    return {id};
  }
  std::shared_ptr<microkernel::dynamicstate::ConsistentPublicLocalDynamicState>
  getLastConsistentState() const override {
    return consistentState;
  }
  std::shared_ptr<microkernel::dynamicstate::TransitoryPublicLocalDynamicState>
  getLastTransitoryState() const override {
    return nullptr;
  }

  void makeRegularReaction(
      const microkernel::SimulationTimeStamp &,
      const microkernel::SimulationTimeStamp &,
      std::shared_ptr<
          microkernel::dynamicstate::ConsistentPublicLocalDynamicState>,
      const std::set<std::shared_ptr<microkernel::influences::IInfluence>> &,
      std::shared_ptr<microkernel::influences::InfluencesMap>) override {
    // No complex reaction needed for this simple demo
  }

  void makeSystemReaction(
      const microkernel::SimulationTimeStamp &,
      const microkernel::SimulationTimeStamp &,
      std::shared_ptr<
          microkernel::dynamicstate::ConsistentPublicLocalDynamicState>,
      const std::vector<std::shared_ptr<microkernel::influences::IInfluence>> &,
      bool, std::shared_ptr<microkernel::influences::InfluencesMap>) override {}

  microkernel::SimulationTimeStamp
  getNextTime(const microkernel::SimulationTimeStamp &currentTime) override {
    return microkernel::SimulationTimeStamp(currentTime.getIdentifier() + 1);
  }

  std::shared_ptr<microkernel::levels::ILevel> clone() const override {
    return std::make_shared<EcosystemLevel>(*this);
  }
};

// ===== Simulation Model =====
class PredatorPreyModel
    : public extendedkernel::simulationmodel::ISimulationModel {
private:
  microkernel::LevelIdentifier levelId;

public:
  PredatorPreyModel() : levelId("Ecosystem") {}

  extendedkernel::simulationmodel::ISimulationParameters *
  getSimulationParameters() override {
    return nullptr;
  }
  microkernel::SimulationTimeStamp getInitialTime() const override {
    return microkernel::SimulationTimeStamp(0);
  }

  bool
  isFinalTimeOrAfter(const microkernel::SimulationTimeStamp &currentTime,
                     const microkernel::ISimulationEngine &) const override {
    return currentTime.getIdentifier() >= 1000; // Run for 1000 steps
  }

  std::vector<std::shared_ptr<microkernel::levels::ILevel>>
  generateLevels(const microkernel::SimulationTimeStamp &) override {
    return {std::make_shared<EcosystemLevel>(levelId)};
  }

  microkernel::ISimulationModel::EnvironmentInitializationData
  generateEnvironment(
      const microkernel::SimulationTimeStamp &,
      const std::map<microkernel::LevelIdentifier,
                     std::shared_ptr<microkernel::levels::ILevel>> &) override {
    // Return empty environment for now
    return microkernel::ISimulationModel::EnvironmentInitializationData(
        nullptr);
  }

  microkernel::ISimulationModel::AgentInitializationData generateAgents(
      const microkernel::SimulationTimeStamp &,
      const std::map<microkernel::LevelIdentifier,
                     std::shared_ptr<microkernel::levels::ILevel>> &) override {

    microkernel::ISimulationModel::AgentInitializationData data;
    std::uniform_real_distribution<> posDist(0, 100);

    // Create Prey
    for (int i = 0; i < 10; ++i) {
      microkernel::AgentCategory cat("Prey");
      auto agent = std::make_shared<extendedkernel::agents::ExtendedAgent>(cat);

      // Initialize global state
      agent->initializeGlobalState(std::make_shared<EcosystemGlobalState>(50));

      // Create local state and add to level
      Position pos(posDist(gen), posDist(gen));
      auto state = std::make_shared<SpatialLocalState>(levelId, pos, agent);
      agent->includeNewLevel(levelId, state, state);

      // Specify behavior
      agent->specifyBehaviorForLevel(
          levelId, std::make_shared<EcosystemPerceptionModel>(levelId, 15),
          std::make_shared<PreyDecisionModel>(levelId, 2));
      agent->specifyGlobalStateRevisionModel(
          std::make_shared<EcosystemGlobalStateRevisionModel>("Prey"));

      // Add to agent set
      data.getAgents().insert(agent);
    }

    // Create Predators
    for (int i = 0; i < 3; ++i) {
      microkernel::AgentCategory cat("Predator");
      auto agent = std::make_shared<extendedkernel::agents::ExtendedAgent>(cat);

      // Initialize global state
      agent->initializeGlobalState(std::make_shared<EcosystemGlobalState>(80));

      // Create local state and add to level
      Position pos(posDist(gen), posDist(gen));
      auto state = std::make_shared<SpatialLocalState>(levelId, pos, agent);
      agent->includeNewLevel(levelId, state, state);

      // Specify behavior
      agent->specifyBehaviorForLevel(
          levelId, std::make_shared<EcosystemPerceptionModel>(levelId, 20),
          std::make_shared<PredatorDecisionModel>(levelId, 3));
      agent->specifyGlobalStateRevisionModel(
          std::make_shared<EcosystemGlobalStateRevisionModel>("Predator"));

      // Add to agent set
      data.getAgents().insert(agent);
    }

    return data;
  }
};

// ===== Main =====
int main() {
  std::cout << "Starting Web-Enabled Predator-Prey Simulation..." << std::endl;

  // Create multithreaded simulation engine (auto-detect CPU cores)
  auto engine =
      std::make_shared<microkernel::engine::MultiThreadedSimulationEngine>();
  auto model = std::make_shared<PredatorPreyModel>();

  extendedkernel::libs::web::SimilarWebRunner runner;
  runner.getConfig()->setPort(8080);
  runner.getConfig()->setSimulationName("Predator vs Prey");
  runner.getConfig()->setAutoOpenBrowser(
      false); // Disable auto-open for headless envs

  std::cout << "Initializing runner..." << std::endl;
  runner.initializeRunner(engine, model);

  std::cout << "Starting web server at http://localhost:8080" << std::endl;
  runner.showView();

  // Keep main thread alive
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}
