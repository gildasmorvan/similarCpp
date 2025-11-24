/**
 * @file highway_example.cpp
 * @brief Simple highway traffic simulation example using JamFree C++.
 *
 * This example demonstrates:
 * - Creating a straight highway with multiple lanes
 * - Adding vehicles with IDM car-following behavior
 * - Running the simulation
 * - Outputting results
 */

#include "../kernel/include/Simulation.h"
#include "../kernel/include/model/Point2D.h"
#include "../kernel/include/model/Road.h"
#include "../kernel/include/model/Vehicle.h"
#include "../kernel/include/tools/MathTools.h"
#include "../microscopic/include/IDM.h"

#include <fstream>
#include <iomanip>
#include <iostream>

using namespace jamfree;
using namespace jamfree::kernel;
using namespace jamfree::kernel::model;
using namespace jamfree::kernel::tools;
using namespace jamfree::microscopic::models;

int main() {
  std::cout << "========================================\n";
  std::cout << "JamFree C++ - Highway Simulation\n";
  std::cout << "========================================\n\n";

  // Simulation parameters
  const double DT = 0.1;               // Time step (seconds)
  const int NUM_STEPS = 1000;          // Simulation duration
  const int NUM_VEHICLES = 10;         // Number of vehicles
  const double VEHICLE_SPACING = 50.0; // Initial spacing (meters)

  // Create simulation
  Simulation sim(DT);

  // Create a 2km straight highway with 2 lanes
  auto highway = std::make_shared<Road>("highway_1", Point2D(0, 0), // Start
                                        Point2D(2000, 0),           // End (2km)
                                        2,                          // 2 lanes
                                        3.5 // 3.5m lane width
  );
  sim.addRoad(highway);

  std::cout << "Created highway:\n";
  std::cout << "  Length: " << highway->getLength() << " m\n";
  std::cout << "  Lanes: " << highway->getNumLanes() << "\n\n";

  // Get right lane
  auto lane = highway->getLane(0);

  // Create vehicles with varying desired speeds
  std::cout << "Adding " << NUM_VEHICLES << " vehicles...\n";
  for (int i = 0; i < NUM_VEHICLES; ++i) {
    std::string id = "vehicle_" + std::to_string(i);
    auto vehicle = std::make_shared<Vehicle>(id);

    // Initial position and speed
    double position = i * VEHICLE_SPACING;
    double speed = MathTools::kmhToMs(100 + (i % 3) * 10); // 100-120 km/h

    // Add to simulation
    sim.addVehicle(vehicle, lane, position, speed);

    // Create IDM model with varying desired speeds
    auto idm = std::make_shared<IDM>(
        MathTools::kmhToMs(110 + (i % 3) * 10), // Desired: 110-130 km/h
        1.5,                                    // Time headway
        2.0,                                    // Min gap
        1.0,                                    // Max accel
        1.5                                     // Comfortable decel
    );
    sim.setVehicleModel(id, idm);

    std::cout << "  " << id << ": pos=" << position << "m, "
              << "speed=" << MathTools::msToKmh(speed) << " km/h\n";
  }

  std::cout << "\nRunning simulation for " << NUM_STEPS * DT
            << " seconds...\n\n";

  // Open output file
  std::ofstream output("highway_simulation.csv");
  output << "time,vehicle_id,position,speed,acceleration\n";

  // Run simulation
  for (int step = 0; step < NUM_STEPS; ++step) {
    sim.step();

    // Output every 10 steps (1 second)
    if (step % 10 == 0) {
      for (const auto &vehicle : sim.getVehicles()) {
        output << std::fixed << std::setprecision(2) << sim.getTime() << ","
               << vehicle->getId() << "," << vehicle->getLanePosition() << ","
               << MathTools::msToKmh(vehicle->getSpeed()) << ","
               << vehicle->getAcceleration() << "\n";
      }
    }

    // Print progress
    if (step % 100 == 0) {
      std::cout << "Step " << step << " / " << NUM_STEPS
                << " (t=" << sim.getTime() << "s)\n";
    }
  }

  output.close();

  // Print final state
  std::cout << "\n========================================\n";
  std::cout << "Final State (t=" << sim.getTime() << "s):\n";
  std::cout << "========================================\n";

  for (const auto &vehicle : sim.getVehicles()) {
    std::cout << vehicle->getId() << ":\n";
    std::cout << "  Position: " << std::fixed << std::setprecision(1)
              << vehicle->getLanePosition() << " m\n";
    std::cout << "  Speed: " << MathTools::msToKmh(vehicle->getSpeed())
              << " km/h\n";
    std::cout << "  Acceleration: " << vehicle->getAcceleration()
              << " m/s²\n\n";
  }

  std::cout << "Results saved to: highway_simulation.csv\n";
  std::cout << "\nSimulation complete!\n";

  return 0;
}
