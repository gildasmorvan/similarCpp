#ifndef EMPTYAGTPERCEPTIONMODEL_H
#define EMPTYAGTPERCEPTIONMODEL_H

#include "../abstractimpl/AbstractAgtPerceptionModel.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace generic {

/**
 * Models a perception model perceiving no data.
 */
class EmptyAgtPerceptionModel : public AbstractAgtPerceptionModel {
public:
  explicit EmptyAgtPerceptionModel(
      const microkernel::LevelIdentifier &levelIdentifier);
  virtual ~EmptyAgtPerceptionModel() = default;

  std::shared_ptr<microkernel::agents::IPerceivedData> perceive(
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      const std::map<microkernel::LevelIdentifier,
                     std::shared_ptr<microkernel::agents::ILocalStateOfAgent>>
          &publicLocalStates,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent>
          privateLocalState,
      std::shared_ptr<microkernel::dynamicstate::IPublicDynamicStateMap>
          dynamicStates) override;
};

} // namespace generic
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // EMPTYAGTPERCEPTIONMODEL_H
