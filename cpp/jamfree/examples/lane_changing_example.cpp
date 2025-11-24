/**
 * @file lane_changing_example.cpp
 * @brief Simplified example demonstrating MOBIL lane-changing model with IDM.
 *
 * This example creates a multi-lane highway with vehicles that can change lanes
 * using the MOBIL (Minimizing Overall Braking Induced by Lane changes) model.
 *
 * Note: This is a simplified version that manually manages the simulation loop.
 */

#include "../kernel/include/model/Lane.h"
#include "../kernel/include/model/Point2D.h"
#include "../kernel/include/model/Road.h"
#include "../kernel/include/model/Vehicle.h"
#include "../microscopic/include/IDM.h"
#include "../microscopic/include/MOBIL.h"

#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

using namespace jamfree;
using namespace jamfree::kernel::model;
using namespace jamfree::microscopic::models;

int main() {
  std::cout << "JamFree C++ - Lane Changing Example with MOBIL" << std::endl;
  std::cout << "========================================" << std::endl;
  std::cout << std::endl;

  // Create a 3-lane highway
  auto highway = std::make_shared<Road>("highway_1", Point2D(0, 0),
                                        Point2D(3000, 0), // 3 km highway
                                        3,                // 3 lanes
                                        3.5               // 3.5m lane width
  );

  std::cout << "Created highway:" << std::endl;
  std::cout << "  Length: " << highway->getLength() << " m" << std::endl;
  std::cout << "  Lanes: " << highway->getNumLanes() << std::endl;
  std::cout << std::endl;

  // Create IDM car-following model (shared by all vehicles)
  auto idm = std::make_shared<IDM>(30.0, // desired speed: 108 km/h
                                   1.5,  // time headway: 1.5s
                                   2.0,  // min gap: 2m
                                   1.0,  // max accel: 1 m/s²
                                   1.5   // comfortable decel: 1.5 m/s²
  );

  // Create MOBIL lane-changing model
  MOBIL mobil(0.3, // politeness: 0.3 (slightly selfish)
              0.1, // threshold: 0.1 m/s²
              4.0, // max safe decel: 4 m/s²
              0.2  // right lane bias: 0.2 m/s²
  );

  // Add vehicles with varying speeds and lanes
  std::cout << "Adding vehicles..." << std::endl;

  std::vector<std::shared_ptr<Vehicle>> vehicles;

  // Slow vehicle in right lane (will cause others to change lanes)
  {
    auto vehicle = std::make_shared<Vehicle>("slow_truck");
    auto lane = highway->getLane(0); // Right lane
    vehicle->setCurrentLane(lane);
    vehicle->setLanePosition(500.0);
    vehicle->setSpeed(20.0); // 72 km/h
    lane->addVehicle(vehicle);
    vehicles.push_back(vehicle);
    std::cout << "  slow_truck: lane=0, pos=500m, speed=72 km/h" << std::endl;
  }

  // Fast vehicles behind the slow one
  for (int i = 0; i < 5; i++) {
    auto vehicle = std::make_shared<Vehicle>("car_" + std::to_string(i));
    auto lane = highway->getLane(0); // Start in right lane
    double pos = 100.0 + i * 50.0;
    double speed = 28.0 + i * 1.0; // 100-108 km/h
    vehicle->setCurrentLane(lane);
    vehicle->setLanePosition(pos);
    vehicle->setSpeed(speed);
    lane->addVehicle(vehicle);
    vehicles.push_back(vehicle);
    std::cout << "  car_" << i << ": lane=0, pos=" << pos
              << "m, speed=" << (speed * 3.6) << " km/h" << std::endl;
  }

  // Some vehicles in middle lane
  for (int i = 0; i < 3; i++) {
    auto vehicle = std::make_shared<Vehicle>("mid_" + std::to_string(i));
    auto lane = highway->getLane(1); // Middle lane
    double pos = 200.0 + i * 100.0;
    double speed = 25.0 + i * 2.0; // 90-100 km/h
    vehicle->setCurrentLane(lane);
    vehicle->setLanePosition(pos);
    vehicle->setSpeed(speed);
    lane->addVehicle(vehicle);
    vehicles.push_back(vehicle);
    std::cout << "  mid_" << i << ": lane=1, pos=" << pos
              << "m, speed=" << (speed * 3.6) << " km/h" << std::endl;
  }

  std::cout << std::endl;

  // Run simulation with lane-changing logic
  std::cout << "Running simulation for 60 seconds..." << std::endl;
  std::cout << std::endl;

  double dt = 0.1;           // Time step: 0.1s
  int num_steps = 600;       // 60 seconds
  int report_interval = 100; // Report every 10 seconds

  for (int step = 0; step < num_steps; step++) {
    // Phase 1: Calculate accelerations using IDM
    for (auto &vehicle : vehicles) {
      auto current_lane = vehicle->getLane();
      if (!current_lane)
        continue;

      // Find leader in current lane
      Vehicle *leader = current_lane->getLeader(*vehicle);

      // Calculate acceleration using IDM
      double accel = idm->calculateAcceleration(*vehicle, leader);

      // Update vehicle state
      vehicle->update(dt, accel);
    }

    // Phase 2: Apply lane-changing decisions (MOBIL)
    for (auto &vehicle : vehicles) {
      auto current_lane = vehicle->getLane();
      if (!current_lane)
        continue;

      int lane_idx = current_lane->getIndex();
      auto road = current_lane->getParentRoad();
      if (!road)
        continue;

      // Get adjacent lanes
      Lane *left_lane = (lane_idx < road->getNumLanes() - 1)
                            ? road->getLane(lane_idx + 1).get()
                            : nullptr;
      Lane *right_lane =
          (lane_idx > 0) ? road->getLane(lane_idx - 1).get() : nullptr;

      // Decide lane change
      auto decision = mobil.decideLaneChange(*vehicle, *current_lane, left_lane,
                                             right_lane, *idm);

      // Execute lane change
      if (decision == MOBIL::Direction::LEFT && left_lane) {
        std::cout << "  [t=" << std::fixed << std::setprecision(1)
                  << (step * dt) << "s] " << vehicle->getId()
                  << " changes LEFT (lane " << lane_idx << " -> "
                  << (lane_idx + 1) << ")" << std::endl;

        // Remove from current lane, add to new lane
        current_lane->removeVehicle(vehicle);
        vehicle->setCurrentLane(highway->getLane(lane_idx + 1));
        highway->getLane(lane_idx + 1)->addVehicle(vehicle);

      } else if (decision == MOBIL::Direction::RIGHT && right_lane) {
        std::cout << "  [t=" << std::fixed << std::setprecision(1)
                  << (step * dt) << "s] " << vehicle->getId()
                  << " changes RIGHT (lane " << lane_idx << " -> "
                  << (lane_idx - 1) << ")" << std::endl;

        // Remove from current lane, add to new lane
        current_lane->removeVehicle(vehicle);
        vehicle->setCurrentLane(highway->getLane(lane_idx - 1));
        highway->getLane(lane_idx - 1)->addVehicle(vehicle);
      }
    }

    // Periodic status report
    if (step % report_interval == 0 && step > 0) {
      std::cout << std::endl;
      std::cout << "Status at t=" << (step * dt) << "s:" << std::endl;
      for (const auto &vehicle : vehicles) {
        auto lane = vehicle->getLane();
        std::cout << "  " << std::setw(12) << vehicle->getId()
                  << ": lane=" << (lane ? lane->getIndex() : -1)
                  << ", pos=" << std::setw(7) << std::fixed
                  << std::setprecision(1) << vehicle->getLanePosition() << "m"
                  << ", speed=" << std::setw(5) << (vehicle->getSpeed() * 3.6)
                  << " km/h" << std::endl;
      }
      std::cout << std::endl;
    }
  }

  // Final state
  std::cout << std::endl;
  std::cout << "Final State (t=60s):" << std::endl;
  for (const auto &vehicle : vehicles) {
    auto lane = vehicle->getLane();
    std::cout << vehicle->getId() << ":" << std::endl;
    std::cout << "  Lane: " << (lane ? lane->getIndex() : -1) << std::endl;
    std::cout << "  Position: " << std::fixed << std::setprecision(1)
              << vehicle->getLanePosition() << " m" << std::endl;
    std::cout << "  Speed: " << (vehicle->getSpeed() * 3.6) << " km/h"
              << std::endl;
    std::cout << "  Acceleration: " << vehicle->getAcceleration() << " m/s²"
              << std::endl;
    std::cout << std::endl;
  }

  std::cout << "Simulation complete!" << std::endl;
  std::cout << "========================================" << std::endl;

  return 0;
}
