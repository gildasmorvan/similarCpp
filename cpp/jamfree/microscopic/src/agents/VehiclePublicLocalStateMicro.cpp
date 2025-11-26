#include "../../include/agents/VehiclePublicLocalStateMicro.h"
#include "../../../../microkernel/include/AgentCategory.h"
#include "../../../kernel/include/agents/VehicleAgent.h"

namespace jamfree {
namespace microscopic {
namespace agents {

VehiclePublicLocalStateMicro::VehiclePublicLocalStateMicro(
    const std::string &ownerId)
    : m_ownerId(ownerId), m_heading(0), m_speed(0), m_acceleration(0),
      m_current_lane(nullptr), m_lane_position(0), m_lane_index(0), m_length(0),
      m_width(0), m_height(0), m_active(true) {}

std::shared_ptr<kernel::agents::ILocalState>
VehiclePublicLocalStateMicro::clone() const {
  auto cloned = std::make_shared<VehiclePublicLocalStateMicro>(m_ownerId);
  cloned->setPosition(m_position);
  cloned->setHeading(m_heading);
  cloned->setSpeed(m_speed);
  cloned->setAcceleration(m_acceleration);
  cloned->setCurrentLane(m_current_lane);
  cloned->setLanePosition(m_lane_position);
  cloned->setLaneIndex(m_lane_index);
  cloned->setLength(m_length);
  cloned->setWidth(m_width);
  cloned->setHeight(m_height);
  cloned->setActive(m_active);
  return cloned;
}

kernel::AgentCategory VehiclePublicLocalStateMicro::getCategoryOfAgent() const {
  return kernel::AgentCategory("Vehicle");
}

bool VehiclePublicLocalStateMicro::isOwnedBy(
    const kernel::agents::IAgent &agent) const {
  const auto *vehicleAgent =
      dynamic_cast<const kernel::agents::VehicleAgent *>(&agent);
  if (vehicleAgent) {
    return vehicleAgent->getId() == m_ownerId;
  }
  return false;
}

} // namespace agents
} // namespace microscopic
} // namespace jamfree
