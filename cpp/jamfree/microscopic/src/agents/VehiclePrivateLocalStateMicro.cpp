#include "../include/agents/VehiclePrivateLocalStateMicro.h"

namespace jamfree {
namespace microscopic {
namespace agents {

VehiclePrivateLocalStateMicro::VehiclePrivateLocalStateMicro()
    // IDM defaults
    : m_desired_speed(33.3),           // 120 km/h
      m_time_headway(1.5),             // 1.5 s
      m_min_gap(2.0),                  // 2 m
      m_max_acceleration(2.0),         // 2 m/s²
      m_comfortable_deceleration(3.0), // 3 m/s²
      m_acceleration_exponent(4.0),    // delta = 4
      // MOBIL defaults
      m_politeness(0.5),            // Moderately polite
      m_lane_change_threshold(0.1), // 0.1 m/s²
      m_max_safe_deceleration(4.0), // 4 m/s²
      m_right_lane_bias(0.3),       // 0.3 m/s² (keep-right)
      // Route defaults
      m_current_route_index(0), m_destination(""),
      // Driver characteristics
      m_reaction_time(1.0),   // 1 s
      m_aggressiveness(0.5) { // Moderate
}

std::shared_ptr<kernel::agents::ILocalState>
VehiclePrivateLocalStateMicro::clone() const {
  auto cloned = std::make_shared<VehiclePrivateLocalStateMicro>();

  // IDM parameters
  cloned->m_desired_speed = m_desired_speed;
  cloned->m_time_headway = m_time_headway;
  cloned->m_min_gap = m_min_gap;
  cloned->m_max_acceleration = m_max_acceleration;
  cloned->m_comfortable_deceleration = m_comfortable_deceleration;
  cloned->m_acceleration_exponent = m_acceleration_exponent;

  // MOBIL parameters
  cloned->m_politeness = m_politeness;
  cloned->m_lane_change_threshold = m_lane_change_threshold;
  cloned->m_max_safe_deceleration = m_max_safe_deceleration;
  cloned->m_right_lane_bias = m_right_lane_bias;

  // Route information
  cloned->m_route = m_route;
  cloned->m_current_route_index = m_current_route_index;
  cloned->m_destination = m_destination;

  // Driver characteristics
  cloned->m_reaction_time = m_reaction_time;
  cloned->m_aggressiveness = m_aggressiveness;

  return cloned;
}

} // namespace agents
} // namespace microscopic
} // namespace jamfree
