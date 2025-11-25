/**
 * @file similar_agent_example.cpp
 * @brief Complete example of using SIMILAR agent architecture in JamFree.
 *
 * This example demonstrates:
 * - Creating vehicle agents with SIMILAR architecture
 * - Setting up perception, decision, and reaction models
 * - Running a complete perception-decision-action cycle
 * - Using Decision Sub-Models (DMS) for behavior composition
 */

#include <iostream>
#include <memory>
#include <vector>

// Core agent infrastructure
#include "../kernel/include/agents/VehicleAgent.h"
#include "../kernel/include/levels/LevelIdentifiers.h"

// Microscopic level components
#include "../microscopic/include/agents/VehiclePerceivedDataMicro.h"
#include "../microscopic/include/agents/VehiclePrivateLocalStateMicro.h"
#include "../microscopic/include/agents/VehiclePublicLocalStateMicro.h"
#include "../microscopic/include/decision/VehicleDecisionModelMicro.h"
#include "../microscopic/include/decision/dms/ConjunctionDMS.h"
#include "../microscopic/include/decision/dms/ForwardAccelerationDMS.h"
#include "../microscopic/include/decision/dms/LaneChangeDMS.h"
#include "../microscopic/include/perception/VehiclePerceptionModelMicro.h"
#include "../microscopic/include/reaction/MicroscopicReactionModel.h"

// Models
#include "../microscopic/include/IDM.h"
#include "../microscopic/include/MOBIL.h"

// Infrastructure
#include "../kernel/include/model/Lane.h"
#include "../kernel/include/model/Point2D.h"
#include "../kernel/include/model/Road.h"

using namespace jamfree;
using namespace jamfree::kernel;
using namespace jamfree::kernel::agents;
using namespace jamfree::kernel::levels;
using namespace jamfree::kernel::model;
using namespace jamfree::microscopic;

/**
 * @brief Create a vehicle agent with complete SIMILAR architecture.
 *
 * @param id Vehicle ID
 * @param initialPosition Initial position
 * @param initialSpeed Initial speed (m/s)
 * @return Configured vehicle agent
 */
std::shared_ptr<VehicleAgent> createVehicleAgent(const std::string &id,
                                                 const Point2D &initialPosition,
                                                 double initialSpeed) {
  // Create agent
  auto vehicle = std::make_shared<VehicleAgent>(id);

  // Add microscopic level
  vehicle->addLevel(LevelIdentifiers::MICROSCOPIC);

  // ========================================================================
  // Public Local State (Observable)
  // ========================================================================
  auto publicState = std::make_shared<agents::VehiclePublicLocalStateMicro>();
  publicState->setPosition(initialPosition);
  publicState->setSpeed(initialSpeed);
  publicState->setHeading(0.0);
  publicState->setLength(5.0); // 5m car
  publicState->setWidth(2.0);  // 2m wide
  publicState->setHeight(1.5); // 1.5m tall
  publicState->setActive(true);

  vehicle->setPublicLocalState(LevelIdentifiers::MICROSCOPIC, publicState);

  // ========================================================================
  // Private Local State (Hidden)
  // ========================================================================
  auto privateState = std::make_shared<agents::VehiclePrivateLocalStateMicro>();

  // IDM parameters
  privateState->setDesiredSpeed(33.3);           // 120 km/h
  privateState->setTimeHeadway(1.5);             // 1.5 seconds
  privateState->setMinGap(2.0);                  // 2 meters
  privateState->setMaxAcceleration(2.0);         // 2 m/s²
  privateState->setComfortableDeceleration(3.0); // 3 m/s²
  privateState->setAccelerationExponent(4.0);    // delta = 4

  // MOBIL parameters
  privateState->setPoliteness(0.5);          // Moderately polite
  privateState->setLaneChangeThreshold(0.1); // 0.1 m/s²
  privateState->setMaxSafeDeceleration(4.0); // 4 m/s²
  privateState->setRightLaneBias(0.3);       // 0.3 m/s² (keep-right)

  // Driver characteristics
  privateState->setReactionTime(1.0);   // 1 second
  privateState->setAggressiveness(0.5); // Moderate

  vehicle->setPrivateLocalState(LevelIdentifiers::MICROSCOPIC, privateState);

  // ========================================================================
  // Perception Model
  // ========================================================================
  auto perceptionModel =
      std::make_shared<perception::VehiclePerceptionModelMicro>(150.0);
  vehicle->setPerceptionModel(LevelIdentifiers::MICROSCOPIC, perceptionModel);

  // ========================================================================
  // Decision Model with DMS
  // ========================================================================

  // Create IDM and MOBIL models
  auto idm = std::make_shared<models::IDM>();
  auto mobil = std::make_shared<models::MOBIL>();

  // Create Decision Sub-Models
  auto forwardAccelerationDMS =
      std::make_shared<decision::dms::ForwardAccelerationDMS>(idm);
  auto laneChangeDMS =
      std::make_shared<decision::dms::LaneChangeDMS>(mobil, idm);

  // Combine DMS using Conjunction (both behaviors execute)
  auto conjunctionDMS = std::make_shared<decision::dms::ConjunctionDMS>();
  conjunctionDMS->addSubmodel(forwardAccelerationDMS);
  conjunctionDMS->addSubmodel(laneChangeDMS);

  // Create decision model
  auto decisionModel =
      std::make_shared<decision::VehicleDecisionModelMicro>(conjunctionDMS);
  vehicle->setDecisionModel(LevelIdentifiers::MICROSCOPIC, decisionModel);

  return vehicle;
}

/**
 * @brief Run one simulation step for an agent.
 *
 * @param vehicle Vehicle agent
 * @param dt Time step (seconds)
 * @param globalState Global simulation state
 * @return Influences produced by the agent
 */
InfluencesMap runAgentStep(std::shared_ptr<VehicleAgent> vehicle, double dt,
                           const GlobalState &globalState) {
  // Time bounds
  SimulationTimeStamp t0(0.0);
  SimulationTimeStamp t1(dt);

  // Get states
  auto publicState =
      vehicle->getPublicLocalState(LevelIdentifiers::MICROSCOPIC);
  auto privateState =
      vehicle->getPrivateLocalState(LevelIdentifiers::MICROSCOPIC);

  // ========================================================================
  // PERCEPTION
  // ========================================================================
  auto perceivedData = std::make_shared<agents::VehiclePerceivedDataMicro>();
  auto perceptionModel =
      vehicle->getPerceptionModel(LevelIdentifiers::MICROSCOPIC);

  perceptionModel->perceive(t0, t1, globalState, *publicState, *privateState,
                            *perceivedData);

  std::cout << "  Perceived: leader gap=" << perceivedData->getGapToLeader()
            << "m, speed limit=" << perceivedData->getCurrentSpeedLimit()
            << " m/s" << std::endl;

  // ========================================================================
  // DECISION
  // ========================================================================
  InfluencesMap influences;
  auto decisionModel = vehicle->getDecisionModel(LevelIdentifiers::MICROSCOPIC);

  decisionModel->decide(t0, t1, globalState, *publicState, *privateState,
                        *perceivedData, influences);

  std::cout << "  Decided: " << influences.size() << " influences produced"
            << std::endl;

  return influences;
}

/**
 * @brief Main example function.
 */
int main() {
  std::cout << "========================================" << std::endl;
  std::cout << "SIMILAR Agent Architecture Example" << std::endl;
  std::cout << "========================================" << std::endl;
  std::cout << std::endl;

  // ========================================================================
  // Create Infrastructure
  // ========================================================================
  std::cout << "Creating road infrastructure..." << std::endl;

  // Create a simple straight road with 2 lanes
  auto road =
      std::make_shared<Road>("road_001", Point2D(0, 0), Point2D(1000, 0),
                             2,  // 2 lanes
                             3.5 // 3.5m lane width
      );

  std::cout << "  Road: " << road->getId() << " (" << road->getNumLanes()
            << " lanes, " << road->getLength() << "m)" << std::endl;
  std::cout << std::endl;

  // ========================================================================
  // Create Agents
  // ========================================================================
  std::cout << "Creating vehicle agents..." << std::endl;

  std::vector<std::shared_ptr<VehicleAgent>> vehicles;

  // Vehicle 1: Leading vehicle
  auto vehicle1 = createVehicleAgent("vehicle_001", Point2D(500, 0), 25.0);
  vehicles.push_back(vehicle1);
  std::cout << "  Created: " << vehicle1->getId() << " (leader)" << std::endl;

  // Vehicle 2: Following vehicle
  auto vehicle2 = createVehicleAgent("vehicle_002", Point2D(450, 0), 30.0);
  vehicles.push_back(vehicle2);
  std::cout << "  Created: " << vehicle2->getId() << " (follower)" << std::endl;

  std::cout << std::endl;

  // ========================================================================
  // Run Simulation Steps
  // ========================================================================
  std::cout << "Running simulation steps..." << std::endl;
  std::cout << std::endl;

  // Global state (placeholder)
  class SimpleGlobalState : public GlobalState {
  public:
    SimulationTimeStamp getCurrentTime() const override {
      return SimulationTimeStamp(m_time);
    }
    void setTime(double t) { m_time = t; }

  private:
    double m_time = 0.0;
  };

  SimpleGlobalState globalState;

  // Create reaction model
  auto reactionModel =
      std::make_shared<reaction::MicroscopicReactionModel>(0.1);

  // Run 5 simulation steps
  for (int step = 0; step < 5; ++step) {
    std::cout << "Step " << step << ":" << std::endl;

    globalState.setTime(step * 0.1);

    // Collect all influences
    InfluencesMap allInfluences;

    // Run each agent
    for (auto &vehicle : vehicles) {
      std::cout << "Agent " << vehicle->getId() << ":" << std::endl;

      auto influences = runAgentStep(vehicle, 0.1, globalState);

      // Merge influences
      for (const auto &influence : influences.getInfluences()) {
        allInfluences.add(influence);
      }
    }

    // Apply reactions
    std::cout << "Reaction:" << std::endl;
    reactionModel->react(SimulationTimeStamp(step * 0.1),
                         SimulationTimeStamp((step + 1) * 0.1), allInfluences);

    std::cout << std::endl;
  }

  // ========================================================================
  // Summary
  // ========================================================================
  std::cout << "========================================" << std::endl;
  std::cout << "Simulation Complete!" << std::endl;
  std::cout << "========================================" << std::endl;
  std::cout << std::endl;
  std::cout << "Architecture used:" << std::endl;
  std::cout << "  ✓ VehicleAgent (multi-level)" << std::endl;
  std::cout << "  ✓ Public/Private local states" << std::endl;
  std::cout << "  ✓ Perception model" << std::endl;
  std::cout << "  ✓ Decision model with DMS" << std::endl;
  std::cout << "  ✓ Influences (ChangeAcceleration, ChangeLane)" << std::endl;
  std::cout << "  ✓ Reaction model" << std::endl;
  std::cout << std::endl;
  std::cout << "This demonstrates the complete SIMILAR" << std::endl;
  std::cout << "Perception-Decision-Action cycle!" << std::endl;

  return 0;
}
