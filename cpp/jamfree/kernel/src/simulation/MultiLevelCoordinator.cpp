#include "../include/simulation/MultiLevelCoordinator.h"
#include "../include/levels/LevelIdentifiers.h"
#include <iostream>

namespace jamfree {
namespace kernel {
namespace simulation {

MultiLevelCoordinator::MultiLevelCoordinator()
    : m_current_time(0.0), m_step_count(0) {}

void MultiLevelCoordinator::addLevel(const LevelConfig &config) {
  m_levels[config.level] = config;
  std::cout << "Added level: " << config.level.getName()
            << " (dt=" << config.time_step
            << "s, freq=" << config.update_frequency << ")" << std::endl;
}

void MultiLevelCoordinator::step() {
  if (!m_engine) {
    std::cerr << "Error: No simulation engine set" << std::endl;
    return;
  }

  // Update each level according to its frequency
  for (auto &[level, config] : m_levels) {
    if (!config.active) {
      continue;
    }

    if (shouldUpdateLevel(level)) {
      updateLevel(level);
    }
  }

  // Synchronize state between levels
  synchronizeLevels();

  // Advance time (use smallest time step)
  double min_dt = 1.0;
  for (const auto &[level, config] : m_levels) {
    if (config.active && config.time_step < min_dt) {
      min_dt = config.time_step;
    }
  }

  m_current_time += min_dt;
  m_step_count++;
}

void MultiLevelCoordinator::run(int numSteps) {
  for (int i = 0; i < numSteps; ++i) {
    step();
  }
}

void MultiLevelCoordinator::transitionAgent(
    const std::string &agentId, const agents::LevelIdentifier &fromLevel,
    const agents::LevelIdentifier &toLevel) {
  auto agent = m_engine->getAgent(agentId);
  if (!agent) {
    std::cerr << "Error: Agent " << agentId << " not found" << std::endl;
    return;
  }

  std::cout << "Transitioning agent " << agentId << " from "
            << fromLevel.getName() << " to " << toLevel.getName() << std::endl;

  // Get state from source level
  auto sourcePublicState = agent->getPublicLocalState(fromLevel);
  auto sourcePrivateState = agent->getPrivateLocalState(fromLevel);

  if (!sourcePublicState || !sourcePrivateState) {
    std::cerr << "Error: Agent " << agentId << " has no state in "
              << fromLevel.getName() << std::endl;
    return;
  }

  // Clone states to target level
  auto targetPublicState = sourcePublicState->clone();
  auto targetPrivateState = sourcePrivateState->clone();

  // Add target level if not already present
  if (!agent->hasLevel(toLevel)) {
    agent->addLevel(toLevel);
  }

  // Set states in target level
  agent->setPublicLocalState(toLevel, targetPublicState);
  agent->setPrivateLocalState(toLevel, targetPrivateState);

  // Update tracking
  m_agent_levels[agentId].push_back(toLevel);

  std::cout << "  Transition complete" << std::endl;
}

void MultiLevelCoordinator::synchronizeLevels() {
  // Synchronize state between levels
  // This is where micro/macro state synchronization would happen

  // For now, this is a placeholder
  // In a full implementation, this would:
  // 1. Aggregate microscopic states to macroscopic
  // 2. Disaggregate macroscopic states to microscopic
  // 3. Update control level based on both

  // Example synchronization logic:
  // - Microscopic → Macroscopic: Aggregate vehicle positions to density
  // - Macroscopic → Microscopic: Generate vehicles from density
  // - Control → Both: Apply control signals (speed limits, signals)
}

void MultiLevelCoordinator::reset() {
  m_current_time = 0.0;
  m_step_count = 0;
  m_agent_levels.clear();

  if (m_engine) {
    m_engine->reset();
  }
}

bool MultiLevelCoordinator::shouldUpdateLevel(
    const agents::LevelIdentifier &level) {
  auto it = m_levels.find(level);
  if (it == m_levels.end()) {
    return false;
  }

  const auto &config = it->second;

  // Update if step count is divisible by frequency
  return (m_step_count % config.update_frequency) == 0;
}

void MultiLevelCoordinator::updateLevel(const agents::LevelIdentifier &level) {
  // Update specific level
  // For now, we use the main engine which updates all levels
  // In a full implementation, this would update only the specified level

  std::cout << "Updating level: " << level.getName() << " (step "
            << m_step_count << ")" << std::endl;

  // TODO: Level-specific updates
  // This would involve:
  // 1. Filtering agents by level
  // 2. Running perception/decision/reaction for that level only
  // 3. Using level-specific time step
}

} // namespace simulation
} // namespace kernel
} // namespace jamfree
