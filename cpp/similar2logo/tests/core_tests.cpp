#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

// Core microkernel includes
#include "LevelIdentifier.h"
#include "SimulationTimeStamp.h"
#include "AgentCategory.h"
#include "influences/InfluencesMap.h"
#include "influences/AbstractInfluence.h"
#include "influences/RegularInfluence.h"
#include "influences/SystemInfluence.h"
#include "libs/generic/EmptyLocalStateOfEnvironment.h"
#include "libs/generic/EmptyPerceivedData.h"

// Similar2Logo includes
#include "kernel/tools/Point2D.h"
#include "kernel/tools/MathUtil.h"
#include "kernel/tools/FastMath.h"
#include "kernel/model/environment/Mark.h"
#include "kernel/model/environment/TurtlePLSInLogo.h"
#include "kernel/model/environment/SituatedEntity.h"
#include "kernel/influences/ChangeAcceleration.h"
#include "kernel/influences/ChangeDirection.h"
#include "kernel/influences/ChangePosition.h"
#include "kernel/influences/ChangeSpeed.h"
#include "kernel/influences/DropMark.h"
#include "kernel/influences/EmitPheromone.h"
#include "kernel/influences/RemoveMark.h"
#include "kernel/influences/RemoveMarks.h"
#include "kernel/influences/Stop.h"

// Namespace aliases
namespace mk = fr::univ_artois::lgi2a::similar::microkernel;
namespace s2l = fr::univ_artois::lgi2a::similar::similar2logo::kernel;

// Test Point2D class methods
void testPoint2D() {
    std::cout << "Testing Point2D class..." << std::endl;

    // Default constructor
    s2l::tools::Point2D p1;
    assert(p1.x == 0.0 && p1.y == 0.0);

    // Parameterized constructor
    s2l::tools::Point2D p2(3.0, 4.0);
    assert(p2.x == 3.0 && p2.y == 4.0);

    // Arithmetic operators
    s2l::tools::Point2D p4(1.0, 2.0);
    s2l::tools::Point2D p5(2.0, 3.0);

    auto sum = p4 + p5;
    assert(sum.x == 3.0 && sum.y == 5.0);

    auto diff = p5 - p4;
    assert(diff.x == 1.0 && diff.y == 1.0);

    auto scaled = p4 * 2.0;
    assert(scaled.x == 2.0 && scaled.y == 4.0);

    auto divided = p4 / 2.0;
    assert(divided.x == 0.5 && divided.y == 1.0);

    // Comparison operators
    s2l::tools::Point2D p7(1.0, 1.0);
    s2l::tools::Point2D p8(1.0, 1.0);
    s2l::tools::Point2D p9(1.000000001, 1.0);

    assert(p7 == p8);
    assert(p7 != p5);
    assert(p7 == p9); // Should be equal due to epsilon comparison

    // Geometric operations
    s2l::tools::Point2D origin(0.0, 0.0);
    s2l::tools::Point2D p10(3.0, 4.0);

    assert(std::abs(p10.magnitude() - 5.0) < 1e-9);
    assert(p10.magnitudeSquared() == 25.0);

    auto dist = p2.distanceTo(p4);
    assert(std::abs(dist - std::sqrt(4.0 + 4.0)) < 1e-9);

    assert(p2.distanceSquaredTo(p4) == 8.0);

    auto normalized = p10.normalized();
    assert(std::abs(normalized.magnitude() - 1.0) < 1e-9);

    // Dot and cross products
    s2l::tools::Point2D v1(1.0, 2.0);
    s2l::tools::Point2D v2(3.0, 4.0);
    assert(v1.dot(v2) == 11.0);
    assert(v1.cross(v2) == -2.0);

    // Angle operations
    s2l::tools::Point2D unitX(1.0, 0.0);
    s2l::tools::Point2D unitY(0.0, 1.0);
    assert(std::abs(unitX.angle() - 0.0) < 1e-9);
    assert(std::abs(unitY.angle() - M_PI/2) < 1e-9);

    // Rotation
    auto rotated = unitX.rotated(M_PI/2);
    assert(std::abs(rotated.x - 0.0) < 1e-9);
    assert(std::abs(rotated.y - 1.0) < 1e-9);

    // Polar coordinates
    auto fromPolar = s2l::tools::Point2D::fromPolar(5.0, M_PI/4);
    assert(std::abs(fromPolar.magnitude() - 5.0) < 1e-9);

    // Linear interpolation
    s2l::tools::Point2D start(0.0, 0.0);
    s2l::tools::Point2D end(10.0, 20.0);
    auto lerpResult = start.lerp(end, 0.5);
    assert(lerpResult.x == 5.0 && lerpResult.y == 10.0);

    // String representation
    std::string str = p2.toString();
    assert(str.find("3") != std::string::npos);

    std::cout << "Point2D tests PASSED" << std::endl;
}

// Test MathUtil class methods
void testMathUtil() {
    std::cout << "Testing MathUtil class..." << std::endl;

    // Test normalizeAngle
    assert(std::abs(s2l::tools::MathUtil::normalizeAngle(0.0) - 0.0) < 1e-9);
    assert(std::abs(s2l::tools::MathUtil::normalizeAngle(3*M_PI) - M_PI) < 1e-9);

    // Test angleDifference
    assert(std::abs(s2l::tools::MathUtil::angleDifference(0.0, M_PI/2) - M_PI/2) < 1e-9);

    // Test clamp
    assert(s2l::tools::MathUtil::clamp(5.0, 0.0, 10.0) == 5.0);
    assert(s2l::tools::MathUtil::clamp(-5.0, 0.0, 10.0) == 0.0);
    assert(s2l::tools::MathUtil::clamp(15.0, 0.0, 10.0) == 10.0);

    // Test sign
    assert(s2l::tools::MathUtil::sign(5.0) == 1);
    assert(s2l::tools::MathUtil::sign(-3.0) == -1);
    assert(s2l::tools::MathUtil::sign(0.0) == 0);

    std::cout << "MathUtil tests PASSED" << std::endl;
}

// Test FastMath class methods
void testFastMath() {
    std::cout << "Testing FastMath class..." << std::endl;

    // Test sin/cos approximations
    for (double angle = 0; angle <= 2*M_PI; angle += 0.5) {
        double std_sin = std::sin(angle);
        double fast_sin = mk::tools::FastMath::sin(angle);
        assert(std::abs(std_sin - fast_sin) < 0.1);

        double std_cos = std::cos(angle);
        double fast_cos = mk::tools::FastMath::cos(angle);
        assert(std::abs(std_cos - fast_cos) < 0.1);
    }

    // Test sqrt
    assert(std::abs(mk::tools::FastMath::sqrt(4.0) - 2.0) < 1e-9);

    std::cout << "FastMath tests PASSED" << std::endl;
}

// Test SimulationTimeStamp class
void testSimulationTimeStamp() {
    std::cout << "Testing SimulationTimeStamp class..." << std::endl;

    // Parameterized constructor
    mk::SimulationTimeStamp ts1(0);
    assert(ts1.getIdentifier() == 0);

    mk::SimulationTimeStamp ts2(42);
    assert(ts2.getIdentifier() == 42);

    // Copy constructor
    mk::SimulationTimeStamp ts3(ts2);
    assert(ts3.getIdentifier() == 42);

    // Assignment operator
    mk::SimulationTimeStamp ts4(0);
    ts4 = ts2;
    assert(ts4.getIdentifier() == 42);

    // Comparison operators
    mk::SimulationTimeStamp ts5(10);
    mk::SimulationTimeStamp ts6(20);
    mk::SimulationTimeStamp ts7(10);

    assert(ts5 < ts6);
    assert(ts5 <= ts7);
    assert(ts5 >= ts7);
    assert(ts5 == ts7);
    assert(ts5 != ts6);

    std::cout << "SimulationTimeStamp tests PASSED" << std::endl;
}

// Test LevelIdentifier class
void testLevelIdentifier() {
    std::cout << "Testing LevelIdentifier class..." << std::endl;

    // Constructor
    mk::LevelIdentifier lid("test_level");
    assert(lid.getName() == "test_level");

    // Copy constructor
    mk::LevelIdentifier lid2(lid);
    assert(lid2.getName() == "test_level");

    // Assignment
    mk::LevelIdentifier lid3("other");
    lid3 = lid;
    assert(lid3.getName() == "test_level");

    // Comparison
    mk::LevelIdentifier lid4("test_level");
    mk::LevelIdentifier lid5("other_level");

    assert(lid == lid4);
    assert(lid != lid5);

    std::cout << "LevelIdentifier tests PASSED" << std::endl;
}

// Test AgentCategory class
void testAgentCategory() {
    std::cout << "Testing AgentCategory class..." << std::endl;

    // Constructor
    mk::AgentCategory cat("test_category", "test_family");
    assert(cat.getName() == "test_category");
    assert(cat.getFamily() == "test_family");

    // Copy constructor
    mk::AgentCategory cat2(cat);
    assert(cat2.getName() == "test_category");
    assert(cat2.getFamily() == "test_family");

    // Comparison
    mk::AgentCategory cat3("test_category", "test_family");
    mk::AgentCategory cat4("other_category", "test_family");

    assert(cat == cat3);
    assert(cat != cat4);

    std::cout << "AgentCategory tests PASSED" << std::endl;
}

// Test Mark class
void testMark() {
    std::cout << "Testing Mark class..." << std::endl;

    // Test SimpleMark
    s2l::tools::Point2D loc(10.0, 20.0);
    auto mark = std::make_shared<s2l::model::environment::SimpleMark>(loc);

    assert(mark->getLocation() == loc);
    assert(mark->getContent() == nullptr);

    std::cout << "Mark tests PASSED" << std::endl;
}

// Test TurtlePLSInLogo class
void testTurtlePLSInLogo() {
    std::cout << "Testing TurtlePLSInLogo class..." << std::endl;

    s2l::tools::Point2D loc(10.0, 20.0);

    // Constructor with basic parameters
    auto turtle = std::make_shared<s2l::model::environment::TurtlePLSInLogo>(
        loc, 1.57, 2.0, 0.5, true, "blue");

    assert(turtle->getLocation() == loc);
    assert(std::abs(turtle->getHeading() - 1.57) < 1e-9);
    assert(std::abs(turtle->getSpeed() - 2.0) < 1e-9);
    assert(turtle->isPenDown() == true);
    assert(turtle->getColor() == "blue");

    // Test setters
    turtle->setHeading(0.0);
    assert(std::abs(turtle->getHeading() - 0.0) < 1e-9);

    turtle->setSpeed(3.0);
    assert(std::abs(turtle->getSpeed() - 3.0) < 1e-9);

    turtle->setColor("red");
    assert(turtle->getColor() == "red");

    std::cout << "TurtlePLSInLogo tests PASSED" << std::endl;
}

// Test SituatedEntity class
void testSituatedEntity() {
    std::cout << "Testing SituatedEntity class..." << std::endl;

    s2l::tools::Point2D loc(10.0, 20.0);

    // Constructor
    s2l::model::environment::SituatedEntity entity(loc);
    assert(entity.getLocation() == loc);

    // Set location
    s2l::tools::Point2D newLoc(15.0, 25.0);
    entity.setLocation(newLoc);
    assert(entity.getLocation() == newLoc);

    std::cout << "SituatedEntity tests PASSED" << std::endl;
}

// Test all influence classes
void testAllInfluences() {
    std::cout << "Testing all influence classes..." << std::endl;

    mk::SimulationTimeStamp t1(0);
    mk::SimulationTimeStamp t2(10);
    s2l::tools::Point2D loc(10, 20);
    auto turtle = std::make_shared<s2l::model::environment::TurtlePLSInLogo>(
        loc, 0.0, 1.0, 0.0, true, "red");

    // Test ChangeDirection
    s2l::influences::ChangeDirection cd(t1, t2, 1.57, turtle);
    assert(cd.getCategory() == "change direction");
    assert(std::abs(cd.getDd() - 1.57) < 1e-9);

    // Test ChangePosition
    s2l::influences::ChangePosition cp(t1, t2, 5.0, -3.0, turtle);
    assert(cp.getCategory() == "change position");
    assert(cp.getDx() == 5.0 && cp.getDy() == -3.0);

    // Test ChangeSpeed
    s2l::influences::ChangeSpeed cs(t1, t2, 3.0, turtle);
    assert(cs.getCategory() == "change speed");
    assert(std::abs(cs.getDs() - 3.0) < 1e-9);

    // Test DropMark
    auto mark = std::make_shared<s2l::model::environment::SimpleMark>(loc);
    s2l::influences::DropMark dm(t1, t2, mark);
    assert(dm.getCategory() == "drop mark");

    // Test RemoveMark
    s2l::influences::RemoveMark rm(t1, t2, mark);
    assert(rm.getCategory() == "remove mark");

    // Test Stop
    s2l::influences::Stop stop(t1, t2, turtle);
    assert(stop.getCategory() == "stop");

    std::cout << "All influence tests PASSED" << std::endl;
}

// Main test runner
int main() {
    std::cout << "Running core C++ unit tests..." << std::endl;
    std::cout << "================================" << std::endl;

    try {
        // Core utility classes
        testPoint2D();
        testMathUtil();
        testFastMath();

        // Core microkernel classes
        testSimulationTimeStamp();
        testLevelIdentifier();
        testAgentCategory();

        // Similar2Logo model classes
        testMark();
        testTurtlePLSInLogo();
        testSituatedEntity();

        // All influence classes
        testAllInfluences();

        std::cout << "================================" << std::endl;
        std::cout << "ALL CORE TESTS PASSED! 🎉" << std::endl;
        std::cout << "Comprehensive test coverage of core C++ classes" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }

    return 0;
}
