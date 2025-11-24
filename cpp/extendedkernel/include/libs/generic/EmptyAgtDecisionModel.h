#ifndef EMPTYAGTDECISIONMODEL_H
#define EMPTYAGTDECISIONMODEL_H

#include "../abstractimpl/AbstractAgtDecisionModel.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace generic {

/**
 * Models a decision model producing no influences.
 */
class EmptyAgtDecisionModel : public AbstractAgtDecisionModel {
public:
  explicit EmptyAgtDecisionModel(
      const microkernel::LevelIdentifier &levelIdentifier);
  virtual ~EmptyAgtDecisionModel() = default;

  void decide(
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      std::shared_ptr<microkernel::agents::IGlobalState> globalState,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent> publicLocalState,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent>
          privateLocalState,
      std::shared_ptr<microkernel::agents::IPerceivedData> perceivedData,
      std::shared_ptr<microkernel::influences::InfluencesMap>
          producedInfluences) override;
};

} // namespace generic
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // EMPTYAGTDECISIONMODEL_H
