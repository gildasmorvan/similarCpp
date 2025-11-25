#include "../include/agents/VehiclePublicLocalStateMacro.h"

namespace jamfree {
namespace macroscopic {
namespace agents {

VehiclePublicLocalStateMacro::VehiclePublicLocalStateMacro()
    : m_density(0.0), m_flow(0.0), m_average_speed(0.0),
      m_current_lane(nullptr), m_cell_position(0.0), m_cell_index(0),
      m_active(true) {}

std::shared_ptr<kernel::agents::ILocalState>
VehiclePublicLocalStateMacro::clone() const {
  auto cloned = std::make_shared<VehiclePublicLocalStateMacro>();

  cloned->m_density = m_density;
  cloned->m_flow = m_flow;
  cloned->m_average_speed = m_average_speed;
  cloned->m_current_lane = m_current_lane;
  cloned->m_cell_position = m_cell_position;
  cloned->m_cell_index = m_cell_index;
  cloned->m_active = m_active;

  return cloned;
}

} // namespace agents
} // namespace macroscopic
} // namespace jamfree
