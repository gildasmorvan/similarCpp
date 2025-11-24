#ifndef SIMILAR2LOGO_LOGOAGENT_H
#define SIMILAR2LOGO_LOGOAGENT_H

#include "kernel/tools/Point2D.h"
#include <agents/ExtendedAgent.h>
#include <agents/IAgtDecisionModel.h>
#include <agents/IAgtPerceptionModel.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace agents {

namespace mk = fr::univ_artois::lgi2a::similar::microkernel;
namespace ek = fr::univ_artois::lgi2a::similar::extendedkernel;

/**
 * Logo Agent - C++ implementation of turtle agents.
 *
 * This provides the core Logo agent functionality in C++ for maximum
 * performance. Python can subclass this or provide decision callbacks.
 */
class LogoAgent : public ek::agents::ExtendedAgent {
private:
  double speed;
  std::string color;

public:
  LogoAgent(const mk::AgentCategory &category, double initialSpeed = 1.0,
            const std::string &color = "blue");

  virtual ~LogoAgent() = default;

  // Accessors
  double getSpeed() const { return speed; }
  void setSpeed(double newSpeed) { speed = newSpeed; }

  const std::string &getColor() const { return color; }
  void setColor(const std::string &newColor) { color = newColor; }
};

/**
 * Logo Perception Data - Contains what a turtle perceives.
 */
class LogoPerceivedData : public mk::agents::IPerceivedData {
private:
  mk::LevelIdentifier level;
  mk::SimulationTimeStamp timeLower, timeUpper;

  // Perception data
  tools::Point2D position;
  double heading;
  double speed;

  // Nearby turtles
  struct NearbyTurtle {
    tools::Point2D position;
    double heading;
    double distance;
    mk::AgentCategory category;
  };
  std::vector<NearbyTurtle> nearbyTurtles;

  // Pheromones at current location
  std::map<std::string, double> pheromones;

public:
  LogoPerceivedData(const mk::LevelIdentifier &level,
                    const mk::SimulationTimeStamp &lower,
                    const mk::SimulationTimeStamp &upper,
                    const tools::Point2D &pos, double heading, double speed)
      : level(level), timeLower(lower), timeUpper(upper), position(pos),
        heading(heading), speed(speed) {}

  mk::LevelIdentifier getLevel() const override { return level; }
  mk::SimulationTimeStamp getTransitoryPeriodMin() const override {
    return timeLower;
  }
  mk::SimulationTimeStamp getTransitoryPeriodMax() const override {
    return timeUpper;
  }

  const tools::Point2D &getPosition() const { return position; }
  double getHeading() const { return heading; }
  double getSpeed() const { return speed; }

  void addNearbyTurtle(const tools::Point2D &pos, double heading,
                       double distance, const mk::AgentCategory &category) {
    nearbyTurtles.push_back({pos, heading, distance, category});
  }

  const std::vector<NearbyTurtle> &getNearbyTurtles() const {
    return nearbyTurtles;
  }

  void setPheromone(const std::string &id, double value) {
    pheromones[id] = value;
  }

  double getPheromone(const std::string &id) const {
    auto it = pheromones.find(id);
    return it != pheromones.end() ? it->second : 0.0;
  }

  const std::map<std::string, double> &getAllPheromones() const {
    return pheromones;
  }
};

/**
 * Python-Callable Decision Model
 *
 * This allows Python code to provide the decision logic while
 * the C++ engine handles the multithreading.
 */
class PythonDecisionModel : public ek::agents::IAgtDecisionModel {
private:
  mk::LevelIdentifier level;

public:
  // Function type for Python callbacks
  using DecisionCallback =
      std::function<void(std::shared_ptr<LogoPerceivedData>,
                         std::shared_ptr<mk::influences::InfluencesMap>)>;

  DecisionCallback callback;

  PythonDecisionModel(const mk::LevelIdentifier &level, DecisionCallback cb)
      : level(level), callback(cb) {}

  mk::LevelIdentifier getLevel() const override { return level; }

  void decide(const mk::SimulationTimeStamp &, const mk::SimulationTimeStamp &,
              std::shared_ptr<mk::agents::IGlobalState>,
              std::shared_ptr<mk::agents::ILocalStateOfAgent>,
              std::shared_ptr<mk::agents::ILocalStateOfAgent>,
              std::shared_ptr<mk::agents::IPerceivedData> perceivedData,
              std::shared_ptr<mk::influences::InfluencesMap> producedInfluences)
      override {

    // Cast to LogoPerceivedData
    auto logoPerception =
        std::dynamic_pointer_cast<LogoPerceivedData>(perceivedData);
    if (logoPerception && callback) {
      callback(logoPerception, producedInfluences);
    }
  }
};

} // namespace agents
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILAR2LOGO_LOGOAGENT_H
