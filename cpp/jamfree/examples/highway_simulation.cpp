#include "../microscopic/include/agents/VehiclePrivateLocalStateMicro.h"
#include "../microscopic/include/agents/VehiclePublicLocalStateMicro.h"
#include "../microscopic/include/decision/VehicleDecisionModelMicro.h"
#include "../microscopic/include/IDM.h"
#include "../microscopic/include/decision/dms/SubsumptionDMS.h"
#include "../microscopic/include/decision/dms/ForwardAccelerationDMS.h"
#include "../microscopic/include/perception/VehiclePerceptionModelMicro.h"
#include "../microscopic/include/reaction/MicroscopicReactionModel.h"
#include "../kernel/include/agents/VehicleAgent.h"
#include "../kernel/include/model/Lane.h"
#include "../kernel/include/model/Road.h"
#include "../kernel/include/simulation/SimulationEngine.h"
#include <iostream>
#include <memory>

using namespace jamfree;

/**
 * @brief Simple highway traffic simulation example.
 *
 * This example demonstrates:
 * - Creating a highway with multiple lanes
 * - Adding vehicles with different behaviors
 * - Running a simulation with perception, decision, and reaction
 * - Visualizing the results
 */
int main() {
  std::cout << "=== JamFree Highway Simulation Example ===" << std::endl;
  std::cout << "Using SIMILAR Extended Kernel Architecture" << std::endl
            << std::endl;

  // Create simulation engine
  double timeStep = 0.1; // 100ms time step
  auto engine = std::make_shared<kernel::simulation::SimulationEngine>(timeStep);

  // Create a 3-lane highway (1km long)
  kernel::model::Point2D start(0, 0);
  kernel::model::Point2D end(1000, 0);
  auto highway = std::make_shared<kernel::model::Road>("Highway-1", start, end, 3, 3.5);

  std::cout << "Created highway: " << highway->getId() << std::endl;
  std::cout << "  Lanes: " << highway->getNumLanes() << std::endl;
  std::cout << "  Length: " << start.distanceTo(end) << " meters" << std::endl
            << std::endl;

  // Create reaction model for microscopic level
  auto reactionModel =
      std::make_shared<microscopic::reaction::MicroscopicReactionModel>(timeStep);
  kernel::agents::LevelIdentifier microLevel("Microscopic");
  engine->setReactionModel(microLevel, reactionModel);

  // Create vehicles
  std::cout << "Creating vehicles..." << std::endl;

  for (int i = 0; i < 5; ++i) {
    // Create vehicle agent
    std::string vehicleId = "Vehicle-" + std::to_string(i);
    auto vehicle = std::make_shared<kernel::agents::VehicleAgent>(vehicleId);

    // Create public state (visible to all)
    auto publicState =
        std::make_shared<microscopic::agents::VehiclePublicLocalStateMicro>(vehicleId);
    publicState->setCurrentLane(highway->getLane(i % 3).get());
    publicState->setLaneIndex(i % 3);
    publicState->setLanePosition(i * 50.0);  // Stagger vehicles
    publicState->setSpeed(25.0 + (i * 2.0)); // Different speeds (90-108 km/h)
    publicState->setAcceleration(0.0);
    publicState->setActive(true);

    // Create private state (agent-specific)
    auto privateState =
        std::make_shared<microscopic::agents::VehiclePrivateLocalStateMicro>(vehicleId);
    privateState->setDesiredSpeed(30.0 + (i * 2.0)); // Desired speed
    privateState->setTimeHeadway(1.5);
    privateState->setMinGap(2.0);
    privateState->setMaxAcceleration(2.0);
    privateState->setComfortableDeceleration(3.0);
    privateState->setPoliteness(0.5);

    // Create perception model
    auto perceptionModel =
        std::make_shared<microscopic::perception::VehiclePerceptionModelMicro>(150.0);

    // Create decision model with subsumption DMS
    auto subsumptionDMS = std::make_shared<microscopic::decision::dms::SubsumptionDMS>();

    // Add IDM-based acceleration DMS
    auto idm = std::make_shared<microscopic::models::IDM>();
    auto accelDMS = std::make_shared<microscopic::decision::dms::ForwardAccelerationDMS>(idm);
    subsumptionDMS->addSubmodel(accelDMS);

    auto decisionModel =
        std::make_shared<microscopic::decision::VehicleDecisionModelMicro>(subsumptionDMS);

    // Add to agent
    vehicle->includeNewLevel(microLevel, publicState, privateState);
    vehicle->setPerceptionModel(microLevel, perceptionModel);
    vehicle->setDecisionModel(microLevel, decisionModel);

    // Add to simulation
    engine->addAgent(vehicle);

    std::cout << "  " << vehicleId << ": Lane " << (i % 3) << ", Position "
              << (i * 50.0) << "m, Speed " << publicState->getSpeed() << " m/s"
              << std::endl;
  }

  std::cout << std::endl << "Running simulation..." << std::endl;
  std::cout << "Time Step: " << timeStep << "s" << std::endl;
  std::cout << "Duration: 10 seconds (100 steps)" << std::endl << std::endl;

  // Run simulation for 10 seconds
  int numSteps = 100;
  for (int step = 0; step < numSteps; ++step) {
    engine->step();

    // Print status every 10 steps (1 second)
    if (step % 10 == 0) {
      std::cout << "t=" << engine->getCurrentTime() << "s: ";

      const auto &agents = engine->getAgents();
      for (const auto &agent : agents) {
        auto publicState = agent->getPublicLocalState(microLevel);
        if (publicState) {
          auto vehicleState =
              std::dynamic_pointer_cast<microscopic::agents::VehiclePublicLocalStateMicro>(
                  publicState);
          if (vehicleState) {
            std::cout << agent->getId() << "="
                      << static_cast<int>(vehicleState->getLanePosition())
                      << "m ";
          }
        }
      }
      std::cout << std::endl;
    }
  }

  std::cout << std::endl << "Simulation complete!" << std::endl;
  std::cout << "Total steps: " << engine->getStepCount() << std::endl;
  std::cout << "Final time: " << engine->getCurrentTime() << "s" << std::endl;

  return 0;
}
