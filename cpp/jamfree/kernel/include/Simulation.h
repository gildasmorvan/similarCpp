#ifndef JAMFREE_KERNEL_SIMULATION_H
#define JAMFREE_KERNEL_SIMULATION_H

#include "../../microscopic/include/IDM.h"
#include "model/Lane.h"
#include "model/Road.h"
#include "model/Vehicle.h"
#include <map>
#include <memory>
#include <vector>

namespace jamfree {
namespace kernel {

/**
 * @brief Main simulation class for traffic simulation.
 *
 * Manages vehicles, roads, and simulation time stepping.
 */
class Simulation {
public:
  /**
   * @brief Constructor.
   *
   * @param dt Time step (seconds)
   */
  explicit Simulation(double dt = 0.1)
      : m_time(0.0), m_dt(dt), m_step_count(0) {}

  /**
   * @brief Add a road to the simulation.
   *
   * @param road Road to add
   */
  void addRoad(std::shared_ptr<model::Road> road) { m_roads.push_back(road); }

  /**
   * @brief Add a vehicle to the simulation.
   *
   * @param vehicle Vehicle to add
   * @param lane Lane to place vehicle in
   * @param position Initial position along lane (meters)
   * @param speed Initial speed (m/s)
   */
  void addVehicle(std::shared_ptr<model::Vehicle> vehicle,
                  std::shared_ptr<model::Lane> lane, double position,
                  double speed) {
    vehicle->setCurrentLane(lane);
    vehicle->setLanePosition(position);
    vehicle->setSpeed(speed);

    // Update 2D position
    if (lane && lane->getParentRoad()) {
      vehicle->setPosition(lane->getPositionAt(position));
      vehicle->setHeading(lane->getHeadingAt(position));
    }

    lane->addVehicle(vehicle);
    m_vehicles.push_back(vehicle);

    // Create default IDM model for vehicle
    m_vehicle_models[vehicle->getId()] =
        std::make_shared<microscopic::models::IDM>();
  }

  /**
   * @brief Set car-following model for a vehicle.
   *
   * @param vehicle_id Vehicle ID
   * @param model Car-following model
   */
  void setVehicleModel(const std::string &vehicle_id,
                       std::shared_ptr<microscopic::models::IDM> model) {
    m_vehicle_models[vehicle_id] = model;
  }

  /**
   * @brief Run simulation for one time step.
   */
  void step() {
    // Calculate accelerations for all vehicles
    std::map<std::string, double> accelerations;

    for (auto &vehicle : m_vehicles) {
      auto lane = vehicle->getCurrentLane();
      if (!lane)
        continue;

      // Find leader
      model::Vehicle *leader = nullptr;
      auto leader_ptr = lane->getVehicleAhead(vehicle->getLanePosition());
      if (leader_ptr) {
        leader = leader_ptr.get();
      }

      // Get model and calculate acceleration
      auto model = m_vehicle_models[vehicle->getId()];
      if (model) {
        double accel = model->calculateAcceleration(*vehicle, leader);
        accelerations[vehicle->getId()] = accel;
      }
    }

    // Update all vehicles
    for (auto &vehicle : m_vehicles) {
      double accel = accelerations[vehicle->getId()];
      vehicle->update(m_dt, accel);
    }

    m_time += m_dt;
    m_step_count++;
  }

  /**
   * @brief Run simulation for multiple steps.
   *
   * @param num_steps Number of steps to run
   */
  void run(int num_steps) {
    for (int i = 0; i < num_steps; ++i) {
      step();
    }
  }

  // Getters
  double getTime() const { return m_time; }
  double getTimeStep() const { return m_dt; }
  int getStepCount() const { return m_step_count; }

  const std::vector<std::shared_ptr<model::Vehicle>> &getVehicles() const {
    return m_vehicles;
  }

  const std::vector<std::shared_ptr<model::Road>> &getRoads() const {
    return m_roads;
  }

  /**
   * @brief Get vehicle by ID.
   *
   * @param id Vehicle ID
   * @return Vehicle pointer, or nullptr if not found
   */
  std::shared_ptr<model::Vehicle> getVehicle(const std::string &id) const {
    for (auto &v : m_vehicles) {
      if (v->getId() == id)
        return v;
    }
    return nullptr;
  }

private:
  double m_time;
  double m_dt;
  int m_step_count;

  std::vector<std::shared_ptr<model::Road>> m_roads;
  std::vector<std::shared_ptr<model::Vehicle>> m_vehicles;
  std::map<std::string, std::shared_ptr<microscopic::models::IDM>>
      m_vehicle_models;
};

} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_SIMULATION_H
