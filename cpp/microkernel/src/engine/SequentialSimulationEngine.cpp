#include "../../include/engine/SequentialSimulationEngine.h"
#include "../../include/dynamicstate/ConsistentPublicLocalDynamicState.h"
#include "../../include/dynamicstate/IPublicDynamicStateMap.h"

#include <iostream>
#include <limits>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace engine {

// Helper class for Dynamic State Map
class PublicDynamicStateMap : public dynamicstate::IPublicDynamicStateMap {
private:
  std::map<LevelIdentifier,
           std::shared_ptr<dynamicstate::IPublicLocalDynamicState>>
      states;

public:
  std::set<LevelIdentifier> keySet() const override {
    std::set<LevelIdentifier> keys;
    for (const auto &pair : states) {
      keys.insert(pair.first);
    }
    return keys;
  }

  std::shared_ptr<dynamicstate::IPublicLocalDynamicState>
  get(const LevelIdentifier &level) const override {
    auto it = states.find(level);
    if (it == states.end()) {
      throw std::out_of_range("Level not found in dynamic state map: " +
                              level.toString());
    }
    return it->second;
  }

  void
  put(std::shared_ptr<dynamicstate::IPublicLocalDynamicState> state) override {
    states[state->getLevel()] = state;
  }
};

SequentialSimulationEngine::SequentialSimulationEngine()
    : abortionRequested(false) {
  dynamicStates = std::make_shared<PublicDynamicStateMap>();
}

void SequentialSimulationEngine::addProbe(const std::string &identifier,
                                          std::shared_ptr<IProbe> probe) {
  std::lock_guard<std::mutex> lock(probesMutex);
  if (probes.find(identifier) != probes.end()) {
    throw std::invalid_argument("Probe with identifier '" + identifier +
                                "' already exists.");
  }
  probes[identifier] = probe;
}

std::shared_ptr<IProbe>
SequentialSimulationEngine::removeProbe(const std::string &identifier) {
  std::lock_guard<std::mutex> lock(probesMutex);
  auto it = probes.find(identifier);
  if (it == probes.end()) {
    return nullptr;
  }
  auto probe = it->second;
  probes.erase(it);
  return probe;
}

std::set<std::string> SequentialSimulationEngine::getProbesIdentifiers() const {
  // Note: Not thread-safe for read, but acceptable for this simple
  // implementation
  std::set<std::string> identifiers;
  for (const auto &pair : probes) {
    identifiers.insert(pair.first);
  }
  return identifiers;
}

void SequentialSimulationEngine::requestSimulationAbortion() {
  abortionRequested = true;
}

void SequentialSimulationEngine::runNewSimulation(
    std::shared_ptr<ISimulationModel> simulationModel) {
  if (!simulationModel) {
    throw std::invalid_argument("Simulation model cannot be null.");
  }

  currentModel = simulationModel;
  abortionRequested = false;

  try {
    initializeSimulation(simulationModel);
    runSimulationLoop();
  } catch (const std::exception &e) {
    // Ensure we clean up or log if needed
    std::cerr << "Simulation failed: " << e.what() << std::endl;
    throw;
  }
}

void SequentialSimulationEngine::initializeSimulation(
    std::shared_ptr<ISimulationModel> model) {
  SimulationTimeStamp initialTime = model->getInitialTime();

  // 1. Generate Levels
  auto generatedLevels = model->generateLevels(initialTime);
  levels.clear();
  for (const auto &level : generatedLevels) {
    levels[level->getIdentifier()] = level;
  }

  // 2. Generate Environment
  auto envData = model->generateEnvironment(initialTime, levels);
  environment = envData.getEnvironment();
  // Note: Initial influences from environment generation are ignored in this
  // simple engine In a full implementation, they might be processed.

  // 3. Generate Agents
  auto agentData = model->generateAgents(initialTime, levels);
  agents.clear();
  agentsByLevel.clear();
  for (const auto &agent : agentData.getAgents()) {
    agents.insert(agent);
    for (const auto &levelId : agent->getLevels()) {
      agentsByLevel[levelId].insert(agent);
    }
  }

  // 4. Initialize Dynamic States
  // We populate the dynamic state map with the initial consistent states of the
  // levels
  for (const auto &pair : levels) {
    auto level = pair.second;
    dynamicStates->put(level->getLastConsistentState());
  }

  notifyProbesOfPreparation(initialTime);
}

void SequentialSimulationEngine::runSimulationLoop() {
  SimulationTimeStamp currentTime = currentModel->getInitialTime();
  notifyProbesOfStart(currentTime);

  while (!abortionRequested &&
         !currentModel->isFinalTimeOrAfter(currentTime, *this)) {
    // 1. Determine next time step (minimum of all levels' next times)
    // For simplicity, we assume all levels move in sync or we take the smallest
    // step. A robust engine would handle multi-rate simulation. Here, we ask
    // the first level for its next time (assuming single level or sync).
    if (levels.empty())
      break;

    // Find the smallest next time across all levels
    SimulationTimeStamp nextTime =
        SimulationTimeStamp(std::numeric_limits<long long>::max());
    bool hasNextTime = false;

    for (const auto &pair : levels) {
      SimulationTimeStamp levelNext = pair.second->getNextTime(currentTime);
      if (!hasNextTime || levelNext < nextTime) {
        nextTime = levelNext;
        hasNextTime = true;
      }
    }

    if (!hasNextTime)
      break; // Should not happen

    // 2. Process each level
    for (const auto &pair : levels) {
      auto levelId = pair.first;
      auto level = pair.second;

      // A. Perception
      // Agents perceive the CURRENT state (at currentTime)
      for (const auto &agent : agentsByLevel[levelId]) {
        // Get public local states of other agents in this level
        std::map<LevelIdentifier, std::shared_ptr<agents::ILocalStateOfAgent>>
            publicStates;
        // This is expensive O(N^2), but correct for a reference implementation
        for (const auto &otherAgent : agentsByLevel[levelId]) {
          if (otherAgent != agent) {
            try {
              // In a real implementation, we'd filter by perception range/graph
              // Here we assume full connectivity for simplicity or let the
              // model handle it Actually, the agent's perceive method takes ALL
              // public states. But we need to construct the map of maps? No,
              // the interface expects: map<LevelIdentifier,
              // shared_ptr<ILocalStateOfAgent>> publicLocalStates Wait, the
              // interface `perceive` takes `publicLocalStates`. Is it the
              // public states of THIS agent or OTHER agents? "publicLocalStates
              // All the public local states of the agent." -> OF THE AGENT
              // ITSELF.

              // Ah, looking at IAgent.h:
              // perceive(..., map<LevelIdentifier,
              // shared_ptr<ILocalStateOfAgent>> &publicLocalStates, ...) "All
              // the public local states of the agent."

              // So the agent perceives the global environment + its own public
              // states + dynamic states. It does NOT receive a list of other
              // agents' states directly in the arguments? Wait, `dynamicStates`
              // contains the level states. Where do agents see other agents?
              // Usually via the Environment or the Level's dynamic state if it
              // aggregates them. OR, the `publicLocalStates` argument is indeed
              // its own states.

              // Let's check IAgent.h again.
              // "publicLocalStates All the public local states of the agent."

              // So how does an agent perceive neighbors?
              // Typically, the Environment provides this, or the Level.
              // In SIMILAR, the `IPerceivedData` returned by `perceive` is what
              // the agent "sees". The `perceive` method is where the agent
              // *computes* what it sees. To do that, it needs access to the
              // world. It has `dynamicStates`. Does `dynamicStates` contain
              // agents? `IPublicLocalDynamicState` is the state of the LEVEL.
              // If the level tracks agents, then yes.
            } catch (...) {
            }
          }
        }

        auto perceivedData = agent->perceive(
            levelId, currentTime, nextTime, agent->getPublicLocalStates(),
            agent->getPrivateLocalState(levelId), dynamicStates);

        agent->setPerceivedData(perceivedData);
      }

      // B. Decision
      auto influences = std::make_shared<influences::InfluencesMap>();
      for (const auto &agent : agentsByLevel[levelId]) {
        // Revise global state first
        agent->reviseGlobalState(currentTime, nextTime,
                                 agent->getPerceivedData(),
                                 agent->getGlobalState());

        // Decide
        agent->decide(levelId, currentTime, nextTime, agent->getGlobalState(),
                      agent->getPublicLocalState(levelId),
                      agent->getPrivateLocalState(levelId),
                      agent->getPerceivedData()[levelId], // Assuming map access
                      influences);
      }

      // C. Reaction (Environment Update)
      // 1. Regular Reaction
      // We need to collect ALL influences from ALL agents for this level.

      auto levelInfluences = std::make_shared<influences::InfluencesMap>();
      for (const auto &agent : agentsByLevel[levelId]) {
        auto agentInfluences = std::make_shared<influences::InfluencesMap>();
        agent->decide(levelId, currentTime, nextTime, agent->getGlobalState(),
                      agent->getPublicLocalState(levelId),
                      agent->getPrivateLocalState(levelId),
                      agent->getPerceivedData()[levelId], agentInfluences);
        levelInfluences->addAll(*agentInfluences);
      }

      // Apply reaction on the Level
      auto consistentState = level->getLastConsistentState();

      // Extract regular influences for this level
      std::list<std::shared_ptr<influences::IInfluence>> influenceList =
          levelInfluences->getInfluencesForLevel(levelId);

      std::set<std::shared_ptr<influences::IInfluence>> regularInfluences(
          influenceList.begin(), influenceList.end());

      auto remainingInfluences = std::make_shared<influences::InfluencesMap>();

      level->makeRegularReaction(currentTime, nextTime, consistentState,
                                 regularInfluences, remainingInfluences);

      // Update dynamic state map with new state
      dynamicStates->put(level->getLastConsistentState());
    }

    currentTime = nextTime;
    notifyProbesOfUpdate(currentTime);
  }

  notifyProbesOfEnd(currentTime);
}

// Probe notifications
void SequentialSimulationEngine::notifyProbesOfPreparation(
    const SimulationTimeStamp &initialTime) {
  std::lock_guard<std::mutex> lock(probesMutex);
  for (auto &pair : probes) {
    pair.second->prepareObservation();
  }
}

void SequentialSimulationEngine::notifyProbesOfStart(
    const SimulationTimeStamp &initialTime) {
  std::lock_guard<std::mutex> lock(probesMutex);
  for (auto &pair : probes) {
    pair.second->observeAtInitialTimes(initialTime, *this);
  }
}

void SequentialSimulationEngine::notifyProbesOfEnd(
    const SimulationTimeStamp &finalTime) {
  std::lock_guard<std::mutex> lock(probesMutex);
  for (auto &pair : probes) {
    pair.second->observeAtFinalTime(finalTime, *this);
    pair.second->endObservation();
  }
}

void SequentialSimulationEngine::notifyProbesOfUpdate(
    const SimulationTimeStamp &time) {
  std::lock_guard<std::mutex> lock(probesMutex);
  for (auto &pair : probes) {
    pair.second->observeAtPartialConsistentTime(time, *this);
  }
}

// Getters
std::shared_ptr<dynamicstate::IPublicDynamicStateMap>
SequentialSimulationEngine::getSimulationDynamicStates() const {
  return dynamicStates;
}

std::set<std::shared_ptr<agents::IAgent4Engine>>
SequentialSimulationEngine::getAgents() const {
  return agents;
}

std::set<LevelIdentifier>
SequentialSimulationEngine::getLevelIdentifiers() const {
  std::set<LevelIdentifier> ids;
  for (const auto &pair : levels) {
    ids.insert(pair.first);
  }
  return ids;
}

std::map<LevelIdentifier, std::shared_ptr<levels::ILevel>>
SequentialSimulationEngine::getLevels() const {
  return levels;
}

std::set<std::shared_ptr<agents::IAgent4Engine>>
SequentialSimulationEngine::getAgents(const LevelIdentifier &level) const {
  auto it = agentsByLevel.find(level);
  if (it == agentsByLevel.end()) {
    return {};
  }
  return it->second;
}

std::shared_ptr<environment::IEnvironment4Engine>
SequentialSimulationEngine::getEnvironment() const {
  return environment;
}

std::shared_ptr<dynamicstate::ConsistentPublicLocalDynamicState>
SequentialSimulationEngine::disambiguation(
    std::shared_ptr<dynamicstate::TransitoryPublicLocalDynamicState>
        transitoryDynamicState) const {
  // Simple implementation: just return null or throw, as this logic is complex
  // and model-dependent In a real engine, this would handle state conflicts.
  return nullptr;
}

} // namespace engine
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
