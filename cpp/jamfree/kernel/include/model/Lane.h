#ifndef JAMFREE_KERNEL_MODEL_LANE_H
#define JAMFREE_KERNEL_MODEL_LANE_H

#include "Point2D.h"
#include <memory>
#include <string>
#include <vector>

namespace jamfree {
namespace kernel {
namespace model {

// Forward declarations
class Road;
class Vehicle;

/**
 * @brief Represents a lane within a road.
 *
 * A lane is a longitudinal section of a road where vehicles travel.
 * It has a specific width, speed limit, and can contain multiple vehicles.
 */
class Lane {
public:
  /**
   * @brief Constructor.
   *
   * @param id Unique identifier for the lane
   * @param index Lane index (0 = rightmost)
   * @param width Lane width in meters
   * @param length Lane length in meters
   */
  Lane(const std::string &id, int index, double width, double length)
      : m_id(id), m_index(index), m_width(width), m_length(length),
        m_speed_limit(33.3) // Default: 120 km/h = 33.3 m/s
        ,
        m_parent_road(nullptr) {}

  // Getters
  const std::string &getId() const { return m_id; }
  int getIndex() const { return m_index; }
  double getWidth() const { return m_width; }
  double getLength() const { return m_length; }
  double getSpeedLimit() const { return m_speed_limit; }
  Road *getParentRoad() const { return m_parent_road; }

  // Setters
  void setSpeedLimit(double speed_limit) { m_speed_limit = speed_limit; }
  void setParentRoad(Road *road) { m_parent_road = road; }

  /**
   * @brief Get position along lane at given distance.
   *
   * @param distance Distance from lane start (meters)
   * @return 2D position
   */
  Point2D getPositionAt(double distance) const;

  /**
   * @brief Get heading (direction) at given distance.
   *
   * @param distance Distance from lane start (meters)
   * @return Heading in radians
   */
  double getHeadingAt(double distance) const;

  /**
   * @brief Check if a position is within this lane.
   *
   * @param position Position to check
   * @param tolerance Tolerance in meters
   * @return True if position is in lane
   */
  bool containsPosition(const Point2D &position, double tolerance = 0.5) const;

  /**
   * @brief Get distance along lane for a given position.
   *
   * @param position Position to project onto lane
   * @return Distance from lane start (meters)
   */
  double getDistanceAlong(const Point2D &position) const;

  /**
   * @brief Add vehicle to lane.
   *
   * @param vehicle Vehicle to add
   */
  void addVehicle(std::shared_ptr<Vehicle> vehicle);

  /**
   * @brief Remove vehicle from lane.
   *
   * @param vehicle Vehicle to remove
   */
  void removeVehicle(std::shared_ptr<Vehicle> vehicle);

  /**
   * @brief Get all vehicles in lane.
   *
   * @return Vector of vehicles
   */
  const std::vector<std::shared_ptr<Vehicle>> &getVehicles() const {
    return m_vehicles;
  }

  /**
   * @brief Get vehicle ahead of given position.
   *
   * @param position Current position along lane
   * @return Vehicle ahead, or nullptr if none
   */
  std::shared_ptr<Vehicle> getVehicleAhead(double position) const;

  /**
   * @brief Get vehicle behind given position.
   *
   * @param position Current position along lane
   * @return Vehicle behind, or nullptr if none
   */
  std::shared_ptr<Vehicle> getVehicleBehind(double position) const;

  /**
   * @brief Get gap to vehicle ahead.
   *
   * @param position Current position along lane
   * @return Gap in meters, or infinity if no vehicle ahead
   */
  double getGapAhead(double position) const;

  /**
   * @brief Get leader vehicle (vehicle ahead) relative to a given vehicle.
   *
   * @param vehicle Reference vehicle
   * @return Leader vehicle, or nullptr if none
   */
  Vehicle *getLeader(const Vehicle &vehicle) const;

  /**
   * @brief Get follower vehicle (vehicle behind) relative to a given vehicle.
   *
   * @param vehicle Reference vehicle
   * @return Follower vehicle, or nullptr if none
   */
  Vehicle *getFollower(const Vehicle &vehicle) const;

private:
  std::string m_id;
  int m_index;
  double m_width;
  double m_length;
  double m_speed_limit;
  Road *m_parent_road;
  std::vector<std::shared_ptr<Vehicle>> m_vehicles;
};

} // namespace model
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_MODEL_LANE_H
