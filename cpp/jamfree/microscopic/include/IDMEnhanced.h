#ifndef JAMFREE_MICROSCOPIC_MODELS_IDM_ENHANCED_H
#define JAMFREE_MICROSCOPIC_MODELS_IDM_ENHANCED_H

#include "../../kernel/include/model/TrafficControl.h"
#include "../../kernel/include/routing/Router.h"
#include "../../realdata/include/TrafficDataSource.h"
#include "IDM.h"

namespace jamfree {
namespace microscopic {
namespace models {

/**
 * @brief Enhanced IDM with traffic control awareness.
 *
 * Extends standard IDM to respect:
 * - Traffic lights
 * - Stop signs
 * - Speed limits
 * - Route following
 * - Real-time traffic data
 */
class IDMEnhanced : public IDM {
public:
  IDMEnhanced(double desired_speed = 33.3, double time_headway = 1.5,
              double min_gap = 2.0, double max_accel = 1.0,
              double comfortable_decel = 1.5, double accel_exponent = 4.0)
      : IDM(desired_speed, time_headway, min_gap, max_accel, comfortable_decel,
            accel_exponent),
        m_traffic_control_manager(nullptr), m_current_route(nullptr),
        m_route_index(0) {}

  /**
   * @brief Set traffic control manager.
   */
  void setTrafficControlManager(kernel::model::TrafficControlManager *manager) {
    m_traffic_control_manager = manager;
  }

  /**
   * @brief Set current route.
   */
  void setRoute(const kernel::routing::Route *route) {
    m_current_route = route;
    m_route_index = 0;
  }

  /**
   * @brief Calculate acceleration with traffic control awareness.
   *
   * @param vehicle The vehicle
   * @param leader Vehicle ahead (nullptr if no leader)
   * @return Acceleration in m/s²
   */
  double calculateAcceleration(
      const kernel::model::Vehicle &vehicle,
      const kernel::model::Vehicle *leader = nullptr) const override {

    // Get base IDM acceleration
    double idm_accel = IDM::calculateAcceleration(vehicle, leader);

    // Check for traffic controls
    if (m_traffic_control_manager) {
      double control_accel = calculateTrafficControlAccel(vehicle);

      // Use most restrictive acceleration
      if (control_accel < idm_accel) {
        return control_accel;
      }
    }

    // Check route compliance
    if (m_current_route && !m_current_route->isEmpty()) {
      double route_accel = calculateRouteAccel(vehicle);
      if (route_accel < idm_accel) {
        return route_accel;
      }
    }

    return idm_accel;
  }

  /**
   * @brief Update route progress.
   *
   * Call this when vehicle changes roads.
   */
  void updateRouteProgress(const kernel::model::Road *current_road) {
    if (!m_current_route || m_current_route->isEmpty()) {
      return;
    }

    // Find current road in route
    for (size_t i = m_route_index; i < m_current_route->roads.size(); ++i) {
      if (m_current_route->roads[i].get() == current_road) {
        m_route_index = i;
        return;
      }
    }
  }

  /**
   * @brief Check if route is complete.
   */
  bool isRouteComplete() const {
    if (!m_current_route || m_current_route->isEmpty()) {
      return true;
    }
    return m_route_index >= m_current_route->roads.size() - 1;
  }

  /**
   * @brief Get next road in route.
   */
  kernel::model::Road *getNextRoad() const {
    if (!m_current_route || m_current_route->isEmpty()) {
      return nullptr;
    }

    if (m_route_index + 1 < m_current_route->roads.size()) {
      return m_current_route->roads[m_route_index + 1].get();
    }

    return nullptr;
  }

private:
  kernel::model::TrafficControlManager *m_traffic_control_manager;
  const kernel::routing::Route *m_current_route;
  size_t m_route_index;

  /**
   * @brief Calculate acceleration for traffic controls.
   */
  double
  calculateTrafficControlAccel(const kernel::model::Vehicle &vehicle) const {

    auto position = vehicle.getPosition();
    double speed = vehicle.getSpeed();

    // Look ahead for traffic controls
    double look_ahead = 50.0; // meters
    auto controls =
        m_traffic_control_manager->getControlsNear(position, look_ahead);

    double min_accel = std::numeric_limits<double>::max();

    for (const auto &control : controls) {
      if (control->shouldStop(position, speed)) {
        // Calculate deceleration needed to stop
        double distance = position.distanceTo(control->getPosition());
        distance -= control->getStoppingDistance();

        if (distance > 0) {
          // a = -v² / (2 * d)
          double required_decel = -(speed * speed) / (2.0 * distance);

          // Add safety margin
          required_decel *= 1.2;

          // Clamp to comfortable deceleration
          required_decel =
              std::max(required_decel, -getComfortableDecel() * 1.5);

          min_accel = std::min(min_accel, required_decel);
        } else {
          // Too close, emergency brake
          min_accel = -getComfortableDecel() * 2.0;
        }
      }
    }

    return (min_accel == std::numeric_limits<double>::max()) ? getMaxAccel()
                                                             : min_accel;
  }

  /**
   * @brief Calculate acceleration for route following.
   */
  double calculateRouteAccel(const kernel::model::Vehicle &vehicle) const {

    // Check if approaching end of current road
    auto lane = vehicle.getCurrentLane();
    if (!lane) {
      return getMaxAccel();
    }

    double position = vehicle.getLanePosition();
    double lane_length = lane->getLength();
    double distance_to_end = lane_length - position;

    // If approaching end and need to change roads
    if (distance_to_end < 50.0) {
      auto next_road = getNextRoad();

      if (!next_road) {
        // End of route, slow down
        double speed = vehicle.getSpeed();
        if (distance_to_end > 0 && speed > 0) {
          double required_decel = -(speed * speed) / (2.0 * distance_to_end);
          return std::max(required_decel, -getComfortableDecel());
        }
      }
    }

    return getMaxAccel();
  }
};

} // namespace models
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_MODELS_IDM_ENHANCED_H
