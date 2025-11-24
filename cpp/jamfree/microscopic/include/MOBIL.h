#ifndef JAMFREE_MICROSCOPIC_MODELS_MOBIL_H
#define JAMFREE_MICROSCOPIC_MODELS_MOBIL_H

#include "../../kernel/include/model/Lane.h"
#include "../../kernel/include/model/Vehicle.h"
#include "IDM.h"
#include <cmath>
#include <memory>

namespace jamfree {
namespace microscopic {
namespace models {

/**
 * @brief MOBIL (Minimizing Overall Braking Induced by Lane changes) model.
 *
 * MOBIL is a lane-changing decision model that considers:
 * - Safety criterion (no dangerous lane changes)
 * - Incentive criterion (lane change must be beneficial)
 * - Politeness factor (consideration for other drivers)
 *
 * Reference: Kesting, A., Treiber, M., & Helbing, D. (2007).
 * "General lane-changing model MOBIL for car-following models."
 * Transportation Research Record, 1999(1), 86-94.
 */
class MOBIL {
public:
  /**
   * @brief Lane change direction.
   */
  enum class Direction {
    NONE, ///< No lane change
    LEFT, ///< Change to left lane
    RIGHT ///< Change to right lane
  };

  /**
   * @brief Constructor with parameters.
   *
   * @param politeness Politeness factor (0 = selfish, 1 = altruistic)
   * @param threshold Acceleration threshold for lane change (m/s²)
   * @param max_safe_decel Maximum safe deceleration (m/s²)
   * @param bias_right Bias toward right lane (m/s²)
   */
  MOBIL(double politeness = 0.5, double threshold = 0.1,
        double max_safe_decel = 4.0, double bias_right = 0.3)
      : m_politeness(politeness), m_threshold(threshold),
        m_max_safe_decel(max_safe_decel), m_bias_right(bias_right) {}

  /**
   * @brief Decide whether to change lanes.
   *
   * @param vehicle The vehicle making the decision
   * @param current_lane Current lane
   * @param left_lane Left lane (nullptr if none)
   * @param right_lane Right lane (nullptr if none)
   * @param car_following_model Car-following model (e.g., IDM)
   * @return Lane change direction
   */
  Direction decideLaneChange(const kernel::model::Vehicle &vehicle,
                             const kernel::model::Lane &current_lane,
                             const kernel::model::Lane *left_lane,
                             const kernel::model::Lane *right_lane,
                             const IDM &car_following_model) const {

    // Check both directions
    Direction best_direction = Direction::NONE;
    double best_advantage = -std::numeric_limits<double>::infinity();

    // Try left lane
    if (left_lane != nullptr) {
      double advantage = evaluateLaneChange(vehicle, current_lane, *left_lane,
                                            car_following_model);
      if (advantage > best_advantage) {
        best_advantage = advantage;
        best_direction = Direction::LEFT;
      }
    }

    // Try right lane (with bias)
    if (right_lane != nullptr) {
      double advantage = evaluateLaneChange(vehicle, current_lane, *right_lane,
                                            car_following_model);
      advantage += m_bias_right; // Prefer right lane (keep-right rule)

      if (advantage > best_advantage) {
        best_advantage = advantage;
        best_direction = Direction::RIGHT;
      }
    }

    // Only change if advantage exceeds threshold
    if (best_advantage > m_threshold) {
      return best_direction;
    }

    return Direction::NONE;
  }

private:
  /**
   * @brief Evaluate advantage of changing to a target lane.
   *
   * @param vehicle The vehicle
   * @param current_lane Current lane
   * @param target_lane Target lane
   * @param cf_model Car-following model
   * @return Advantage (positive = beneficial)
   */
  double evaluateLaneChange(const kernel::model::Vehicle &vehicle,
                            const kernel::model::Lane &current_lane,
                            const kernel::model::Lane &target_lane,
                            const IDM &cf_model) const {

    // Find relevant vehicles
    auto current_leader = current_lane.getLeader(vehicle);
    auto target_leader = target_lane.getLeader(vehicle);
    auto target_follower = target_lane.getFollower(vehicle);

    // Safety criterion: Check if new follower can brake safely
    if (target_follower != nullptr) {
      double new_follower_accel =
          cf_model.calculateAcceleration(*target_follower, &vehicle);

      if (new_follower_accel < -m_max_safe_decel) {
        // Too dangerous for follower
        return -std::numeric_limits<double>::infinity();
      }
    }

    // Incentive criterion: Calculate acceleration advantage

    // 1. Own acceleration in current lane
    double accel_current =
        cf_model.calculateAcceleration(vehicle, current_leader);

    // 2. Own acceleration in target lane
    double accel_target =
        cf_model.calculateAcceleration(vehicle, target_leader);

    // 3. Own advantage
    double own_advantage = accel_target - accel_current;

    // 4. Old follower's advantage (in current lane)
    double old_follower_advantage = 0.0;
    auto current_follower = current_lane.getFollower(vehicle);
    if (current_follower != nullptr) {
      double accel_before =
          cf_model.calculateAcceleration(*current_follower, &vehicle);
      double accel_after =
          cf_model.calculateAcceleration(*current_follower, current_leader);
      old_follower_advantage = accel_after - accel_before;
    }

    // 5. New follower's disadvantage (in target lane)
    double new_follower_disadvantage = 0.0;
    if (target_follower != nullptr) {
      double accel_before =
          cf_model.calculateAcceleration(*target_follower, target_leader);
      double accel_after =
          cf_model.calculateAcceleration(*target_follower, &vehicle);
      new_follower_disadvantage = accel_after - accel_before;
    }

    // Total advantage with politeness factor
    double total_advantage =
        own_advantage +
        m_politeness * (old_follower_advantage + new_follower_disadvantage);

    return total_advantage;
  }

  double m_politeness;     ///< Politeness factor (0-1)
  double m_threshold;      ///< Acceleration threshold (m/s²)
  double m_max_safe_decel; ///< Maximum safe deceleration (m/s²)
  double m_bias_right;     ///< Bias toward right lane (m/s²)
};

} // namespace models
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_MODELS_MOBIL_H
