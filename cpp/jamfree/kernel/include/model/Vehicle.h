#ifndef JAMFREE_KERNEL_MODEL_VEHICLE_H
#define JAMFREE_KERNEL_MODEL_VEHICLE_H

#include "Lane.h"
#include "Point2D.h"
#include <memory>
#include <string>

namespace jamfree {
namespace kernel {
namespace model {

/**
 * @brief Represents a vehicle in the traffic simulation.
 *
 * Stores vehicle state (position, speed, acceleration) and properties
 * (length, max speed, max acceleration/deceleration).
 */
class Vehicle {
public:
  /**
   * @brief Constructor.
   *
   * @param id Unique identifier
   * @param length Vehicle length (meters)
   * @param max_speed Maximum speed (m/s)
   * @param max_accel Maximum acceleration (m/s²)
   * @param max_decel Maximum deceleration (m/s²)
   */
  Vehicle(const std::string &id, double length = 5.0,
          double max_speed = 55.0, // ~200 km/h
          double max_accel = 3.0, double max_decel = 6.0)
      : m_id(id), m_length(length), m_width(2.0) // Default width
        ,
        m_max_speed(max_speed), m_max_accel(max_accel), m_max_decel(max_decel),
        m_position(0.0, 0.0), m_speed(0.0), m_acceleration(0.0), m_heading(0.0),
        m_lane_position(0.0), m_current_lane(nullptr) {}

  // Getters - Identity
  const std::string &getId() const { return m_id; }

  // Getters - Properties
  double getLength() const { return m_length; }
  double getWidth() const { return m_width; }
  double getMaxSpeed() const { return m_max_speed; }
  double getMaxAccel() const { return m_max_accel; }
  double getMaxDecel() const { return m_max_decel; }

  // Getters - State
  const Point2D &getPosition() const { return m_position; }
  double getSpeed() const { return m_speed; }
  double getAcceleration() const { return m_acceleration; }
  double getHeading() const { return m_heading; }
  double getLanePosition() const { return m_lane_position; }
  std::shared_ptr<Lane> getCurrentLane() const { return m_current_lane; }

  // Setters - Properties
  void setLength(double length) { m_length = length; }
  void setWidth(double width) { m_width = width; }
  void setMaxSpeed(double max_speed) { m_max_speed = max_speed; }
  void setMaxAccel(double max_accel) { m_max_accel = max_accel; }
  void setMaxDecel(double max_decel) { m_max_decel = max_decel; }

  // Setters - State
  void setPosition(const Point2D &position) { m_position = position; }
  void setSpeed(double speed) { m_speed = std::max(0.0, speed); }
  void setAcceleration(double acceleration) { m_acceleration = acceleration; }
  void setHeading(double heading) { m_heading = heading; }
  void setLanePosition(double position) { m_lane_position = position; }
  void setCurrentLane(std::shared_ptr<Lane> lane) { m_current_lane = lane; }

  // Convenience methods (aliases)
  Lane *getLane() const { return m_current_lane.get(); }
  void setLane(Lane *lane) {
    // Note: This assumes lane is managed elsewhere
    // For proper ownership, use setCurrentLane with shared_ptr
    m_current_lane.reset(lane, [](Lane *) {}); // Non-owning shared_ptr
  }

  /**
   * @brief Update vehicle state for one time step.
   *
   * @param dt Time step (seconds)
   * @param acceleration Desired acceleration (m/s²)
   */
  void update(double dt, double acceleration) {
    // Clamp acceleration to vehicle limits
    acceleration = std::max(-m_max_decel, std::min(m_max_accel, acceleration));
    m_acceleration = acceleration;

    // Update speed
    m_speed += acceleration * dt;
    m_speed = std::max(0.0, std::min(m_max_speed, m_speed));

    // Update position along lane
    m_lane_position += m_speed * dt;

    // Update 2D position if we have a lane
    if (m_current_lane && m_current_lane->getParentRoad()) {
      m_position = m_current_lane->getPositionAt(m_lane_position);
      m_heading = m_current_lane->getHeadingAt(m_lane_position);
    }
  }

  /**
   * @brief Get front position of vehicle.
   *
   * @return Position of vehicle front
   */
  Point2D getFrontPosition() const {
    return m_position + Point2D(m_length * std::cos(m_heading),
                                m_length * std::sin(m_heading));
  }

  /**
   * @brief Get rear position of vehicle.
   *
   * @return Position of vehicle rear
   */
  Point2D getRearPosition() const { return m_position; }

  /**
   * @brief Get distance to vehicle ahead.
   *
   * @param leader Vehicle ahead
   * @return Net gap (meters), negative if overlapping
   */
  double getGapTo(const Vehicle &leader) const {
    // Gap = leader rear position - this front position
    double leader_rear = leader.getLanePosition();
    double this_front = m_lane_position + m_length;
    return leader_rear - this_front;
  }

  /**
   * @brief Get relative speed to vehicle ahead.
   *
   * @param leader Vehicle ahead
   * @return Speed difference (m/s), positive if approaching
   */
  double getRelativeSpeedTo(const Vehicle &leader) const {
    return m_speed - leader.getSpeed();
  }

  /**
   * @brief Check if vehicle is stopped.
   *
   * @param threshold Speed threshold (m/s)
   * @return True if speed below threshold
   */
  bool isStopped(double threshold = 0.1) const { return m_speed < threshold; }

private:
  // Identity
  std::string m_id;

  // Properties (constant)
  double m_length;
  double m_width;
  double m_max_speed;
  double m_max_accel;
  double m_max_decel;

  // State (dynamic)
  Point2D m_position;     ///< 2D position
  double m_speed;         ///< Current speed (m/s)
  double m_acceleration;  ///< Current acceleration (m/s²)
  double m_heading;       ///< Direction (radians)
  double m_lane_position; ///< Position along current lane (meters)
  std::shared_ptr<Lane> m_current_lane; ///< Current lane
};

} // namespace model
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_MODEL_VEHICLE_H
