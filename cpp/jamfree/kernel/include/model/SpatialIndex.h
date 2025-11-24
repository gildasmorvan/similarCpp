#ifndef JAMFREE_KERNEL_MODEL_SPATIAL_INDEX_H
#define JAMFREE_KERNEL_MODEL_SPATIAL_INDEX_H

#include "Vehicle.h"
#include <algorithm>
#include <memory>
#include <vector>

namespace jamfree {
namespace kernel {
namespace model {

/**
 * @brief Spatial index for efficient vehicle queries.
 *
 * Provides O(log N) queries instead of O(N) for finding leaders/followers.
 * Uses a sorted array with binary search.
 *
 * For more advanced needs, this can be extended to R-tree or grid-based index.
 */
class SpatialIndex {
public:
  /**
   * @brief Constructor.
   */
  SpatialIndex() = default;

  /**
   * @brief Add vehicle to index.
   *
   * @param vehicle Vehicle to add
   */
  void addVehicle(const std::shared_ptr<Vehicle> &vehicle) {
    m_vehicles.push_back(vehicle);
    m_needs_sort = true;
  }

  /**
   * @brief Remove vehicle from index.
   *
   * @param vehicle Vehicle to remove
   */
  void removeVehicle(const std::shared_ptr<Vehicle> &vehicle) {
    auto it = std::find(m_vehicles.begin(), m_vehicles.end(), vehicle);
    if (it != m_vehicles.end()) {
      m_vehicles.erase(it);
      m_needs_sort = true;
    }
  }

  /**
   * @brief Update index (resort if needed).
   *
   * Call this after vehicle positions have changed.
   */
  void update() {
    if (m_needs_sort) {
      sortVehicles();
      m_needs_sort = false;
    }
  }

  /**
   * @brief Find leader (vehicle ahead).
   *
   * O(log N) complexity using binary search.
   *
   * @param vehicle Query vehicle
   * @return Leader vehicle or nullptr
   */
  Vehicle *findLeader(const Vehicle &vehicle) {
    if (m_needs_sort) {
      sortVehicles();
      m_needs_sort = false;
    }

    if (m_vehicles.empty()) {
      return nullptr;
    }

    double pos = vehicle.getLanePosition();

    // Binary search for first vehicle ahead
    auto it = std::upper_bound(
        m_vehicles.begin(), m_vehicles.end(), pos,
        [](double position, const std::shared_ptr<Vehicle> &v) {
          return position < v->getLanePosition();
        });

    if (it != m_vehicles.end()) {
      // Make sure it's not the same vehicle
      if (it->get() != &vehicle) {
        return it->get();
      }
      // Try next vehicle
      ++it;
      if (it != m_vehicles.end()) {
        return it->get();
      }
    }

    return nullptr;
  }

  /**
   * @brief Find follower (vehicle behind).
   *
   * O(log N) complexity using binary search.
   *
   * @param vehicle Query vehicle
   * @return Follower vehicle or nullptr
   */
  Vehicle *findFollower(const Vehicle &vehicle) {
    if (m_needs_sort) {
      sortVehicles();
      m_needs_sort = false;
    }

    if (m_vehicles.empty()) {
      return nullptr;
    }

    double pos = vehicle.getLanePosition();

    // Binary search for last vehicle behind
    auto it = std::lower_bound(
        m_vehicles.begin(), m_vehicles.end(), pos,
        [](const std::shared_ptr<Vehicle> &v, double position) {
          return v->getLanePosition() < position;
        });

    if (it != m_vehicles.begin()) {
      --it;
      // Make sure it's not the same vehicle
      if (it->get() != &vehicle) {
        return it->get();
      }
      // Try previous vehicle
      if (it != m_vehicles.begin()) {
        --it;
        return it->get();
      }
    }

    return nullptr;
  }

  /**
   * @brief Find all vehicles in range.
   *
   * @param min_pos Minimum position
   * @param max_pos Maximum position
   * @return Vector of vehicles in range
   */
  std::vector<Vehicle *> findInRange(double min_pos, double max_pos) {
    if (m_needs_sort) {
      sortVehicles();
      m_needs_sort = false;
    }

    std::vector<Vehicle *> result;

    // Binary search for range
    auto it_start = std::lower_bound(
        m_vehicles.begin(), m_vehicles.end(), min_pos,
        [](const std::shared_ptr<Vehicle> &v, double position) {
          return v->getLanePosition() < position;
        });

    auto it_end = std::upper_bound(
        m_vehicles.begin(), m_vehicles.end(), max_pos,
        [](double position, const std::shared_ptr<Vehicle> &v) {
          return position < v->getLanePosition();
        });

    for (auto it = it_start; it != it_end; ++it) {
      result.push_back(it->get());
    }

    return result;
  }

  /**
   * @brief Get number of vehicles in index.
   *
   * @return Vehicle count
   */
  size_t size() const { return m_vehicles.size(); }

  /**
   * @brief Clear index.
   */
  void clear() {
    m_vehicles.clear();
    m_needs_sort = false;
  }

  /**
   * @brief Get all vehicles (sorted by position).
   *
   * @return Vector of vehicles
   */
  const std::vector<std::shared_ptr<Vehicle>> &getVehicles() {
    if (m_needs_sort) {
      sortVehicles();
      m_needs_sort = false;
    }
    return m_vehicles;
  }

private:
  std::vector<std::shared_ptr<Vehicle>> m_vehicles;
  bool m_needs_sort = false;

  /**
   * @brief Sort vehicles by lane position.
   */
  void sortVehicles() {
    std::sort(m_vehicles.begin(), m_vehicles.end(),
              [](const std::shared_ptr<Vehicle> &a,
                 const std::shared_ptr<Vehicle> &b) {
                return a->getLanePosition() < b->getLanePosition();
              });
  }
};

} // namespace model
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_MODEL_SPATIAL_INDEX_H
