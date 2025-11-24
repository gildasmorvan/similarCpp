#ifndef ABSTRACTPERCEIVEDDATA_H
#define ABSTRACTPERCEIVEDDATA_H

#include "../agents/IPerceivedData.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace libs {

/**
 * Abstract base class for perceived data implementations.
 */
class AbstractPerceivedData : public agents::IPerceivedData {
private:
  LevelIdentifier levelIdentifier;
  SimulationTimeStamp transitoryPeriodMin;
  SimulationTimeStamp transitoryPeriodMax;

public:
  AbstractPerceivedData(const LevelIdentifier &levelIdentifier,
                        const SimulationTimeStamp &transitoryPeriodMin,
                        const SimulationTimeStamp &transitoryPeriodMax);

  virtual ~AbstractPerceivedData() = default;

  LevelIdentifier getLevel() const override;
  SimulationTimeStamp getTransitoryPeriodMin() const override;
  SimulationTimeStamp getTransitoryPeriodMax() const override;
};

} // namespace libs
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ABSTRACTPERCEIVEDDATA_H
