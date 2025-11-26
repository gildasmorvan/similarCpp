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
      m_network(network), m_vehicles(vehicles) {

  // Create the microscopic level
  m_microscopic_level = std::make_shared<TrafficLevel>(initialTime);

  // Initialize agents in the level
  // In SIMILAR, agents are usually added to the level via the environment or
  // directly to the level's state. But here we are setting up the simulation
  // model which the engine uses.

  // We need to ensure agents are "in" the level.
  // VehicleAgent::setPublicLocalState/setPrivateLocalState does this by calling
  // includeNewLevel. But that requires the agent to be initialized.

  // For this integration, we assume agents are already configured with their
  // local states (which they are in the current Python code, mostly). However,
  // we might need to explicitly register them with the level if the level needs
  // to know about them. The Level's state (ConsistentPublicLocalDynamicState)
  // maintains a set of agent public states.

  auto levelId = m_microscopic_level->getIdentifier();
  auto consistentState = m_microscopic_level->getLastConsistentState();

  // Set environment state (dummy for now, or could wrap RoadNetwork)
  // We use EmptyLocalStateOfEnvironment for now as RoadNetwork is static/global
  // in this context
  consistentState->setPublicLocalStateOfEnvironment(
      std::make_shared<libs::generic::EmptyLocalStateOfEnvironment>(levelId));

  for (const auto &vehicle : m_vehicles) {
    // Ensure vehicle has this level
    if (vehicle->hasLevel(levelId)) {
      auto pubState = vehicle->getPublicLocalState(levelId);
      // Cast to ILocalStateOfAgent (ILocalState inherits from it? No,
      // ILocalState IS ILocalStateOfAgent in JamFree?) Let's check Interfaces.h
      // In JamFree, ILocalState is an alias or inherits.
      // Actually, in VehicleAgent.h:
      // std::shared_ptr<ILocalState> getPublicLocalState(...)
      // And ILocalState is likely jamfree::kernel::agents::ILocalState

      // We need to add it to the level's state.
      // consistentState->addPublicLocalStateOfAgent(pubState);
      // But pubState needs to be castable to
      // microkernel::agents::ILocalStateOfAgent
    }
  }

  m_levels[levelId] = m_microscopic_level;
}

std::shared_ptr<environment::IEnvironment>
TrafficSimulationModel::getEnvironment() const {
  // We don't have a separate Environment class yet, return nullptr or a dummy
  // if needed. The engine might not strictly require it if we don't use
  // environment-based influences.
  return nullptr;
}

std::map<LevelIdentifier, std::shared_ptr<levels::ILevel>>
TrafficSimulationModel::getLevels() const {
  return m_levels;
}

} // namespace simulation
} // namespace kernel
} // namespace jamfree
