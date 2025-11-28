#ifndef EMPTYPERCEIVEDDATA_H
#define EMPTYPERCEIVEDDATA_H

#include "../AbstractPerceivedData.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace libs {
namespace generic {

/**
 * Models perceived data containing no information.
 *
 * This type of perceived data is used whenever the agent makes decision in a
 * level using only its private local state and its global memory state.
 */
class EmptyPerceivedData : public AbstractPerceivedData {
public:
  EmptyPerceivedData(const LevelIdentifier &levelIdentifier,
                     const SimulationTimeStamp &transitoryPeriodMin,
                     const SimulationTimeStamp &transitoryPeriodMax);

  virtual ~EmptyPerceivedData() = default;

  std::shared_ptr<microkernel::agents::IPerceivedData> clone() const override {
    return std::make_shared<EmptyPerceivedData>(*this);
  }
};

} // namespace generic
} // namespace libs
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // EMPTYPERCEIVEDDATA_H
