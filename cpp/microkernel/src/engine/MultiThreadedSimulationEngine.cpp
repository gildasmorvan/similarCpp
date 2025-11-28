#include "engine/MultiThreadedSimulationEngine.h"
#include "IProbe.h"
#include "agents/IPerceivedData.h"
#include "environment/IEnvironment4Engine.h"
#include <algorithm>
#include <iostream>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace engine {

MultiThreadedSimulationEngine::MultiThreadedSimulationEngine(size_t numThreads)
    : numThreads(numThreads == 0 ? std::thread::hardware_concurrency()
                                 : numThreads),
      currentTime(0) {
  if (this->numThreads == 0) {
    this->numThreads = 4; // Fallback if hardware_concurrency returns 0
  }
  std::cout << "MultiThreadedSimulationEngine initialized with "
            << this->numThreads << " threads" << std::endl;

  dynamicStates = std::make_shared<PublicDynamicStateMap>();
}

void MultiThreadedSimulationEngine::addProbe(const std::string &identifier,
                                             std::shared_ptr<IProbe> probe) {
  if (identifier.empty() || !probe) {
    throw std::invalid_argument(
        "Probe identifier and probe cannot be null/empty");
  }
  if (probes.find(identifier) != probes.end()) {
    throw std::invalid_argument("A probe with identifier '" + identifier +
                                "' already exists");
  }
  probes[identifier] = probe;
}

std::shared_ptr<IProbe>
MultiThreadedSimulationEngine::removeProbe(const std::string &identifier) {
  auto it = probes.find(identifier);
  if (it != probes.end()) {
    auto probe = it->second;
    probes.erase(it);
    return probe;
  }
  return nullptr;
}

std::set<std::string>
MultiThreadedSimulationEngine::getProbesIdentifiers() const {
  std::set<std::string> identifiers;
  for (const auto &pair : probes) {
    identifiers.insert(pair.first);
  }
  return identifiers;
}

void MultiThreadedSimulationEngine::requestSimulationAbortion() {
  abortRequested = true;
}

// Getters implementation
std::shared_ptr<dynamicstate::IPublicDynamicStateMap>
MultiThreadedSimulationEngine::getSimulationDynamicStates() const {
  return dynamicStates;
}

std::set<std::shared_ptr<agents::IAgent4Engine>>
MultiThreadedSimulationEngine::getAgents() const {
  return agents;
}

std::set<LevelIdentifier>
MultiThreadedSimulationEngine::getLevelIdentifiers() const {
  std::set<LevelIdentifier> ids;
  for (const auto &pair : levels) {
    ids.insert(pair.first);
  }
  return ids;
}

std::map<LevelIdentifier, std::shared_ptr<levels::ILevel>>
MultiThreadedSimulationEngine::getLevels() const {
  return levels;
}

std::set<std::shared_ptr<agents::IAgent4Engine>>
MultiThreadedSimulationEngine::getAgents(const LevelIdentifier &level) const {
  auto it = agentsByLevel.find(level);
  if (it != agentsByLevel.end()) {
    return it->second;
  }
  return {};
}

std::shared_ptr<environment::IEnvironment4Engine>
MultiThreadedSimulationEngine::getEnvironment() const {
  return environment;
}

std::shared_ptr<dynamicstate::ConsistentPublicLocalDynamicState>
MultiThreadedSimulationEngine::disambiguation(
    std::shared_ptr<dynamicstate::TransitoryPublicLocalDynamicState>
        transitoryDynamicState) const {
  return nullptr;
}

void MultiThreadedSimulationEngine::runNewSimulation(
    std::shared_ptr<ISimulationModel> model) {

  abortRequested = false;
  currentModel = model;

  // Get initial time
  currentTime = model->getInitialTime();

  // 1. Generate Levels
  std::vector<std::shared_ptr<levels::ILevel>> levelsVector =
      model->generateLevels(currentTime);
  levels.clear();
  std::map<LevelIdentifier, std::shared_ptr<levels::ILevel>> levelsMap;

  for (const auto &level : levelsVector) {
    levels[level->getIdentifier()] = level;
    levelsMap[level->getIdentifier()] = level;
  }

  // 2. Generate Environment
  auto envInitData = model->generateEnvironment(currentTime, levelsMap);
  environment = envInitData.getEnvironment();

  // 3. Generate Agents
  auto agentInitData = model->generateAgents(currentTime, levelsMap);
  agents.clear();
  agentsByLevel.clear();

  for (const auto &agent : agentInitData.getAgents()) {
    agents.insert(agent);
    for (const auto &levelId : agent->getLevels()) {
      agentsByLevel[levelId].insert(agent);
    }
  }

  // 4. Initialize Dynamic States
  auto publicStateMap =
      std::dynamic_pointer_cast<PublicDynamicStateMap>(dynamicStates);
  if (!publicStateMap) {
    publicStateMap = std::make_shared<PublicDynamicStateMap>();
    dynamicStates = publicStateMap;
  }

  for (const auto &pair : levels) {
    auto level = pair.second;
    publicStateMap->put(level->getLastConsistentState());
  }

  // Notify probes of initial time
  for (const auto &probe : probes) {
    probe.second->observeAtInitialTimes(currentTime, *this);
  }

  // Main simulation loop
  runSimulation(SimulationTimeStamp(std::numeric_limits<long>::max()));
}

void MultiThreadedSimulationEngine::runSimulation(
    const SimulationTimeStamp &finalTime) {
  if (!currentModel) {
    throw std::runtime_error("Simulation has not been initialized.");
  }

  // Build agent list for parallel processing
  std::vector<std::shared_ptr<agents::IAgent4Engine>> agentsVector;
  for (const auto &agent : agents) {
    agentsVector.push_back(agent);
  }

  auto publicStateMap =
      std::dynamic_pointer_cast<PublicDynamicStateMap>(dynamicStates);

  while (!currentModel->isFinalTimeOrAfter(currentTime, *this) &&
         !abortRequested && currentTime < finalTime) {

    SimulationTimeStamp nextTime(currentTime, 1);

    // Prepare results storage for parallel processing
    std::vector<
        std::map<LevelIdentifier, std::shared_ptr<influences::InfluencesMap>>>
        agentInfluences(agentsVector.size());

    // PARALLEL PERCEPTION AND DECISION
    parallelProcess(
        agentsVector,
        [&](size_t index, std::shared_ptr<agents::IAgent4Engine> agent) {
          for (const auto &levelId : agent->getLevels()) {
            // Perceive
            auto perceived = agent->perceive(
                levelId, currentTime, nextTime, agent->getPublicLocalStates(),
                agent->getPrivateLocalState(levelId), dynamicStates);
            agent->setPerceivedData(perceived);

            // Revise Global State
            // Note: getPerceivedData returns a map, but we just updated it.
            // reviseGlobalState takes the whole map.
            agent->reviseGlobalState(currentTime, nextTime,
                                     agent->getPerceivedData(),
                                     agent->getGlobalState());

            // Decide
            auto influences = std::make_shared<influences::InfluencesMap>();
            agent->decide(levelId, currentTime, nextTime,
                          agent->getGlobalState(),
                          agent->getPublicLocalState(levelId),
                          agent->getPrivateLocalState(levelId),
                          perceived, // Use the one we just got
                          influences);

            agentInfluences[index][levelId] = influences;
          }
        });

    if (abortRequested)
      break;

    // SEQUENTIAL REACTION PHASE
    for (const auto &pair : levels) {
      auto levelId = pair.first;
      auto level = pair.second;

      // Aggregate influences for this level
      auto levelInfluences = std::make_shared<influences::InfluencesMap>();
      for (const auto &agentMap : agentInfluences) {
        auto it = agentMap.find(levelId);
        if (it != agentMap.end()) {
          levelInfluences->addAll(*it->second);
        }
      }

      // Apply reaction on the Level
      auto consistentState = level->getLastConsistentState();

      // Extract regular influences for this level
      // Note: getInfluencesForLevel returns std::list
      std::list<std::shared_ptr<influences::IInfluence>> influenceList =
          levelInfluences->getInfluencesForLevel(levelId);

      std::set<std::shared_ptr<influences::IInfluence>> regularInfluences(
          influenceList.begin(), influenceList.end());

      auto remainingInfluences = std::make_shared<influences::InfluencesMap>();

      level->makeRegularReaction(currentTime, nextTime, consistentState,
                                 regularInfluences, remainingInfluences);

      // Update dynamic state map with new state
      publicStateMap->put(level->getLastConsistentState());
    }

    // Advance time
    currentTime = nextTime;

    // Notify probes
    for (const auto &probe : probes) {
      probe.second->observeAtPartialConsistentTime(currentTime, *this);
    }

    // Check final time again after update?
    // The loop condition handles it.
    if (finalTime < currentTime) {
      break;
    }
  }

  if (abortRequested) {
    throw ExceptionSimulationAborted("Simulation was aborted");
  }
}

void MultiThreadedSimulationEngine::parallelPerception(
    const std::vector<std::shared_ptr<agents::IAgent4Engine>> &agents,
    SimulationTimeStamp timeLowerBound, SimulationTimeStamp timeUpperBound,
    std::shared_ptr<dynamicstate::IPublicDynamicStateMap> consistentState) {
  // Unused now
}

void MultiThreadedSimulationEngine::parallelDecision(
    const std::vector<std::shared_ptr<agents::IAgent4Engine>> &agents,
    SimulationTimeStamp timeLowerBound, SimulationTimeStamp timeUpperBound,
    std::shared_ptr<dynamicstate::IPublicDynamicStateMap> transitoryState) {
  // Unused now
}

template <typename Func>
void MultiThreadedSimulationEngine::parallelProcess(
    const std::vector<std::shared_ptr<agents::IAgent4Engine>> &agents,
    Func processFunc) {

  if (agents.empty())
    return;

  size_t agentsPerThread = (agents.size() + numThreads - 1) / numThreads;
  std::vector<std::thread> threads;

  for (size_t t = 0; t < numThreads; ++t) {
    size_t start = t * agentsPerThread;
    size_t end = std::min(start + agentsPerThread, agents.size());

    if (start >= agents.size())
      break;

    threads.emplace_back([&, start, end]() {
      for (size_t i = start; i < end && !abortRequested; ++i) {
        processFunc(i, agents[i]);
      }
    });
  }

  // Wait for all threads to complete
  for (auto &thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

std::shared_ptr<ISimulationEngine>
MultiThreadedSimulationEngine::clone() const {
  auto clonedEngine =
      std::make_shared<MultiThreadedSimulationEngine>(this->numThreads);

  // 1. Clone probes
  for (const auto &pair : this->probes) {
    clonedEngine->addProbe(pair.first, pair.second->clone());
  }

  // 2. Clone levels
  for (const auto &pair : this->levels) {
    clonedEngine->levels[pair.first] = pair.second->clone();
  }

  // 3. Clone environment
  if (this->environment) {
    clonedEngine->environment =
        std::dynamic_pointer_cast<environment::IEnvironment4Engine>(
            this->environment->clone());
  }

  // 4. Clone agents
  for (const auto &agent : this->agents) {
    auto clonedAgent =
        std::dynamic_pointer_cast<agents::IAgent4Engine>(agent->clone());
    clonedEngine->agents.insert(clonedAgent);
    for (const auto &levelId : clonedAgent->getLevels()) {
      clonedEngine->agentsByLevel[levelId].insert(clonedAgent);
    }
  }

  // 5. Clone dynamic states
  // The dynamic states are held by the levels, so we just need to populate the
  // map from the cloned levels
  // We need to cast to PublicDynamicStateMap to use put() or ensure
  // dynamicStates is initialized
  auto publicStateMap = std::make_shared<PublicDynamicStateMap>();
  clonedEngine->dynamicStates = publicStateMap;

  for (const auto &pair : clonedEngine->levels) {
    publicStateMap->put(pair.second->getLastConsistentState());
  }

  // 6. Copy other fields
  clonedEngine->abortRequested = this->abortRequested.load();

  return clonedEngine;
}

} // namespace engine
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
