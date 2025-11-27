#ifndef JAMFREE_MICROSCOPIC_PERCEPTION_VEHICLE_PERCEPTION_MODEL_MICRO_H
#define JAMFREE_MICROSCOPIC_PERCEPTION_VEHICLE_PERCEPTION_MODEL_MICRO_H

#include "../../../kernel/include/agents/Interfaces.h"
#include "../agents/VehiclePerceivedDataMicro.h"
#include "../agents/VehiclePrivateLocalStateMicro.h"
#include "../agents/VehiclePublicLocalStateMicro.h"
#include <memory>

namespace jamfree {
namespace microscopic {
namespace perception {

/**
 * @brief Perception model for vehicles in the microscopic level.
 *
 * This model perceives the environment and fills the perceived data
 * with information about:
 * - Leader and follower vehicles
 * - Adjacent lanes and their vehicles
 * - Lane end proximity
 * - Routing information
 * - Speed limits
 */
class VehiclePerceptionModelMicro : public kernel::agents::IPerceptionModel {
public:
  /**
   * @brief Constructor.
   * @param perceptionRange Maximum perception range (meters)
   */
  explicit VehiclePerceptionModelMicro(double perceptionRange = 150.0);

  /**
   * @brief Perceive environment from agent's perspective.
   *
   * @param timeLowerBound Lower bound of time interval
   * @param timeUpperBound Upper bound of time interval
   * @param publicLocalStates Map of public local states for all levels
   * @param privateLocalState Private local state of agent
   * @param dynamicStates Dynamic state map
   * @return Perceived data
   */
  std::shared_ptr<kernel::agents::IPerceivedData>
  perceive(const kernel::agents::SimulationTimeStamp &timeLowerBound,
           const kernel::agents::SimulationTimeStamp &timeUpperBound,
           const std::map<kernel::agents::LevelIdentifier,
                          std::shared_ptr<kernel::agents::ILocalState>>
               &publicLocalStates,
           std::shared_ptr<kernel::agents::ILocalState> privateLocalState,
           std::shared_ptr<kernel::agents::IPublicDynamicStateMap>
               dynamicStates) override;

  kernel::agents::LevelIdentifier getLevel() const override;

private:
  double m_perception_range; // Maximum perception distance (m)

  /**
   * @brief Perceive leader and follower in current lane.
   */
  void
  perceiveCurrentLane(const agents::VehiclePublicLocalStateMicro &publicState,
                      agents::VehiclePerceivedDataMicro &perceivedData);

  /**
   * @brief Perceive adjacent lanes (left and right).
   */
  void
  perceiveAdjacentLanes(const agents::VehiclePublicLocalStateMicro &publicState,
                        agents::VehiclePerceivedDataMicro &perceivedData);

  /**
   * @brief Perceive lane end and next road.
   */
  void
  perceiveLaneEnd(const agents::VehiclePublicLocalStateMicro &publicState,
                  const agents::VehiclePrivateLocalStateMicro &privateState,
                  agents::VehiclePerceivedDataMicro &perceivedData);

  /**
   * @brief Perceive speed limit.
   */
  void
  perceiveSpeedLimit(const agents::VehiclePublicLocalStateMicro &publicState,
                     agents::VehiclePerceivedDataMicro &perceivedData);

  /**
   * @brief Find leader vehicle in a lane.
   * @param lane Lane to search
   * @param position Current position along lane
   * @param maxRange Maximum search range
   * @return Leader vehicle and gap, or nullptr if none found
   */
  std::pair<kernel::model::Vehicle *, double>
  findLeader(kernel::model::Lane *lane, double position, double maxRange);

  /**
   * @brief Find follower vehicle in a lane.
   * @param lane Lane to search
   * @param position Current position along lane
   * @param maxRange Maximum search range
   * @return Follower vehicle and gap, or nullptr if none found
   */
  std::pair<kernel::model::Vehicle *, double>
  findFollower(kernel::model::Lane *lane, double position, double maxRange);
};

} // namespace perception
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_PERCEPTION_VEHICLE_PERCEPTION_MODEL_MICRO_H
