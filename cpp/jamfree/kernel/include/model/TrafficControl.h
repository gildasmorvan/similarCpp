#ifndef JAMFREE_KERNEL_MODEL_TRAFFIC_CONTROL_H
#define JAMFREE_KERNEL_MODEL_TRAFFIC_CONTROL_H

#include "Point2D.h"
#include <memory>
#include <string>
#include <vector>

namespace jamfree {
namespace kernel {
namespace model {

/**
 * @brief Traffic control device types.
 */
enum class TrafficControlType {
  STOP_SIGN,
  TRAFFIC_LIGHT,
  YIELD_SIGN,
  SPEED_LIMIT,
  PEDESTRIAN_CROSSING,
  RAILWAY_CROSSING
};

/**
 * @brief Traffic light phase.
 */
enum class LightPhase {
  RED,
  YELLOW,
  GREEN,
  RED_YELLOW // Some countries use this
};

/**
 * @brief Base class for traffic control devices.
 */
class TrafficControl {
public:
  TrafficControl(const std::string &id, TrafficControlType type,
                 const Point2D &position)
      : m_id(id), m_type(type), m_position(position), m_active(true) {}

  virtual ~TrafficControl() = default;

  // Getters
  const std::string &getId() const { return m_id; }
  TrafficControlType getType() const { return m_type; }
  const Point2D &getPosition() const { return m_position; }
  bool isActive() const { return m_active; }

  // Setters
  void setActive(bool active) { m_active = active; }

  /**
   * @brief Check if vehicle should stop.
   *
   * @param vehicle_position Vehicle position
   * @param vehicle_speed Vehicle speed
   * @return True if vehicle must stop
   */
  virtual bool shouldStop(const Point2D &vehicle_position,
                          double vehicle_speed) const = 0;

  /**
   * @brief Get stopping distance.
   *
   * @return Distance before control where vehicle should stop (meters)
   */
  virtual double getStoppingDistance() const { return 5.0; }

  /**
   * @brief Update control state (for time-dependent controls like lights).
   *
   * @param dt Time step (seconds)
   */
  virtual void update(double dt) {}

protected:
  std::string m_id;
  TrafficControlType m_type;
  Point2D m_position;
  bool m_active;
};

/**
 * @brief Stop sign.
 */
class StopSign : public TrafficControl {
public:
  StopSign(const std::string &id, const Point2D &position)
      : TrafficControl(id, TrafficControlType::STOP_SIGN, position) {}

  bool shouldStop(const Point2D &vehicle_position,
                  double vehicle_speed) const override {
    if (!m_active)
      return false;

    // Vehicle must stop if within stopping distance and hasn't stopped yet
    double distance = vehicle_position.distanceTo(m_position);
    return distance < getStoppingDistance() && vehicle_speed > 0.1;
  }

  double getStoppingDistance() const override { return 10.0; }
};

/**
 * @brief Traffic light with configurable phases.
 */
class TrafficLight : public TrafficControl {
public:
  /**
   * @brief Phase configuration.
   */
  struct PhaseConfig {
    LightPhase phase;
    double duration; // seconds
  };

  TrafficLight(const std::string &id, const Point2D &position,
               const std::vector<PhaseConfig> &phases = {})
      : TrafficControl(id, TrafficControlType::TRAFFIC_LIGHT, position),
        m_current_phase(0), m_phase_time(0.0), m_phases(phases) {

    // Default 3-phase cycle if not specified
    if (m_phases.empty()) {
      m_phases = {{LightPhase::GREEN, 30.0},
                  {LightPhase::YELLOW, 3.0},
                  {LightPhase::RED, 30.0}};
    }
  }

  bool shouldStop(const Point2D &vehicle_position,
                  double vehicle_speed) const override {
    if (!m_active)
      return false;

    LightPhase phase = getCurrentPhase();

    // Stop on red or yellow (if close)
    if (phase == LightPhase::RED || phase == LightPhase::RED_YELLOW) {
      return true;
    }

    if (phase == LightPhase::YELLOW) {
      double distance = vehicle_position.distanceTo(m_position);
      // Stop if far enough to brake safely
      double stopping_distance =
          vehicle_speed * vehicle_speed / (2.0 * 3.0); // a = 3 m/s²
      return distance > stopping_distance;
    }

    return false;
  }

  void update(double dt) override {
    m_phase_time += dt;

    // Check if phase should change
    if (m_phase_time >= m_phases[m_current_phase].duration) {
      m_phase_time = 0.0;
      m_current_phase = (m_current_phase + 1) % m_phases.size();
    }
  }

  LightPhase getCurrentPhase() const { return m_phases[m_current_phase].phase; }

  void setPhase(LightPhase phase) {
    // Find phase in cycle
    for (size_t i = 0; i < m_phases.size(); ++i) {
      if (m_phases[i].phase == phase) {
        m_current_phase = i;
        m_phase_time = 0.0;
        break;
      }
    }
  }

  double getStoppingDistance() const override { return 15.0; }

private:
  size_t m_current_phase;
  double m_phase_time;
  std::vector<PhaseConfig> m_phases;
};

/**
 * @brief Yield sign.
 */
class YieldSign : public TrafficControl {
public:
  YieldSign(const std::string &id, const Point2D &position)
      : TrafficControl(id, TrafficControlType::YIELD_SIGN, position) {}

  bool shouldStop(const Point2D &vehicle_position,
                  double vehicle_speed) const override {
    if (!m_active)
      return false;

    // Yield means slow down, but only stop if necessary
    // This would need to check for other vehicles with right-of-way
    // For now, just slow down
    return false;
  }

  double getStoppingDistance() const override { return 8.0; }
};

/**
 * @brief Manager for traffic control devices.
 */
class TrafficControlManager {
public:
  /**
   * @brief Add traffic control device.
   */
  void addControl(std::shared_ptr<TrafficControl> control) {
    m_controls.push_back(control);
  }

  /**
   * @brief Remove traffic control device.
   */
  void removeControl(const std::string &id) {
    m_controls.erase(
        std::remove_if(m_controls.begin(), m_controls.end(),
                       [&id](const auto &c) { return c->getId() == id; }),
        m_controls.end());
  }

  /**
   * @brief Get all controls near a position.
   */
  std::vector<std::shared_ptr<TrafficControl>>
  getControlsNear(const Point2D &position, double radius) const {
    std::vector<std::shared_ptr<TrafficControl>> result;

    for (const auto &control : m_controls) {
      if (control->getPosition().distanceTo(position) <= radius) {
        result.push_back(control);
      }
    }

    return result;
  }

  /**
   * @brief Update all time-dependent controls.
   */
  void update(double dt) {
    for (auto &control : m_controls) {
      control->update(dt);
    }
  }

  /**
   * @brief Check if vehicle should stop for any control.
   */
  bool shouldStopForControl(const Point2D &vehicle_position,
                            double vehicle_speed,
                            double look_ahead_distance = 50.0) const {
    auto nearby = getControlsNear(vehicle_position, look_ahead_distance);

    for (const auto &control : nearby) {
      if (control->shouldStop(vehicle_position, vehicle_speed)) {
        return true;
      }
    }

    return false;
  }

  /**
   * @brief Get all controls.
   */
  const std::vector<std::shared_ptr<TrafficControl>> &getControls() const {
    return m_controls;
  }

private:
  std::vector<std::shared_ptr<TrafficControl>> m_controls;
};

} // namespace model
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_MODEL_TRAFFIC_CONTROL_H
