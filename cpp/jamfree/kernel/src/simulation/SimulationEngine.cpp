#include "../../include/simulation/SimulationEngine.h"
#include <algorithm>
#include <iostream>

namespace jamfree {
namespace kernel {
namespace simulation {

SimulationEngine::SimulationEngine(double dt)
    : m_dt(dt), m_current_time(0.0), m_step_count(0),
      m_global_state(std::make_shared<SimulationGlobalState>(0.0)) {}

void SimulationEngine::addAgent(std::shared_ptr<agents::VehicleAgent> agent) {
  if (!agent) {
    return;
  }

  // Check if already exists
  if (m_agents_by_id.find(agent->getId()) != m_agents_by_id.end()) {
    std::cerr << "Warning: Agent " << agent->getId() << " already exists"
              << std::endl;
    return;
  }

  m_agents.push_back(agent);
  m_agents_by_id[agent->getId()] = agent;
}

void SimulationEngine::removeAgent(const std::string &agentId) {
  auto it = m_agents_by_id.find(agentId);
  if (it == m_agents_by_id.end()) {
    return;
  }

  // Remove from vector
  m_agents.erase(std::remove_if(m_agents.begin(), m_agents.end(),
                                [&agentId](const auto &agent) {
                                  return agent->getId() == agentId;
                                }),
                 m_agents.end());

  // Remove from map
  m_agents_by_id.erase(it);
}

std::shared_ptr<agents::VehicleAgent>
SimulationEngine::getAgent(const std::string &agentId) {
  auto it = m_agents_by_id.find(agentId);
  if (it != m_agents_by_id.end()) {
    return it->second;
  }
  return nullptr;
}

void SimulationEngine::setReactionModel(
    const agents::LevelIdentifier &level,
    std::shared_ptr<agents::IReactionModel> reactionModel) {
  m_reaction_models[level] = reactionModel;
}

agents::IReactionModel *
SimulationEngine::getReactionModel(const agents::LevelIdentifier &level) {
  auto it = m_reaction_models.find(level);
  if (it != m_reaction_models.end()) {
    return it->second.get();
  }
  return nullptr;
}

void SimulationEngine::step() {
  // Update global state time
  m_global_state->setTime(m_current_time);

  // Execute simulation cycle
  perceptionPhase();
  auto influences = decisionPhase();
  reactionPhase(influences);

  // Advance time
  m_current_time += m_dt;
  m_step_count++;
}

void SimulationEngine::run(int numSteps) {
  for (int i = 0; i < numSteps; ++i) {
    step();
  }
}

void SimulationEngine::reset() {
  m_current_time = 0.0;
  m_step_count = 0;
  m_global_state->setTime(0.0);
  m_agents.clear();
  m_agents_by_id.clear();
}

void SimulationEngine::perceptionPhase() {
  agents::SimulationTimeStamp t0(m_current_time);
  agents::SimulationTimeStamp t1(m_current_time + m_dt);

  agents::LevelIdentifier microLevel("Microscopic");

  for (auto &agent : m_agents) {
    // For each level the agent participates in
    // For now, we focus on microscopic level
    auto levels = agent->getLevels();
    if (levels.find(microLevel) == levels.end()) {
      continue;
    }

    auto perceptionModel = agent->getPerceptionModel(microLevel);
    if (!perceptionModel) {
      continue;
    }

    // Get all public states for the agent
    std::map<agents::LevelIdentifier, std::shared_ptr<agents::ILocalState>>
        publicStates;
    for (const auto &level : levels) {
      auto pubState = agent->getPublicLocalState(level);
      if (pubState) {
        publicStates[level] = pubState;
      }
    }

    auto privateState = agent->getPrivateLocalState(microLevel);
    if (!privateState) {
      continue;
    }

    // Execute perception - it returns perceived data
    auto perceivedData =
        perceptionModel->perceive(t0, t1, publicStates, privateState, nullptr);

    // TODO: Store perceived data in agent for decision phase
    // For now, decision phase will re-perceive (inefficient but functional)
  }
}

agents::InfluencesMap SimulationEngine::decisionPhase() {
  agents::SimulationTimeStamp t0(m_current_time);
  agents::SimulationTimeStamp t1(m_current_time + m_dt);

  agents::InfluencesMap allInfluences;
  agents::LevelIdentifier microLevel("Microscopic");

  for (auto &agent : m_agents) {
    auto levels = agent->getLevels();
    if (levels.find(microLevel) == levels.end()) {
      continue;
    }

    auto decisionModel = agent->getDecisionModel(microLevel);
    if (!decisionModel) {
      continue;
    }

    auto publicState = agent->getPublicLocalState(microLevel);
    auto privateState = agent->getPrivateLocalState(microLevel);

    if (!publicState || !privateState) {
      continue;
    }

    // Re-perceive (TODO: use stored perceived data from perception phase)
    auto perceptionModel = agent->getPerceptionModel(microLevel);
    std::shared_ptr<agents::IPerceivedData> perceivedData;

    if (perceptionModel) {
      std::map<agents::LevelIdentifier, std::shared_ptr<agents::ILocalState>>
          publicStates;
      for (const auto &level : levels) {
        auto pubState = agent->getPublicLocalState(level);
        if (pubState) {
          publicStates[level] = pubState;
        }
      }
      perceivedData = perceptionModel->perceive(t0, t1, publicStates,
                                                privateState, nullptr);
    }

    if (!perceivedData) {
      continue;
    }

    // Execute decision
    auto agentInfluences = std::make_shared<agents::InfluencesMap>();
    decisionModel->decide(t0, t1, m_global_state, publicState, privateState,
                          perceivedData, agentInfluences);

    // Merge influences
    allInfluences.addAll(*agentInfluences);
  }

  return allInfluences;
}

void SimulationEngine::reactionPhase(const agents::InfluencesMap &influences) {
  agents::SimulationTimeStamp t0(m_current_time);
  agents::SimulationTimeStamp t1(m_current_time + m_dt);

  // Apply reactions for each level
  for (const auto &[level, reactionModel] : m_reaction_models) {
    if (reactionModel) {
      reactionModel->react(t0, t1, influences);
    }
  }
}

} // namespace simulation
} // namespace kernel
} // namespace jamfree
