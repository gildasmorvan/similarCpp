#ifndef JAMFREE_KERNEL_SIMULATION_TRAFFIC_SIMULATION_MODEL_H
#define JAMFREE_KERNEL_SIMULATION_TRAFFIC_SIMULATION_MODEL_H

#include "../../../../microkernel/include/libs/abstractimpl/AbstractSimulationModel.h"
#include "../../../realdata/include/OSMParser.h"
#include "../agents/VehicleAgent.h"
#include "TrafficLevel.h"
#include <memory>
#include <vector>

namespace jamfree {
namespace kernel {
namespace simulation {

/**
 * @brief Traffic simulation model.
 *
 * Orchestrates the simulation by managing the environment (road network),
 * agents (vehicles), and the simulation level.
 */
class TrafficSimulationModel
    : public fr::univ_artois::lgi2a::similar::microkernel::libs::abstractimpl::
          AbstractSimulationModel {
public:
  /**
   * @brief Constructor.
   * @param initialTime Initial simulation time
   * @param network Road network
   * @param vehicles List of vehicle agents
   */
  TrafficSimulationModel(
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &initialTime,
      std::shared_ptr<jamfree::realdata::osm::RoadNetwork> network,
      const std::vector<std::shared_ptr<jamfree::kernel::agents::VehicleAgent>>
          &vehicles);

  /**
   * @brief Destructor.
   */
  virtual ~TrafficSimulationModel() = default;

  // ISimulationModel implementation
  bool isFinalTimeOrAfter(
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &currentTime,
      const fr::univ_artois::lgi2a::similar::microkernel::ISimulationEngine
          &engine) const override;

  std::vector<std::shared_ptr<
      fr::univ_artois::lgi2a::similar::microkernel::levels::ILevel>>
  generateLevels(
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &initialTime) override;

  fr::univ_artois::lgi2a::similar::microkernel::ISimulationModel::
      EnvironmentInitializationData
      generateEnvironment(
          const fr::univ_artois::lgi2a::similar::microkernel::
              SimulationTimeStamp &initialTime,
          const std::map<
              fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier,
              std::shared_ptr<
                  fr::univ_artois::lgi2a::similar::microkernel::levels::ILevel>>
              &levels) override;

  fr::univ_artois::lgi2a::similar::microkernel::ISimulationModel::
      AgentInitializationData
      generateAgents(
          const fr::univ_artois::lgi2a::similar::microkernel::
              SimulationTimeStamp &initialTime,
          const std::map<
              fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier,
              std::shared_ptr<
                  fr::univ_artois::lgi2a::similar::microkernel::levels::ILevel>>
              &levels) override;

  std::map<fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier,
           std::shared_ptr<
               fr::univ_artois::lgi2a::similar::microkernel::levels::ILevel>>
  getLevels() const;

private:
  std::shared_ptr<jamfree::realdata::osm::RoadNetwork> m_network;
  std::vector<std::shared_ptr<jamfree::kernel::agents::VehicleAgent>>
      m_vehicles;
  std::shared_ptr<TrafficLevel> m_microscopic_level;

  // We need to store levels in a map for getLevels()
  std::map<fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier,
           std::shared_ptr<
               fr::univ_artois::lgi2a::similar::microkernel::levels::ILevel>>
      m_levels;
};

} // namespace simulation
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_SIMULATION_TRAFFIC_SIMULATION_MODEL_H
