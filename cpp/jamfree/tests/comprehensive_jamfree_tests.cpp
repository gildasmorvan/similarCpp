#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <limits>

// JamFree Core Kernel includes
#include "../kernel/include/Simulation.h"
#include "../kernel/include/model/Vehicle.h"
#include "../kernel/include/model/Road.h"
#include "../kernel/include/model/Lane.h"
#include "../kernel/include/model/Point2D.h"
#include "../kernel/include/model/SpatialIndex.h"
#include "../kernel/include/model/TrafficControl.h"
#include "../kernel/include/agents/VehicleAgent.h"
#include "../kernel/include/agents/Interfaces.h"
#include "../kernel/include/influences/AccelerationInfluence.h"
#include "../kernel/include/influences/LaneChangeInfluence.h"
#include "../kernel/include/influences/PositionUpdateInfluence.h"
#include "../kernel/include/tools/MathTools.h"
#include "../kernel/include/tools/GeometryTools.h"
#include "../kernel/include/tools/FastMath.h"
#include "../kernel/include/routing/Router.h"
#include "../kernel/include/simulation/SimulationEngine.h"
#include "../kernel/include/simulation/TrafficLevel.h"
#include "../kernel/include/simulation/TrafficSimulationModel.h"
#include "../kernel/include/simulation/MultiLevelCoordinator.h"
#include "../kernel/include/reaction/TrafficReactionModel.h"

// Microscopic Models
#include "../microscopic/include/IDM.h"
#include "../microscopic/include/IDMEnhanced.h"
#include "../microscopic/include/MOBIL.h"
#include "../microscopic/include/agents/VehicleDecisionModelMicro.h"
#include "../microscopic/include/agents/VehiclePerceptionModelMicro.h"
#include "../microscopic/include/decision/dms/IDecisionMicroSubmodel.h"
#include "../microscopic/include/decision/dms/ConjunctionDMS.h"
#include "../microscopic/include/decision/dms/ForwardAccelerationDMS.h"
#include "../microscopic/include/decision/dms/LaneChangeDMS.h"
#include "../microscopic/include/decision/dms/SubsumptionDMS.h"

// Macroscopic Models
#include "../macroscopic/include/CTM.h"
#include "../macroscopic/include/LWR.h"
#include "../macroscopic/include/MicroMacroBridge.h"

// Hybrid Simulation
#include "../hybrid/include/AdaptiveSimulator.h"

// Namespace aliases
namespace jf = jamfree;
namespace jfk = jamfree::kernel;
namespace jfm = jamfree::microscopic;
namespace jfmac = jamfree::macroscopic;

// Test Point2D class (JamFree version)
void testJamFreePoint2D() {
    std::cout << "Testing JamFree Point2D class..." << std::endl;

    // Default constructor
    jfk::model::Point2D p1;
    assert(p1.x == 0.0 && p1.y == 0.0);

    // Parameterized constructor
    jfk::model::Point2D p2(3.0, 4.0);
    assert(p2.x == 3.0 && p2.y == 4.0);

    // Arithmetic operators
    jfk::model::Point2D p4(1.0, 2.0);
    jfk::model::Point2D p5(2.0, 3.0);

    auto sum = p4 + p5;
    assert(sum.x == 3.0 && sum.y == 5.0);

    auto diff = p5 - p4;
    assert(diff.x == 1.0 && diff.y == 1.0);

    auto scaled = p4 * 2.0;
    assert(scaled.x == 2.0 && scaled.y == 4.0);

    // Distance and geometric operations
    jfk::model::Point2D origin(0.0, 0.0);
    jfk::model::Point2D p10(3.0, 4.0);

    assert(std::abs(p10.distanceTo(p2) - 1.0) < 1e-9);
    assert(std::abs(p10.magnitude() - 5.0) < 1e-9);

    std::cout << "JamFree Point2D tests PASSED" << std::endl;
}

// Test Vehicle class
void testVehicle() {
    std::cout << "Testing Vehicle class..." << std::endl;

    // Constructor
    jfk::model::Vehicle vehicle("test_vehicle", 4.5, 50.0, 2.0, 5.0);

    // Test identity
    assert(vehicle.getId() == "test_vehicle");

    // Test properties
    assert(std::abs(vehicle.getLength() - 4.5) < 1e-9);
    assert(std::abs(vehicle.getWidth() - 2.0) < 1e-9);
    assert(std::abs(vehicle.getMaxSpeed() - 50.0) < 1e-9);
    assert(std::abs(vehicle.getMaxAccel() - 2.0) < 1e-9);
    assert(std::abs(vehicle.getMaxDecel() - 5.0) < 1e-9);

    // Test initial state
    assert(vehicle.getSpeed() == 0.0);
    assert(vehicle.getAcceleration() == 0.0);
    assert(vehicle.getLanePosition() == 0.0);

    // Test setters
    vehicle.setSpeed(25.0);
    assert(vehicle.getSpeed() == 25.0);

    vehicle.setAcceleration(1.5);
    assert(vehicle.getAcceleration() == 1.5);

    vehicle.setLanePosition(100.0);
    assert(vehicle.getLanePosition() == 100.0);

    // Test speed clamping
    vehicle.setSpeed(-5.0);
    assert(vehicle.getSpeed() == 0.0);

    vehicle.setSpeed(60.0); // Above max
    assert(vehicle.getSpeed() == 50.0); // Clamped to max

    // Test position calculations
    jfk::model::Point2D pos(10.0, 20.0);
    vehicle.setPosition(pos);
    assert(vehicle.getPosition() == pos);

    vehicle.setHeading(M_PI/2); // 90 degrees
    auto front = vehicle.getFrontPosition();
    assert(std::abs(front.x - (10.0 + 4.5 * 0.0)) < 1e-9); // cos(π/2) = 0
    assert(std::abs(front.y - (20.0 + 4.5 * 1.0)) < 1e-9); // sin(π/2) = 1

    // Test stopped check
    vehicle.setSpeed(0.05); // Below default threshold
    assert(vehicle.isStopped());

    vehicle.setSpeed(0.2); // Above threshold
    assert(!vehicle.isStopped());

    std::cout << "Vehicle tests PASSED" << std::endl;
}

// Test Road and Lane classes
void testRoadAndLane() {
    std::cout << "Testing Road and Lane classes..." << std::endl;

    // Create road
    jfk::model::Point2D start(0.0, 0.0);
    jfk::model::Point2D end(100.0, 0.0);
    jfk::model::Road road("test_road", start, end, 2, 3.5);

    // Test road properties
    assert(road.getId() == "test_road");
    assert(road.getStart() == start);
    assert(road.getEnd() == end);
    assert(road.getNumLanes() == 2);
    assert(std::abs(road.getLaneWidth() - 3.5) < 1e-9);

    // Test lane access
    auto lane0 = road.getLane(0);
    auto lane1 = road.getLane(1);
    auto laneInvalid = road.getLane(2);

    assert(lane0 != nullptr);
    assert(lane1 != nullptr);
    assert(laneInvalid == nullptr);

    // Test lane properties
    assert(lane0->getIndex() == 0);
    assert(lane1->getIndex() == 1);
    assert(std::abs(lane0->getWidth() - 3.5) < 1e-9);
    assert(std::abs(lane0->getLength() - 100.0) < 1e-9);

    // Test position calculation (straight road)
    auto pos0 = lane0->getPositionAt(0.0);
    auto pos50 = lane0->getPositionAt(50.0);
    auto pos100 = lane0->getPositionAt(100.0);

    assert(pos0 == start);
    assert(std::abs(pos50.x - 50.0) < 1e-9 && std::abs(pos50.y - 0.0) < 1e-9);
    assert(pos100 == end);

    // Test heading (straight road)
    auto heading0 = lane0->getHeadingAt(0.0);
    auto heading50 = lane0->getHeadingAt(50.0);
    assert(std::abs(heading0 - 0.0) < 1e-9); // East
    assert(std::abs(heading50 - 0.0) < 1e-9);

    std::cout << "Road and Lane tests PASSED" << std::endl;
}

// Test IDM (Intelligent Driver Model)
void testIDM() {
    std::cout << "Testing IDM class..." << std::endl;

    // Create IDM with custom parameters
    jfm::models::IDM idm(30.0, 1.5, 2.0, 1.0, 2.0, 4.0);

    // Create vehicles
    jfk::model::Vehicle ego("ego");
    ego.setSpeed(20.0);
    ego.setLanePosition(50.0);

    jfk::model::Vehicle leader("leader");
    leader.setSpeed(15.0);
    leader.setLanePosition(60.0); // 10m ahead

    // Test free-flow acceleration (no leader)
    double accel_free = idm.calculateAcceleration(ego, nullptr);
    assert(accel_free > 0.0); // Should accelerate toward desired speed

    // Test with leader
    double accel_with_leader = idm.calculateAcceleration(ego, &leader);
    assert(accel_with_leader < accel_free); // Should decelerate

    // Test desired gap calculation
    double desired_gap = idm.calculateDesiredGap(20.0, 5.0); // 20 m/s, approaching at 5 m/s
    assert(desired_gap > 2.0); // Should be greater than minimum gap

    // Test zero relative speed
    double gap_same_speed = idm.calculateDesiredGap(20.0, 0.0);
    assert(gap_same_speed > 0.0);

    std::cout << "IDM tests PASSED" << std::endl;
}

// Test MOBIL (Lane-changing model)
void testMOBIL() {
    std::cout << "Testing MOBIL class..." << std::endl;

    // Create MOBIL
    jfm::models::MOBIL mobil(0.5, 0.1, 4.0, 0.3);

    // Create road with lanes
    jfk::model::Point2D start(0.0, 0.0);
    jfk::model::Point2D end(1000.0, 0.0);
    jfk::model::Road road("test_road", start, end, 3, 3.5);

    auto lane0 = road.getLane(0); // Left
    auto lane1 = road.getLane(1); // Middle
    auto lane2 = road.getLane(2); // Right

    // Create ego vehicle in middle lane
    jfk::model::Vehicle ego("ego");
    ego.setCurrentLane(lane1);
    ego.setLanePosition(500.0);
    ego.setSpeed(25.0);
    lane1->addVehicle(ego);

    // Test lane change decision with empty lanes
    jfm::models::IDM idm;
    auto decision = mobil.decideLaneChange(ego, *lane1, lane0.get(), lane2.get(), idm);

    // Should prefer right lane due to bias
    assert(decision == jfm::models::MOBIL::Direction::RIGHT);

    // Test safety criterion - create unsafe situation
    jfk::model::Vehicle follower("follower");
    follower.setCurrentLane(lane2);
    follower.setLanePosition(480.0); // Close behind
    follower.setSpeed(30.0); // Faster
    lane2->addVehicle(follower);

    // Ego wants to move right but follower would have to brake hard
    auto unsafe_decision = mobil.decideLaneChange(ego, *lane1, nullptr, lane2.get(), idm);
    assert(unsafe_decision == jfm::models::MOBIL::Direction::NONE);

    std::cout << "MOBIL tests PASSED" << std::endl;
}

// Test MathTools
void testMathTools() {
    std::cout << "Testing MathTools class..." << std::endl;

    // Test square function
    assert(jfk::tools::MathTools::square(3.0) == 9.0);
    assert(jfk::tools::MathTools::square(-2.0) == 4.0);
    assert(jfk::tools::MathTools::square(0.0) == 0.0);

    // Test clamp
    assert(jfk::tools::MathTools::clamp(5.0, 0.0, 10.0) == 5.0);
    assert(jfk::tools::MathTools::clamp(-5.0, 0.0, 10.0) == 0.0);
    assert(jfk::tools::MathTools::clamp(15.0, 0.0, 10.0) == 10.0);

    // Test sign
    assert(jfk::tools::MathTools::sign(5.0) == 1);
    assert(jfk::tools::MathTools::sign(-3.0) == -1);
    assert(jfk::tools::MathTools::sign(0.0) == 0);

    std::cout << "MathTools tests PASSED" << std::endl;
}

// Test GeometryTools
void testGeometryTools() {
    std::cout << "Testing GeometryTools class..." << std::endl;

    jfk::model::Point2D p1(0.0, 0.0);
    jfk::model::Point2D p2(3.0, 4.0);

    // Test distance
    double dist = jfk::tools::GeometryTools::distance(p1, p2);
    assert(std::abs(dist - 5.0) < 1e-9);

    // Test point-line distance
    jfk::model::Point2D line_start(0.0, 0.0);
    jfk::model::Point2D line_end(10.0, 0.0);
    jfk::model::Point2D point(5.0, 3.0);

    double line_dist = jfk::tools::GeometryTools::pointToLineDistance(point, line_start, line_end);
    assert(std::abs(line_dist - 3.0) < 1e-9);

    std::cout << "GeometryTools tests PASSED" << std::endl;
}

// Test FastMath
void testJamFreeFastMath() {
    std::cout << "Testing JamFree FastMath class..." << std::endl;

    // Test basic trig functions
    for (double angle = 0.0; angle < 2*M_PI; angle += 0.5) {
        double std_sin = std::sin(angle);
        double fast_sin = jfk::tools::FastMath::sin(angle);
        assert(std::abs(std_sin - fast_sin) < 0.01);

        double std_cos = std::cos(angle);
        double fast_cos = jfk::tools::FastMath::cos(angle);
        assert(std::abs(std_cos - fast_cos) < 0.01);
    }

    // Test sqrt
    assert(std::abs(jfk::tools::FastMath::sqrt(4.0) - 2.0) < 1e-9);
    assert(std::abs(jfk::tools::FastMath::sqrt(9.0) - 3.0) < 1e-9);

    std::cout << "JamFree FastMath tests PASSED" << std::endl;
}

// Test influences
void testInfluences() {
    std::cout << "Testing influence classes..." << std::endl;

    // Create test vehicle
    jfk::model::Vehicle vehicle("test_vehicle");

    // Test AccelerationInfluence
    jfk::influences::AccelerationInfluence accel_inf(0, 1, vehicle, 2.5);
    assert(accel_inf.getAcceleration() == 2.5);

    // Test PositionUpdateInfluence
    jfk::influences::PositionUpdateInfluence pos_inf(0, 1, vehicle, 10.0, 20.0);
    assert(pos_inf.getDx() == 10.0);
    assert(pos_inf.getDy() == 20.0);

    // Test LaneChangeInfluence
    jfk::model::Road road("test_road", jfk::model::Point2D(0,0), jfk::model::Point2D(100,0), 2, 3.5);
    auto lane1 = road.getLane(1);
    jfk::influences::LaneChangeInfluence lane_inf(0, 1, vehicle, lane1);
    assert(lane_inf.getTargetLane() == lane1);

    std::cout << "Influence tests PASSED" << std::endl;
}

// Test macroscopic models
void testMacroscopicModels() {
    std::cout << "Testing macroscopic models..." << std::endl;

    // Test CTM (Cell Transmission Model)
    jfmac::CTM ctm(1.0, 0.8, 2000.0, 1800.0); // vmax, rhoc, qmax, rhomax

    // Test basic properties
    assert(std::abs(ctm.getMaxSpeed() - 1.0) < 1e-9);
    assert(std::abs(ctm.getMaxDensity() - 0.8) < 1e-9);

    // Test flow calculation
    double flow = ctm.flow(0.4); // At density 0.4
    assert(flow >= 0.0);

    // Test LWR (Lighthill-Whitham-Richards) model
    jfmac::LWR lwr(1.0, 0.8);

    // Test propagation speed
    double speed = lwr.getPropagationSpeed();
    assert(speed > 0.0);

    std::cout << "Macroscopic model tests PASSED" << std::endl;
}

// Test spatial index
void testSpatialIndex() {
    std::cout << "Testing SpatialIndex class..." << std::endl;

    jfk::model::SpatialIndex index;

    // Add some vehicles
    jfk::model::Vehicle v1("v1");
    v1.setPosition(jfk::model::Point2D(10.0, 10.0));

    jfk::model::Vehicle v2("v2");
    v2.setPosition(jfk::model::Point2D(20.0, 20.0));

    jfk::model::Vehicle v3("v3");
    v3.setPosition(jfk::model::Point2D(50.0, 50.0));

    index.add(&v1);
    index.add(&v2);
    index.add(&v3);

    // Query nearby vehicles
    auto nearby = index.queryRadius(15.0, 15.0, 10.0);
    assert(nearby.size() == 2); // v1 and v2

    // Query farther
    auto farther = index.queryRadius(50.0, 50.0, 5.0);
    assert(farther.size() == 1); // v3

    // Clear and test
    index.clear();
    auto empty = index.queryRadius(0.0, 0.0, 100.0);
    assert(empty.empty());

    std::cout << "SpatialIndex tests PASSED" << std::endl;
}

// Main test runner
int main() {
    std::cout << "Running comprehensive JamFree C++ unit tests..." << std::endl;
    std::cout << "==================================================" << std::endl;

    try {
        // Core infrastructure
        testJamFreePoint2D();
        testMathTools();
        testGeometryTools();
        testJamFreeFastMath();

        // Traffic models
        testVehicle();
        testRoadAndLane();
        testSpatialIndex();

        // Microscopic models
        testIDM();
        testMOBIL();

        // Influences and reactions
        testInfluences();

        // Macroscopic models
        testMacroscopicModels();

        std::cout << "==================================================" << std::endl;
        std::cout << "ALL JAMFREE C++ TESTS PASSED! 🚗✨" << std::endl;
        std::cout << "Comprehensive test coverage of traffic simulation components" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }

    return 0;
}
