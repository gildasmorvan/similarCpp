#ifndef ABSTRACTAGTPERCEPTIONMODEL_H
#define ABSTRACTAGTPERCEPTIONMODEL_H

#include "../../agents/IAgtPerceptionModel.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {

/**
 * An abstract implementation of the IAgtPerceptionModel interface,
 * providing a default behavior to the generic methods.
 */
class AbstractAgtPerceptionModel : public agents::IAgtPerceptionModel {
private:
  microkernel::LevelIdentifier levelIdentifier;

public:
  /**
   * Creates a bare instance of a perception model, using a specific level
   * identifier.
   * @param levelIdentifier The identifier of the level for which this
   * perception model is defined.
   */
  explicit AbstractAgtPerceptionModel(
      const microkernel::LevelIdentifier &levelIdentifier);
  virtual ~AbstractAgtPerceptionModel() = default;

  microkernel::LevelIdentifier getLevel() const override;
};

} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ABSTRACTAGTPERCEPTIONMODEL_H
