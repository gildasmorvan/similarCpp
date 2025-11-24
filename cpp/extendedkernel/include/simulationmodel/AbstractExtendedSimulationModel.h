#ifndef ABSTRACTEXTENDEDSIMULATIONMODEL_H
#define ABSTRACTEXTENDEDSIMULATIONMODEL_H

#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

#include "../../microkernel/include/ISimulationEngine.h"
#include "../../microkernel/include/ISimulationModel.h"
#include "../../microkernel/include/LevelIdentifier.h"
#include "../../microkernel/include/SimulationTimeStamp.h"
#include "../../microkernel/include/levels/ILevel.h"
#include "../../microkernel/include/libs/abstractimpl/AbstractSimulationModel.h"
#include "IEndCriterionModel.h"
#include "ISimulationParameters.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace simulationmodel {

/**
 * Models a simulation model in the extended kernel.
 *
 * This model adds a feature compared to the common libs of the micro-kernel:
 * it separates the declaration of the simulation end criterion from the code
 * of the simulation model.
 *
 * Benefits:
 * - Simulation end criteria libraries can be built and re-used in many
 * simulations.
 */
class AbstractExtendedSimulationModel
    : public microkernel::libs::abstractimpl::AbstractSimulationModel {
private:
  /**
   * The parameters currently being used in the simulation.
   */
  std::shared_ptr<ISimulationParameters> simulationParameters;

  /**
   * The end criterion model used in this simulation model.
   */
  std::shared_ptr<IEndCriterionModel> endCriterionModel;

  /**
   * Checks the validity of the parameters provided to the constructor.
   * @param simulationParameters The execution parameters of the simulation.
   * @param endCriterionModel The end criterion model used in this simulation
   * model.
   * @return The initial time stamp of the simulation.
   * @throws std::invalid_argument If an argument is null.
   */
  static microkernel::SimulationTimeStamp checkParametersValidity(
      std::shared_ptr<ISimulationParameters> simulationParameters,
      std::shared_ptr<IEndCriterionModel> endCriterionModel) {
    if (!simulationParameters) {
      throw std::invalid_argument("The 'simulationParameters' cannot be null.");
    }
    if (!endCriterionModel) {
      throw std::invalid_argument("The 'endCriterion' cannot be null.");
    }
    return simulationParameters->getInitialTime();
  }

protected:
  /**
   * Generates the bare levels of the simulation.
   * These levels contain no agents and define no environment.
   * @param simulationParameters The execution parameters of the simulation.
   * @return The bare levels of the simulation.
   */
  virtual std::vector<std::shared_ptr<microkernel::levels::ILevel>>
  generateLevels(
      std::shared_ptr<ISimulationParameters> simulationParameters) = 0;

  /**
   * Generates the environment of the simulation.
   * At this stage, no agents are generated in the simulation.
   *
   * This method should set the public local state of the environment for each
   * level of the simulation.
   *
   * @param simulationParameters The execution parameters of the simulation.
   * @param levels The levels of the simulation.
   * @return The generated environment and the influences to put in the state
   *         dynamics of the initial dynamic state of the levels.
   */
  virtual microkernel::ISimulationModel::EnvironmentInitializationData
  generateEnvironment(
      std::shared_ptr<ISimulationParameters> simulationParameters,
      const std::map<microkernel::LevelIdentifier,
                     std::shared_ptr<microkernel::levels::ILevel>> &levels) = 0;

  /**
   * Generates the agents of the simulation.
   *
   * This method should only create the agents, without adding them in the
   * levels. This task is performed by the simulation engine.
   *
   * @param simulationParameters The execution parameters of the simulation.
   * @param levels The levels of the simulation.
   * @return The generated agents and the influences to put in the state
   *         dynamics of the initial dynamic state of the levels.
   */
  virtual microkernel::ISimulationModel::AgentInitializationData generateAgents(
      std::shared_ptr<ISimulationParameters> simulationParameters,
      const std::map<microkernel::LevelIdentifier,
                     std::shared_ptr<microkernel::levels::ILevel>> &levels) = 0;

public:
  /**
   * Builds an instance of an abstract extended simulation model.
   * @param simulationParameters The execution parameters of the simulation.
   * @param endCriterionModel The end criterion model used in this simulation
   * model.
   * @throws std::invalid_argument If an argument is null.
   */
  AbstractExtendedSimulationModel(
      std::shared_ptr<ISimulationParameters> simulationParameters,
      std::shared_ptr<IEndCriterionModel> endCriterionModel)
      : AbstractSimulationModel(
            checkParametersValidity(simulationParameters, endCriterionModel)),
        simulationParameters(simulationParameters),
        endCriterionModel(endCriterionModel) {}

  virtual ~AbstractExtendedSimulationModel() = default;

  /**
   * Gets the end criterion model used in this simulation model.
   * @return The end criterion model used in this simulation model.
   */
  std::shared_ptr<IEndCriterionModel> getEndCriterionModel() const {
    return endCriterionModel;
  }

  /**
   * Gets the parameters currently being used in the simulation.
   * @return The parameters currently being used in the simulation.
   */
  std::shared_ptr<ISimulationParameters> getSimulationParameters() const {
    return simulationParameters;
  }

  /**
   * {@inheritDoc}
   */
  bool isFinalTimeOrAfter(
      const microkernel::SimulationTimeStamp &currentTime,
      const microkernel::ISimulationEngine &engine) const override final {
    return endCriterionModel->isFinalTimeOrAfter(currentTime, engine);
  }

  /**
   * {@inheritDoc}
   */
  std::vector<std::shared_ptr<microkernel::levels::ILevel>> generateLevels(
      const microkernel::SimulationTimeStamp &initialTime) override final {
    return generateLevels(simulationParameters);
  }

  /**
   * {@inheritDoc}
   */
  microkernel::ISimulationModel::EnvironmentInitializationData
  generateEnvironment(
      const microkernel::SimulationTimeStamp &initialTime,
      const std::map<microkernel::LevelIdentifier,
                     std::shared_ptr<microkernel::levels::ILevel>> &levels)
      override final {
    return generateEnvironment(simulationParameters, levels);
  }

  /**
   * {@inheritDoc}
   */
  microkernel::ISimulationModel::AgentInitializationData
  generateAgents(const microkernel::SimulationTimeStamp &initialTime,
                 const std::map<microkernel::LevelIdentifier,
                                std::shared_ptr<microkernel::levels::ILevel>>
                     &levels) override final {
    return generateAgents(simulationParameters, levels);
  }
};

} // namespace simulationmodel
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ABSTRACTEXTENDEDSIMULATIONMODEL_H
