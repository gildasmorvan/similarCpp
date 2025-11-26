#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <unordered_set>

#include "kernel/influences/AgentPositionUpdate.h"
#include "kernel/influences/ChangeAcceleration.h"
#include "kernel/influences/ChangeDirection.h"
#include "kernel/influences/ChangePosition.h"
#include "kernel/influences/ChangeSpeed.h"
#include "kernel/influences/DropMark.h"
#include "kernel/influences/EmitPheromone.h"
#include "kernel/influences/PheromoneFieldUpdate.h"
#include "kernel/influences/RemoveMark.h"
#include "kernel/influences/RemoveMarks.h"
#include "kernel/influences/Stop.h"

#include "kernel/model/environment/Mark.h"
#include "kernel/model/environment/TurtlePLSInLogo.h"
#include "kernel/tools/Point2D.h"

namespace mk = fr::univ_artois::lgi2a::similar::microkernel;
namespace s2l = fr::univ_artois::lgi2a::similar::similar2logo::kernel;

void testChangeAcceleration() {
  std::cout << "Testing ChangeAcceleration..." << std::endl;
  mk::SimulationTimeStamp t1(0);
  mk::SimulationTimeStamp t2(10);
  s2l::tools::Point2D loc(10, 20);
  auto turtle = std::make_shared<s2l::model::environment::TurtlePLSInLogo>(
      loc, 0.0, 1.0, 0.0, true, std::string("red"));

  s2l::influences::ChangeAcceleration inf(t1, t2, 5.0, turtle);

  assert(inf.getCategory() == "change acceleration");
  assert(inf.getDa() == 5.0);
  assert(inf.getTarget() == turtle);
  std::cout << "PASS" << std::endl;
}

void testChangeDirection() {
  std::cout << "Testing ChangeDirection..." << std::endl;
  mk::SimulationTimeStamp t1(0);
  mk::SimulationTimeStamp t2(10);
  s2l::tools::Point2D loc(10, 20);
  auto turtle = std::make_shared<s2l::model::environment::TurtlePLSInLogo>(
      loc, 0.0, 1.0, 0.0, true, std::string("red"));

  s2l::influences::ChangeDirection inf(t1, t2, 1.57, turtle);

  assert(inf.getCategory() == "change direction");
  assert(std::abs(inf.getDd() - 1.57) < 1e-9);
  assert(inf.getTarget() == turtle);
  std::cout << "PASS" << std::endl;
}

void testChangePosition() {
  std::cout << "Testing ChangePosition..." << std::endl;
  mk::SimulationTimeStamp t1(0);
  mk::SimulationTimeStamp t2(10);
  s2l::tools::Point2D loc(10, 20);
  auto turtle = std::make_shared<s2l::model::environment::TurtlePLSInLogo>(
      loc, 0.0, 1.0, 0.0, true, std::string("red"));

  s2l::influences::ChangePosition inf(t1, t2, 5.0, -3.0, turtle);

  assert(inf.getCategory() == "change position");
  assert(inf.getDx() == 5.0);
  assert(inf.getDy() == -3.0);
  assert(inf.getTarget() == turtle);
  std::cout << "PASS" << std::endl;
}

void testChangeSpeed() {
  std::cout << "Testing ChangeSpeed..." << std::endl;
  mk::SimulationTimeStamp t1(0);
  mk::SimulationTimeStamp t2(10);
  s2l::tools::Point2D loc(10, 20);
  auto turtle = std::make_shared<s2l::model::environment::TurtlePLSInLogo>(
      loc, 0.0, 1.0, 0.0, true, std::string("red"));

  s2l::influences::ChangeSpeed inf(t1, t2, 2.5, turtle);

  assert(inf.getCategory() == "change speed");
  assert(inf.getDs() == 2.5);
  assert(inf.getTarget() == turtle);
  std::cout << "PASS" << std::endl;
}

void testDropMark() {
  std::cout << "Testing DropMark..." << std::endl;
  mk::SimulationTimeStamp t1(0);
  mk::SimulationTimeStamp t2(10);
  s2l::tools::Point2D loc(10, 20);
  auto mark = std::make_shared<s2l::model::environment::SimpleMark>(loc);

  s2l::influences::DropMark inf(t1, t2, mark);

  assert(inf.getCategory() == "drop mark");
  assert(inf.getMark() == mark);
  std::cout << "PASS" << std::endl;
}

void testEmitPheromone() {
  std::cout << "Testing EmitPheromone..." << std::endl;
  mk::SimulationTimeStamp t1(0);
  mk::SimulationTimeStamp t2(10);
  s2l::tools::Point2D loc(10, 20);

  s2l::influences::EmitPheromone inf(t1, t2, loc, "phero1", 100.0);

  assert(inf.getCategory() == "emit pheromone");
  assert(inf.getLocation() == loc);
  assert(inf.getPheromoneIdentifier() == "phero1");
  assert(inf.getValue() == 100.0);
  std::cout << "PASS" << std::endl;
}

void testRemoveMark() {
  std::cout << "Testing RemoveMark..." << std::endl;
  mk::SimulationTimeStamp t1(0);
  mk::SimulationTimeStamp t2(10);
  s2l::tools::Point2D loc(10, 20);
  auto mark = std::make_shared<s2l::model::environment::SimpleMark>(loc);

  s2l::influences::RemoveMark inf(t1, t2, mark);

  assert(inf.getCategory() == "remove mark");
  assert(inf.getMark() == mark);
  std::cout << "PASS" << std::endl;
}

void testRemoveMarks() {
  std::cout << "Testing RemoveMarks..." << std::endl;
  mk::SimulationTimeStamp t1(0);
  mk::SimulationTimeStamp t2(10);
  s2l::tools::Point2D loc(10, 20);
  auto mark1 = std::make_shared<s2l::model::environment::SimpleMark>(loc);
  auto mark2 = std::make_shared<s2l::model::environment::SimpleMark>(loc);

  std::unordered_set<std::shared_ptr<s2l::model::environment::SimpleMark>>
      marks;
  marks.insert(mark1);
  marks.insert(mark2);

  s2l::influences::RemoveMarks inf(t1, t2, marks);

  assert(inf.getCategory() == "remove marks");
  assert(inf.getMarks().size() == 2);
  assert(inf.getMarks().count(mark1) == 1);
  assert(inf.getMarks().count(mark2) == 1);
  std::cout << "PASS" << std::endl;
}

void testStop() {
  std::cout << "Testing Stop..." << std::endl;
  mk::SimulationTimeStamp t1(0);
  mk::SimulationTimeStamp t2(10);
  s2l::tools::Point2D loc(10, 20);
  auto turtle = std::make_shared<s2l::model::environment::TurtlePLSInLogo>(
      loc, 0.0, 1.0, 0.0, true, std::string("red"));

  s2l::influences::Stop inf(t1, t2, turtle);

  assert(inf.getCategory() == "stop");
  assert(inf.getTarget() == turtle);
  std::cout << "PASS" << std::endl;
}

void testPheromoneFieldUpdate() {
  std::cout << "Testing PheromoneFieldUpdate..." << std::endl;
  mk::SimulationTimeStamp t1(0);
  mk::SimulationTimeStamp t2(10);

  s2l::influences::PheromoneFieldUpdate inf(t1, t2);

  assert(inf.getCategory() == "pheromone field update");
  std::cout << "PASS" << std::endl;
}

void testAgentPositionUpdate() {
  std::cout << "Testing AgentPositionUpdate..." << std::endl;
  mk::SimulationTimeStamp t1(0);
  mk::SimulationTimeStamp t2(10);

  s2l::influences::AgentPositionUpdate inf(t1, t2);

  assert(inf.getCategory() == "agent position update");
  std::cout << "PASS" << std::endl;
}

int main() {
  std::cout << "Running similar2logo influence tests..." << std::endl;

  testChangeAcceleration();
  testChangeDirection();
  testChangePosition();
  testChangeSpeed();
  testDropMark();
  testEmitPheromone();
  testRemoveMark();
  testRemoveMarks();
  testStop();
  testPheromoneFieldUpdate();
  testAgentPositionUpdate();

  std::cout << "All tests passed!" << std::endl;
  return 0;
}
