#include "../include/agents/VehiclePublicLocalStateMicro.h"

namespace jamfree {
namespace microscopic {
namespace agents {

VehiclePublicLocalStateMicro::VehiclePublicLocalStateMicro()
    : m_position(0, 0), m_heading(0.0), m_speed(0.0), m_acceleration(0.0),
      m_current_lane(nullptr), m_lane_position(0.0), m_lane_index(0),
      m_length(5.0), // Default car length
      m_width(2.0),  // Default car width
      m_height(1.5), // Default car height
      m_active(true) {}

std::shared_ptr<kernel::agents::ILocalState>
VehiclePublicLocalStateMicro::clone() const {
  auto cloned = std::make_shared<VehiclePublicLocalStateMicro>();

  cloned->m_position = m_position;
  cloned->m_heading = m_heading;
  cloned->m_speed = m_speed;
  cloned->m_acceleration = m_acceleration;
  cloned->m_current_lane = m_current_lane;
  cloned->m_lane_position = m_lane_position;
  cloned->m_lane_index = m_lane_index;
  cloned->m_length = m_length;
  cloned->m_width = m_width;
  cloned->m_height = m_height;
  cloned->m_active = m_active;

  return cloned;
}

} // namespace agents
} // namespace microscopic
} // namespace jamfree
