#ifndef IPERCEIVEDDATA_H
#define IPERCEIVEDDATA_H

#include "../LevelIdentifier.h"
#include "../SimulationTimeStamp.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace agents {

/**
 * Models the data that were perceived by an agent from a specific level.
 */
class IPerceivedData {
public:
  virtual ~IPerceivedData() = default;

  /**
   * Gets the identifier of the level from which these data were perceived.
   * @return The identifier of the level for which these data were perceived.
   */
  virtual LevelIdentifier getLevel() const = 0;

  /**
   * Gets the lower bound of the transitory period for which these data were
   * perceived.
   * @return The lower bound of the transitory period for which these data were
   * perceived.
   */
  virtual SimulationTimeStamp getTransitoryPeriodMin() const = 0;

  /**
   * Gets the upper bound of the transitory period for which these data were
   * perceived.
   * @return The upper bound of the transitory period for which these data were
   * perceived.
   */
  virtual SimulationTimeStamp getTransitoryPeriodMax() const = 0;
};

} // namespace agents
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IPERCEIVEDDATA_H
