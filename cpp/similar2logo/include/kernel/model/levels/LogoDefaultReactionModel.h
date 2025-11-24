#ifndef SIMILAR2LOGO_LOGODEFAULTREACTIONMODEL_H
#define SIMILAR2LOGO_LOGODEFAULTREACTIONMODEL_H

#include "../../../../../extendedkernel/include/levels/ILevelReactionModel.h"
#include "../../../../../microkernel/include/SimulationTimeStamp.h"
#include "../../../../../microkernel/include/dynamicstate/ConsistentPublicLocalDynamicState.h"
#include "../../../../../microkernel/include/influences/IInfluence.h"
#include "../../../../../microkernel/include/influences/InfluencesMap.h"
#include "../environment/LogoEnvPLS.h"
#include "../environment/Pheromone.h"
#include <memory>
#include <set>
#include <vector>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace model {
namespace levels {

/**
 * The default reaction function of a Logo level.
 *
 * This reaction model handles:
 * - Logo-specific influences (ChangePosition, DropMark, EmitPheromone, etc.)
 * - Pheromone dynamics (evaporation and diffusion)
 * - Agent position updates in the grid
 * - System influences (add/remove agents)
 */
class LogoDefaultReactionModel
    : public extendedkernel::levels::ILevelReactionModel {
public:
  LogoDefaultReactionModel() = default;
  virtual ~LogoDefaultReactionModel() = default;

  /**
   * Makes the regular reaction to influences.
   * Processes Logo-specific influences and natural dynamics.
   */
  void makeRegularReaction(
      const microkernel::SimulationTimeStamp &transitoryTimeMin,
      const microkernel::SimulationTimeStamp &transitoryTimeMax,
      std::shared_ptr<
          microkernel::dynamicstate::ConsistentPublicLocalDynamicState>
          consistentState,
      const std::set<std::shared_ptr<microkernel::influences::IInfluence>>
          &regularInfluencesOftransitoryStateDynamics,
      std::shared_ptr<microkernel::influences::InfluencesMap>
          remainingInfluences) override {

    // Get the Logo environment
    auto envState = consistentState->getPublicLocalStateOfEnvironment();
    auto logoEnv = std::dynamic_pointer_cast<environment::LogoEnvPLS>(envState);

    if (!logoEnv) {
      return; // Not a Logo environment
    }

    // Process influences
    // Note: Full implementation would handle:
    // - DropMark, RemoveMark, RemoveMarks
    // - EmitPheromone
    // - ChangePosition, ChangeDirection, ChangeSpeed, ChangeAcceleration
    // - Stop
    // - AgentPositionUpdate
    // - PheromoneFieldUpdate

    // For now, this is a placeholder
    // Influences will be implemented in Phase 2
  }

  /**
   * Makes the system reaction to influences.
   * Handles adding/removing agents from the grid.
   */
  void makeSystemReaction(
      const microkernel::SimulationTimeStamp &transitoryTimeMin,
      const microkernel::SimulationTimeStamp &transitoryTimeMax,
      std::shared_ptr<
          microkernel::dynamicstate::ConsistentPublicLocalDynamicState>
          consistentState,
      const std::vector<std::shared_ptr<microkernel::influences::IInfluence>>
          &systemInfluencesToManage,
      bool happensBeforeRegularReaction,
      std::shared_ptr<microkernel::influences::InfluencesMap>
          newInfluencesToProcess) override {

    // Get the Logo environment
    auto envState = consistentState->getPublicLocalStateOfEnvironment();
    auto logoEnv = std::dynamic_pointer_cast<environment::LogoEnvPLS>(envState);

    if (!logoEnv) {
      return;
    }

    // Process system influences
    // Note: Full implementation would handle:
    // - SystemInfluenceAddAgentToLevel: Add turtle to grid
    // - SystemInfluenceRemoveAgentFromLevel: Remove turtle from grid

    // For now, this is a placeholder
  }

protected:
  /**
   * Computes the diffusion of pheromones.
   * @param environment The Logo environment
   * @param dt The time step size
   */
  void pheromoneDiffusion(std::shared_ptr<environment::LogoEnvPLS> environment,
                          long dt) {
    auto &pheromoneField = environment->getPheromoneField();

    for (auto &[pheromone, field] : pheromoneField) {
      // Create temporary field for diffusion calculation
      auto tmpField = field; // Copy

      int width = environment->getWidth();
      int height = environment->getHeight();

      for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
          // Get neighbors
          auto neighbors = environment->getNeighbors(x, y, 1);

          // Diffuse to neighbors
          for (const auto &neighbor : neighbors) {
            int nx = static_cast<int>(neighbor.x);
            int ny = static_cast<int>(neighbor.y);

            if (nx != x || ny != y) {
              // Diffuse to neighbor (8 neighbors)
              tmpField[nx][ny] +=
                  pheromone.getDiffusionCoef() * field[x][y] * dt / 8.0;
            }
          }

          // Reduce current cell
          tmpField[x][y] -= field[x][y] * pheromone.getDiffusionCoef() * dt;
        }
      }

      // Update field
      field = tmpField;
    }
  }

  /**
   * Computes the evaporation of pheromones.
   * @param environment The Logo environment
   * @param dt The time step size
   */
  void
  pheromoneEvaporation(std::shared_ptr<environment::LogoEnvPLS> environment,
                       long dt) {
    auto &pheromoneField = environment->getPheromoneField();

    for (auto &[pheromone, field] : pheromoneField) {
      int width = environment->getWidth();
      int height = environment->getHeight();

      for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
          // Evaporate
          field[x][y] -= pheromone.getEvaporationCoef() * field[x][y] * dt;

          // Apply minimum value
          if (field[x][y] < pheromone.getMinValue()) {
            field[x][y] = 0.0;
          }
        }
      }
    }
  }

  /**
   * Reacts to pheromone field update influence.
   * Performs evaporation and diffusion.
   */
  void reactToPheromoneFieldUpdate(
      const microkernel::SimulationTimeStamp &transitoryTimeMin,
      const microkernel::SimulationTimeStamp &transitoryTimeMax,
      std::shared_ptr<environment::LogoEnvPLS> environment) {

    long dt = transitoryTimeMax.compareToTimeStamp(transitoryTimeMin);

    // Perform diffusion
    pheromoneDiffusion(environment, dt);

    // Perform evaporation
    pheromoneEvaporation(environment, dt);
  }
};

} // namespace levels
} // namespace model
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILAR2LOGO_LOGODEFAULTREACTIONMODEL_H
