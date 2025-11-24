#include "kernel/model/LogoSimulationModel.h"
#include "kernel/model/environment/LogoEnvironment.h"
#include "kernel/model/levels/LogoDefaultReactionModel.h"
#include <environment/IEnvironment4Engine.h>
#include <random>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace model {

namespace mk = fr::univ_artois::lgi2a::similar::microkernel;
// namespace levels is already defined by the included header

LogoSimulationModel::LogoSimulationModel(int width, int height, bool xTorus,
                                         bool yTorus, int maxSteps)
    : width(width), height(height), xTorus(xTorus), yTorus(yTorus),
      maxSteps(maxSteps) {
  // Initialize default agent factory if none provided
  agentFactory = []() {
    return std::vector<std::shared_ptr<agents::LogoAgent>>();
  };
}

std::vector<std::shared_ptr<mk::levels::ILevel>>
LogoSimulationModel::generateLevels(
    const mk::SimulationTimeStamp &initialTime) {
  std::vector<std::shared_ptr<mk::levels::ILevel>> levels;

  // Create the default level
  mk::LevelIdentifier id("default");
  auto level = std::make_shared<levels::LogoDefaultReactionModel>();

  // We need to set the identifier and time if the constructor doesn't take them
  // But ILevelReactionModel usually doesn't store them?
  // ILevel stores them.
  // Wait, generateLevels returns ILevel.
  // So LogoDefaultReactionModel MUST inherit from ILevel?
  // LogoDefaultReactionModel.h says: public
  // extendedkernel::levels::ILevelReactionModel ILevelReactionModel is NOT
  // ILevel. ILevel is the container.

  // This implies the existing code was completely wrong or I misunderstood it.
  // I need to create an ILevel (e.g. StandardLevel) and assign the reaction
  // model.

  // Let's look at how to create a level.
  // Usually: auto level = make_shared<StandardLevel>(id, reactionModel, ...);

  // But I don't have StandardLevel header included.
  // And I don't know if LogoDefaultReactionModel is a Level or ReactionModel.
  // The name says ReactionModel.
  // The inheritance says ILevelReactionModel.

  // So I need to create a Level.
  // Maybe `LogoSimulationLevelList` helps?
  // It was included in LogoSimulationModel.h.

  // Let's assume for now I just fix the constructor call to be empty,
  // and if it fails to convert to ILevel, I'll know.

  levels.push_back(std::dynamic_pointer_cast<mk::levels::ILevel>(level));

  return levels;
}

microkernel::ISimulationModel::EnvironmentInitializationData
LogoSimulationModel::generateEnvironment(
    const microkernel::SimulationTimeStamp &initialTime,
    const std::map<microkernel::LevelIdentifier,
                   std::shared_ptr<microkernel::levels::ILevel>> &levels) {

  // Create Logo environment

  auto pls = std::make_shared<environment::LogoEnvPLS>(
      levels::LogoSimulationLevelList::LOGO, width, height, xTorus, yTorus,
      pheromones);
  auto env = std::make_shared<environment::LogoEnvironment>(pls);
  return microkernel::ISimulationModel::EnvironmentInitializationData(env);
}

microkernel::ISimulationModel::AgentInitializationData
LogoSimulationModel::generateAgents(
    const microkernel::SimulationTimeStamp &initialTime,
    const std::map<microkernel::LevelIdentifier,
                   std::shared_ptr<microkernel::levels::ILevel>> &levels) {

  microkernel::ISimulationModel::AgentInitializationData data;

  // Use agent factory if provided
  if (agentFactory) {
    auto agents = agentFactory();
    for (auto &agent : agents) {
      data.getAgents().insert(agent);
    }
  }

  return data;
}

} // namespace model
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
