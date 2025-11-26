#include "../include/agents/VehiclePublicLocalStateMacro.h"
#include "../../../../microkernel/include/AgentCategory.h"
#include "../../../kernel/include/agents/VehicleAgent.h"

namespace jamfree {
namespace macroscopic {
namespace agents {

VehiclePublicLocalStateMacro::VehiclePublicLocalStateMacro(
    const std::string &ownerId)
    : m_ownerId(ownerId), m_density(0.0), m_flow(0.0), m_average_speed(0.0),
      m_current_lane(nullptr), m_cell_position(0.0), m_cell_index(0),
      m_active(true) {}

std::shared_ptr<kernel::agents::ILocalState>
VehiclePublicLocalStateMacro::clone() const {
  auto cloned = std::make_shared<VehiclePublicLocalStateMacro>(m_ownerId);
  cloned->setDensity(m_density);
  cloned->setFlow(m_flow);
  cloned->setAverageSpeed(m_average_speed);
  cloned->setCurrentLane(m_current_lane);
  cloned->setCellPosition(m_cell_position);
  cloned->setCellIndex(m_cell_index);
  cloned->setActive(m_active);
  return cloned;
}

kernel::AgentCategory VehiclePublicLocalStateMacro::getCategoryOfAgent() const {
  return kernel::AgentCategory("Vehicle");
}

bool VehiclePublicLocalStateMacro::isOwnedBy(
    const kernel::agents::IAgent &agent) const {
  const auto *vehicleAgent =
      dynamic_cast<const kernel::agents::VehicleAgent *>(&agent);
  if (vehicleAgent) {
    return vehicleAgent->getId() == m_ownerId;
  }
  return false;
}

} // namespace agents
} // namespace macroscopic
} // namespace jamfree
