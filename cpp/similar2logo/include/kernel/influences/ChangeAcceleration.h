#ifndef SIMILAR2LOGO_CHANGEACCELERATION_H
#define SIMILAR2LOGO_CHANGEACCELERATION_H

#include "../../../../microkernel/include/LevelIdentifier.h"
#include "../../../../microkernel/include/SimulationTimeStamp.h"
#include "../../../../microkernel/include/influences/RegularInfluence.h"
#include "../model/environment/TurtlePLSInLogo.h"
#include <memory>
#include <string>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace influences {

/**
 * Models an influence that aims at changing the acceleration of a turtle.
 * Matches the Java implementation exactly.
 *
 * @author Gildas Morvan
 * @author Yoann Kubera
 */
class ChangeAcceleration : public microkernel::influences::RegularInfluence {
public:
  /**
   * The category of the influence, used as a unique identifier in
   * the reaction of the target level to determine the nature of the influence.
   */
  static constexpr const char *CATEGORY = "change acceleration";

private:
  /**
   * The acceleration change.
   */
  double da;

  /**
   * The turtle's public local state that is going to change.
   */
  std::shared_ptr<model::environment::TurtlePLSInLogo> target;

public:
  /**
   * Builds an instance of this influence created during the transitory
   * period ] timeLowerBound, timeUpperBound [, in the LOGO level.
   *
   * @param timeLowerBound The lower bound of the transitory period
   * @param timeUpperBound The upper bound of the transitory period
   * @param da The acceleration change
   * @param target The turtle's public local state that is going to change
   */
  ChangeAcceleration(const microkernel::SimulationTimeStamp &timeLowerBound,
                     const microkernel::SimulationTimeStamp &timeUpperBound,
                     double da,
                     std::shared_ptr<model::environment::TurtlePLSInLogo> target);

  /**
   * Builds an instance of this influence created during the transitory
   * period ] timeLowerBound, timeUpperBound [, in a given level.
   *
   * @param levelIdentifier The level in which the influence is emitted
   * @param timeLowerBound The lower bound of the transitory period
   * @param timeUpperBound The upper bound of the transitory period
   * @param da The acceleration change
   * @param target The turtle's public local state that is going to change
   */
  ChangeAcceleration(const microkernel::LevelIdentifier &levelIdentifier,
                     const microkernel::SimulationTimeStamp &timeLowerBound,
                     const microkernel::SimulationTimeStamp &timeUpperBound,
                     double da,
                     std::shared_ptr<model::environment::TurtlePLSInLogo> target);

  /**
   * @return The acceleration change.
   */
  double getDa() const { return da; }

  /**
   * @return The turtle's public local state that is going to change.
   */
  std::shared_ptr<model::environment::TurtlePLSInLogo> getTarget() const {
    return target;
  }
};

} // namespace influences
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILAR2LOGO_CHANGEACCELERATION_H
