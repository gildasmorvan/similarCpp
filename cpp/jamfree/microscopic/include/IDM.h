#ifndef JAMFREE_MICROSCOPIC_MODELS_IDM_H
#define JAMFREE_MICROSCOPIC_MODELS_IDM_H

#include "../../kernel/include/model/Vehicle.h"
#include "../../kernel/include/tools/MathTools.h"
#include <cmath>
#include <memory>

namespace jamfree {
namespace microscopic {
namespace models {

/**
 * @brief Intelligent Driver Model (IDM) for car-following.
 *
 * The IDM is one of the most widely used car-following models in traffic
 * simulation. It produces realistic acceleration/deceleration behavior based
 * on:
 * - Desired speed
 * - Gap to leader
 * - Relative speed to leader
 *
 * Reference: Treiber, M., Hennecke, A., & Helbing, D. (2000).
 * "Congested traffic states in empirical observations and microscopic
 * simulations." Physical Review E, 62(2), 1805.
 */
class IDM {
public:
  /**
   * @brief Constructor with parameters.
   *
   * @param desired_speed Desired free-flow speed (m/s)
   * @param time_headway Desired time headway (seconds)
   * @param min_gap Minimum gap in standstill (meters)
   * @param max_accel Maximum acceleration (m/s²)
   * @param comfortable_decel Comfortable deceleration (m/s²)
   * @param accel_exponent Acceleration exponent (dimensionless)
   */
  IDM(double desired_speed = 33.3,    // 120 km/h
      double time_headway = 1.5,      // 1.5 seconds
      double min_gap = 2.0,           // 2 meters
      double max_accel = 1.0,         // 1 m/s²
      double comfortable_decel = 1.5, // 1.5 m/s²
      double accel_exponent = 4.0)    // delta = 4
      : m_desired_speed(desired_speed), m_time_headway(time_headway),
        m_min_gap(min_gap), m_max_accel(max_accel),
        m_comfortable_decel(comfortable_decel),
        m_accel_exponent(accel_exponent) {}

  /**
   * @brief Calculate acceleration for a vehicle.
   *
   * @param vehicle The vehicle to calculate acceleration for
   * @param leader Vehicle ahead (nullptr if no leader)
   * @return Acceleration in m/s²
   */
  double
  calculateAcceleration(const kernel::model::Vehicle &vehicle,
                        const kernel::model::Vehicle *leader = nullptr) const {
    double v = vehicle.getSpeed();
    double v0 = m_desired_speed;
    double a = m_max_accel;
    double delta = m_accel_exponent;

    // Free-flow acceleration term
    double accel_free = a * (1.0 - std::pow(v / v0, delta));

    if (leader == nullptr) {
      // No leader - just accelerate toward desired speed
      return accel_free;
    }

    // Calculate gap and relative speed
    double s = vehicle.getGapTo(*leader);
    double dv = vehicle.getRelativeSpeedTo(*leader);

    // Desired gap
    double s_star = calculateDesiredGap(v, dv);

    // Interaction term
    double accel_interaction =
        -a * kernel::tools::MathTools::square(s_star / s);

    // Total acceleration
    return accel_free + accel_interaction;
  }

  /**
   * @brief Calculate desired gap.
   *
   * s* = s₀ + v*T + v*Δv / (2√(a*b))
   *
   * @param speed Current speed (m/s)
   * @param speed_diff Relative speed to leader (m/s)
   * @return Desired gap (meters)
   */
  double calculateDesiredGap(double speed, double speed_diff) const {
    double s0 = m_min_gap;
    double T = m_time_headway;
    double a = m_max_accel;
    double b = m_comfortable_decel;

    // Interaction term (braking strategy)
    double interaction = speed * speed_diff / (2.0 * std::sqrt(a * b));

    // Desired gap
    return s0 + speed * T + interaction;
  }

  // Getters
  double getDesiredSpeed() const { return m_desired_speed; }
  double getTimeHeadway() const { return m_time_headway; }
  double getMinGap() const { return m_min_gap; }
  double getMaxAccel() const { return m_max_accel; }
  double getComfortableDecel() const { return m_comfortable_decel; }
  double getAccelExponent() const { return m_accel_exponent; }

  // Setters
  void setDesiredSpeed(double v0) { m_desired_speed = v0; }
  void setTimeHeadway(double T) { m_time_headway = T; }
  void setMinGap(double s0) { m_min_gap = s0; }
  void setMaxAccel(double a) { m_max_accel = a; }
  void setComfortableDecel(double b) { m_comfortable_decel = b; }
  void setAccelExponent(double delta) { m_accel_exponent = delta; }

private:
  double m_desired_speed;     ///< v₀: Desired speed (m/s)
  double m_time_headway;      ///< T: Desired time headway (s)
  double m_min_gap;           ///< s₀: Minimum gap (m)
  double m_max_accel;         ///< a: Maximum acceleration (m/s²)
  double m_comfortable_decel; ///< b: Comfortable deceleration (m/s²)
  double m_accel_exponent;    ///< δ: Acceleration exponent
};

/**
 * @brief IDM with improved emergency braking (IDM+).
 *
 * Adds a term to prevent collisions in critical situations.
 */
class IDMPlus : public IDM {
public:
  using IDM::IDM; // Inherit constructors

  /**
   * @brief Calculate acceleration with improved emergency braking.
   */
  double
  calculateAcceleration(const kernel::model::Vehicle &vehicle,
                        const kernel::model::Vehicle *leader = nullptr) const {
    if (leader == nullptr) {
      return IDM::calculateAcceleration(vehicle, leader);
    }

    double accel_idm = IDM::calculateAcceleration(vehicle, leader);

    // Emergency braking term
    double s = vehicle.getGapTo(*leader);
    double v = vehicle.getSpeed();
    double dv = vehicle.getRelativeSpeedTo(*leader);

    // Critical gap for collision avoidance
    double s_crit = getMinGap() + v * getTimeHeadway();

    if (s < s_crit && dv > 0) {
      // Approaching too fast - emergency braking
      double accel_emergency = -getComfortableDecel() * (s_crit - s) / s_crit;
      return std::min(accel_idm, accel_emergency);
    }

    return accel_idm;
  }
};

} // namespace models
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_MODELS_IDM_H
