#include "../../include/agents/VehiclePrivateLocalStateMicro.h"
#include "../../../../microkernel/include/AgentCategory.h"
#include "../../../kernel/include/agents/VehicleAgent.h"

namespace jamfree {
namespace microscopic {
namespace agents {

VehiclePrivateLocalStateMicro::VehiclePrivateLocalStateMicro(
    const std::string &ownerId)
    : m_ownerId(ownerId), m_level("microscopic"),
      m_desired_speed(33.33), // 120 km/h
      m_time_headway(1.5), m_min_gap(2.0), m_max_acceleration(1.0),
      m_comfortable_deceleration(2.0), m_acceleration_exponent(4.0),
      m_politeness(0.1), m_lane_change_threshold(0.2),
      m_max_safe_deceleration(4.0), m_right_lane_bias(0.1),
      m_current_route_index(0), m_reaction_time(1.0), m_aggressiveness(0.5) {}

std::shared_ptr<
    fr::univ_artois::lgi2a::similar::microkernel::ILocalState>
VehiclePrivateLocalStateMicro::clone() const {
  auto cloned = std::shared_ptr<VehiclePrivateLocalStateMicro>(
      new VehiclePrivateLocalStateMicro(m_ownerId));
  cloned->setDesiredSpeed(m_desired_speed);
  cloned->setTimeHeadway(m_time_headway);
  cloned->setMinGap(m_min_gap);
  cloned->setMaxAcceleration(m_max_acceleration);
  cloned->setComfortableDeceleration(m_comfortable_deceleration);
  cloned->setAccelerationExponent(m_acceleration_exponent);
  cloned->setPoliteness(m_politeness);
  cloned->setLaneChangeThreshold(m_lane_change_threshold);
  cloned->setMaxSafeDeceleration(m_max_safe_deceleration);
  cloned->setRightLaneBias(m_right_lane_bias);
  cloned->setRoute(m_route);
  cloned->setCurrentRouteIndex(m_current_route_index);
  cloned->setDestination(m_destination);
  cloned->setReactionTime(m_reaction_time);
  cloned->setAggressiveness(m_aggressiveness);
  return cloned;
}

kernel::agents::AgentCategory
VehiclePrivateLocalStateMicro::getCategoryOfAgent() const {
  return kernel::agents::AgentCategory("Vehicle");
}

bool VehiclePrivateLocalStateMicro::isOwnedBy(
    const kernel::agents::IAgent &agent) const {
  const auto *vehicleAgent =
      dynamic_cast<const kernel::agents::VehicleAgent *>(&agent);
  if (vehicleAgent) {
    return vehicleAgent->getId() == m_ownerId;
  }
  return false;
}

kernel::agents::LevelIdentifier
VehiclePrivateLocalStateMicro::getLevel() const {
  return m_level;
}

} // namespace agents
} // namespace microscopic
} // namespace jamfree
