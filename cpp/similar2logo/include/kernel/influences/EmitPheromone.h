#ifndef SIMILAR2LOGO_EMITPHEROMONE_H
#define SIMILAR2LOGO_EMITPHEROMONE_H

#include "../../../../microkernel/include/LevelIdentifier.h"
#include "../../../../microkernel/include/SimulationTimeStamp.h"
#include "../../../../microkernel/include/influences/RegularInfluence.h"
#include "../tools/Point2D.h"
#include <string>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace influences {

/**
 * Models an influence that aims at emitting a pheromone at given location.
 * Matches the Java implementation exactly.
 *
 * @author Gildas Morvan
 * @author Yoann Kubera
 */
class EmitPheromone : public microkernel::influences::RegularInfluence {
public:
  /**
   * The category of the influence, used as a unique identifier in
   * the reaction of the target level to determine the nature of the influence.
   */
  static constexpr const char *CATEGORY = "emit pheromone";

private:
  /**
   * The identifier of the emitted pheromone.
   */
  std::string pheromoneIdentifier;

  /**
   * The location where the pheromone is emitted.
   */
  tools::Point2D location;

  /**
   * The amount of emitted pheromone.
   */
  double value;

public:
  /**
   * Builds an instance of this influence created during the transitory
   * period ] timeLowerBound, timeUpperBound [, in the LOGO level.
   *
   * @param timeLowerBound The lower bound of the transitory period
   * @param timeUpperBound The upper bound of the transitory period
   * @param location The location where the pheromone is emitted
   * @param pheromoneIdentifier The identifier of the emitted pheromone
   * @param value The amount of emitted pheromone
   */
  EmitPheromone(const microkernel::SimulationTimeStamp &timeLowerBound,
                const microkernel::SimulationTimeStamp &timeUpperBound,
                const tools::Point2D &location,
                const std::string &pheromoneIdentifier, double value);

  /**
   * Builds an instance of this influence created during the transitory
   * period ] timeLowerBound, timeUpperBound [, in a given level.
   *
   * @param levelIdentifier The level in which the influence is emitted
   * @param timeLowerBound The lower bound of the transitory period
   * @param timeUpperBound The upper bound of the transitory period
   * @param location The location where the pheromone is emitted
   * @param pheromoneIdentifier The identifier of the emitted pheromone
   * @param value The amount of emitted pheromone
   */
  EmitPheromone(const microkernel::LevelIdentifier &levelIdentifier,
                const microkernel::SimulationTimeStamp &timeLowerBound,
                const microkernel::SimulationTimeStamp &timeUpperBound,
                const tools::Point2D &location,
                const std::string &pheromoneIdentifier, double value);

  /**
   * @return The location where the pheromone is emitted.
   */
  const tools::Point2D &getLocation() const { return location; }

  /**
   * @return The identifier of the emitted pheromone.
   */
  const std::string &getPheromoneIdentifier() const {
    return pheromoneIdentifier;
  }

  /**
   * @return The amount of emitted pheromone.
   */
  double getValue() const { return value; }
};

} // namespace influences
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILAR2LOGO_EMITPHEROMONE_H
