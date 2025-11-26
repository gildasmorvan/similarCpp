/**
 * @file complete_simulation_example.cpp
 * @brief Complete example of SIMILAR multi-level simulation in JamFree.
 *
 * This example demonstrates:
 * - Complete simulation engine setup
 * - Multi-level coordination (microscopic + macroscopic)
 * - Agent lifecycle management
 * - Perception-decision-action-reaction cycle
 * - Level transitions
 */

#include <iostream>
#include <memory>
#include <vector>

// Simulation infrastructure
#include "../kernel/include/simulation/MultiLevelCoordinator.h"
#include "../kernel/include/simulation/SimulationEngine.h"

// Core agent infrastructure
#include "../kernel/include/agents/VehicleAgent.h"
#include "../kernel/include/levels/LevelIdentifiers.h"

// Microscopic components
#include "../microscopic/include/agents/VehiclePrivateLocalStateMicro.h"
#include "../microscopic/include/agents/VehiclePublicLocalStateMicro.h"
#include "../microscopic/include/decision/VehicleDecisionModelMicro.h"
#include "../microscopic/include/decision/dms/ConjunctionDMS.h"
#include "../microscopic/include/decision/dms/ForwardAccelerationDMS.h"
#include "../microscopic/include/decision/dms/LaneChangeDMS.h"
#include "../microscopic/include/perception/VehiclePerceptionModelMicro.h"
#include "../microscopic/include/reaction/MicroscopicReactionModel.h"

// Macroscopic components
#include "../macroscopic/include/agents/VehiclePublicLocalStateMacro.h"

// Models
#include "../microscopic/include/IDM.h"
#include "../microscopic/include/MOBIL.h"

// Infrastructure
#include "../kernel/include/model/Point2D.h"
#include "../kernel/include/model/Road.h"

using namespace jamfree;
using namespace jamfree::kernel;
using namespace jamfree::kernel::simulation;
using namespace jamfree::kernel::agents;
using namespace jamfree::kernel::levels;
using namespace jamfree::kernel::model;
using namespace jamfree::microscopic;
using namespace jamfree::macroscopic;

/**
 * @brief Create a microscopic vehicle agent.
 */
std::shared_ptr<VehicleAgent> createMicroscopicAgent(const std::string &id,
                                                     const Point2D &position,
                                                     double speed) {
  auto vehicle = std::make_shared<VehicleAgent>(id);
  vehicle->addLevel(LevelIdentifiers::MICROSCOPIC);

  // Public state
  auto publicState =
      std::make_shared<microscopic::agents::VehiclePublicLocalStateMicro>(id);
  publicState->setPosition(position);
  publicState->setSpeed(speed);
  publicState->setLength(5.0);
  vehicle->setPublicLocalState(LevelIdentifiers::MICROSCOPIC, publicState);

  // Private state
  auto privateState =
      std::make_shared<microscopic::agents::VehiclePrivateLocalStateMicro>(id);
  privateState->setDesiredSpeed(33.3); // 120 km/h
  privateState->setPoliteness(0.5);
  vehicle->setPrivateLocalState(LevelIdentifiers::MICROSCOPIC, privateState);

  // Perception
  auto perception =
      std::make_shared<microscopic::perception::VehiclePerceptionModelMicro>(
          150.0);
  vehicle->setPerceptionModel(LevelIdentifiers::MICROSCOPIC, perception);

  // Decision
  auto idm = std::make_shared<microscopic::models::IDM>();
  auto mobil = std::make_shared<microscopic::models::MOBIL>();

  auto forwardDMS =
      std::make_shared<microscopic::decision::dms::ForwardAccelerationDMS>(idm);
  auto laneChangeDMS =
      std::make_shared<microscopic::decision::dms::LaneChangeDMS>(mobil, idm);

  auto conjunction =
      std::make_shared<microscopic::decision::dms::ConjunctionDMS>();
  conjunction->addSubmodel(forwardDMS);
  conjunction->addSubmodel(laneChangeDMS);

  auto decision =
      std::make_shared<microscopic::decision::VehicleDecisionModelMicro>(
          conjunction);
  vehicle->setDecisionModel(LevelIdentifiers::MICROSCOPIC, decision);

  return vehicle;
}

/**
 * @brief Create a macroscopic flow cell.
 */
std::shared_ptr<VehicleAgent>
createMacroscopicCell(const std::string &id, int cellIndex, double density) {
  auto cell = std::make_shared<VehicleAgent>(id);
  cell->addLevel(LevelIdentifiers::MACROSCOPIC);

  // Public state
  auto publicState =
      std::shared_ptr<macroscopic::agents::VehiclePublicLocalStateMacro>(
          new macroscopic::agents::VehiclePublicLocalStateMacro(id));
  publicState->setCellIndex(cellIndex);
  publicState->setDensity(density);
  publicState->setAverageSpeed(25.0);   // m/s
  publicState->setFlow(density * 25.0); // flow = density * speed
  cell->setPublicLocalState(LevelIdentifiers::MACROSCOPIC, publicState);

  return cell;
}

/**
 * @brief Main example function.
 */
int main() {
  std::cout << "========================================" << std::endl;
  std::cout << "Complete SIMILAR Multi-Level Simulation" << std::endl;
  std::cout << "========================================" << std::endl;
  std::cout << std::endl;

  // ========================================================================
  // Create Simulation Engine
  // ========================================================================
  std::cout << "Creating simulation engine..." << std::endl;
  auto engine = std::make_shared<SimulationEngine>(0.1); // 100ms time step

  // Set up reaction models
  auto microReaction =
      std::make_shared<microscopic::reaction::MicroscopicReactionModel>(0.1);
  microReaction->setSimulationEngine(engine.get());
  engine->setReactionModel(LevelIdentifiers::MICROSCOPIC, microReaction);

  std::cout << "  ✓ Engine created with dt=0.1s" << std::endl;
  std::cout << std::endl;

  // ========================================================================
  // Create Multi-Level Coordinator
  // ========================================================================
  std::cout << "Creating multi-level coordinator..." << std::endl;
  auto coordinator =
      std::shared_ptr<MultiLevelCoordinator>(new MultiLevelCoordinator());
  coordinator->setSimulationEngine(engine);

  // Add levels with different time scales
  MultiLevelCoordinator::LevelConfig microConfig(LevelIdentifiers::MICROSCOPIC,
                                                 0.1, // 100ms time step
                                                 1    // Update every step
  );
  coordinator->addLevel(microConfig);

  MultiLevelCoordinator::LevelConfig macroConfig(LevelIdentifiers::MACROSCOPIC,
                                                 1.0, // 1s time step
                                                 10   // Update every 10 steps
  );
  coordinator->addLevel(macroConfig);

  std::cout << std::endl;

  // ========================================================================
  // Create Agents
  // ========================================================================
  std::cout << "Creating agents..." << std::endl;

  // Microscopic agents
  std::vector<std::shared_ptr<VehicleAgent>> microAgents;
  for (int i = 0; i < 5; ++i) {
    auto agent = createMicroscopicAgent("micro_" + std::to_string(i),
                                        Point2D(i * 50.0, 0.0), 25.0 + i * 2.0);
    microAgents.push_back(agent);
    engine->addAgent(agent);
  }
  std::cout << "  Created " << microAgents.size() << " microscopic agents"
            << std::endl;

  // Macroscopic cells
  std::vector<std::shared_ptr<VehicleAgent>> macroCells;
  for (int i = 0; i < 10; ++i) {
    auto cell = createMacroscopicCell("macro_" + std::to_string(i), i,
                                      0.05 + i * 0.005 // Increasing density
    );
    macroCells.push_back(cell);
    engine->addAgent(cell);
  }
  std::cout << "  Created " << macroCells.size() << " macroscopic cells"
            << std::endl;
  std::cout << std::endl;

  // ========================================================================
  // Run Simulation
  // ========================================================================
  std::cout << "Running simulation..." << std::endl;
  std::cout << std::endl;

  for (int step = 0; step < 20; ++step) {
    std::cout << "=== Step " << step << " (t=" << coordinator->getCurrentTime()
              << "s) ===" << std::endl;

    coordinator->step();

    // Show some agent states
    if (step % 5 == 0) {
      auto agent = microAgents[0];
      auto publicState = std::dynamic_pointer_cast<
          microscopic::agents::VehiclePublicLocalStateMicro>(
          agent->getPublicLocalState(LevelIdentifiers::MICROSCOPIC));

      if (publicState) {
        std::cout << "  Agent " << agent->getId()
                  << ": speed=" << publicState->getSpeed() << " m/s"
                  << ", pos=" << publicState->getPosition().x << "m"
                  << std::endl;
      }
    }

    std::cout << std::endl;
  }

  // ========================================================================
  // Demonstrate Level Transition
  // ========================================================================
  std::cout << "========================================" << std::endl;
  std::cout << "Demonstrating Level Transition" << std::endl;
  std::cout << "========================================" << std::endl;
  std::cout << std::endl;

  // Transition first microscopic agent to macroscopic
  std::cout << "Transitioning agent micro_0 to macroscopic level..."
            << std::endl;
  coordinator->transitionAgent("micro_0", LevelIdentifiers::MICROSCOPIC,
                               LevelIdentifiers::MACROSCOPIC);
  std::cout << std::endl;

  // ========================================================================
  // Summary
  // ========================================================================
  std::cout << "========================================" << std::endl;
  std::cout << "Simulation Complete!" << std::endl;
  std::cout << "========================================" << std::endl;
  std::cout << std::endl;
  std::cout << "Final Statistics:" << std::endl;
  std::cout << "  Total steps: " << coordinator->getStepCount() << std::endl;
  std::cout << "  Final time: " << coordinator->getCurrentTime() << "s"
            << std::endl;
  std::cout << "  Total agents: " << engine->getAgents().size() << std::endl;
  std::cout << std::endl;
  std::cout << "Architecture demonstrated:" << std::endl;
  std::cout << "  ✓ Simulation engine" << std::endl;
  std::cout << "  ✓ Multi-level coordinator" << std::endl;
  std::cout << "  ✓ Microscopic level (0.1s time step)" << std::endl;
  std::cout << "  ✓ Macroscopic level (1.0s time step)" << std::endl;
  std::cout << "  ✓ Level transitions" << std::endl;
  std::cout << "  ✓ Perception-Decision-Action-Reaction cycle" << std::endl;
  std::cout << std::endl;
  std::cout << "This is a complete SIMILAR multi-level simulation!"
            << std::endl;

  return 0;
}
