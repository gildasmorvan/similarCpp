#include "../../include/simulation/TrafficSimulationModel.h"
#include "../../../../microkernel/include/libs/generic/EmptyLocalStateOfEnvironment.h"

namespace jamfree {
namespace kernel {
namespace simulation {

using namespace fr::univ_artois::lgi2a::similar::microkernel;

TrafficSimulationModel::TrafficSimulationModel(
    const SimulationTimeStamp &initialTime,
    std::shared_ptr<jamfree::realdata::osm::RoadNetwork> network,
    const std::vector<std::shared_ptr<jamfree::kernel::agents::VehicleAgent>>
        &vehicles)
    : libs::abstractimpl::AbstractSimulationModel(initialTime),
      m_network(network), m_vehicles(vehicles) {}

bool TrafficSimulationModel::isFinalTimeOrAfter(
    const SimulationTimeStamp &currentTime,
    const ISimulationEngine &engine) const {
  // Simple check: if current time >= 10000 (arbitrary end time for now)
  // In a real simulation, this would be configurable
  return currentTime.getIdentifier() >= 10000;
}

std::vector<std::shared_ptr<levels::ILevel>>
TrafficSimulationModel::generateLevels(const SimulationTimeStamp &initialTime) {
  std::vector<std::shared_ptr<levels::ILevel>> levels;

  // Create the microscopic level
  m_microscopic_level =
      std::shared_ptr<TrafficLevel>(new TrafficLevel(initialTime));
  levels.push_back(m_microscopic_level);

  // Store in map for getLevels()
  m_levels[m_microscopic_level->getIdentifier()] = m_microscopic_level;

  return levels;
}

ISimulationModel::EnvironmentInitializationData
TrafficSimulationModel::generateEnvironment(
    const SimulationTimeStamp &initialTime,
    const std::map<LevelIdentifier, std::shared_ptr<levels::ILevel>> &levels) {
  // We don't have a separate Environment class yet, so we return empty data
  // But we need to initialize the level's environment state

  // Note: The engine calls this method. We can use this opportunity to set up
  // the level's environment state if needed.

  auto levelId = m_microscopic_level->getIdentifier();
  auto consistentState = m_microscopic_level->getLastConsistentState();

  // Set environment state (dummy for now)
  consistentState->setPublicLocalStateOfEnvironment(
      std::make_shared<libs::generic::EmptyLocalStateOfEnvironment>(levelId));

  // Return empty initialization data (nullptr environment)
  return EnvironmentInitializationData(nullptr);
}

ISimulationModel::AgentInitializationData
TrafficSimulationModel::generateAgents(
    const SimulationTimeStamp &initialTime,
    const std::map<LevelIdentifier, std::shared_ptr<levels::ILevel>> &levels) {
  AgentInitializationData data;

  for (const auto &vehicle : m_vehicles) {
    data.getAgents().insert(vehicle);
  }

  return data;
}

std::map<LevelIdentifier, std::shared_ptr<levels::ILevel>>
TrafficSimulationModel::getLevels() const {
  return m_levels;
}

} // namespace simulation
} // namespace kernel
} // namespace jamfree
