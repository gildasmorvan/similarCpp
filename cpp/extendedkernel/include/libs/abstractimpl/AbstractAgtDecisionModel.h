#ifndef ABSTRACTAGTDECISIONMODEL_H
#define ABSTRACTAGTDECISIONMODEL_H

#include "../../agents/IAgtDecisionModel.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {

/**
 * An abstract implementation of the IAgtDecisionModel interface,
 * providing a default behavior to the generic methods.
 */
class AbstractAgtDecisionModel : public agents::IAgtDecisionModel {
private:
  microkernel::LevelIdentifier levelIdentifier;

public:
  /**
   * Creates a bare instance of a decision model, using a specific level
   * identifier.
   * @param levelIdentifier The identifier of the level for which this decision
   * model is defined.
   */
  explicit AbstractAgtDecisionModel(
      const microkernel::LevelIdentifier &levelIdentifier);
  virtual ~AbstractAgtDecisionModel() = default;

  microkernel::LevelIdentifier getLevel() const override;
};

} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ABSTRACTAGTDECISIONMODEL_H
