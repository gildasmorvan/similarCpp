#ifndef ABSTRACTENVNATURALMODEL_H
#define ABSTRACTENVNATURALMODEL_H

#include "../../../../microkernel/include/LevelIdentifier.h"
#include "../../environment/IEnvNaturalModel.h"
#include <stdexcept>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace abstractimpl {

/**
 * An abstract implementation of the IEnvNaturalModel interface,
 * providing a default behavior to the generic methods.
 */
class AbstractEnvNaturalModel : public environment::IEnvNaturalModel {
private:
  microkernel::LevelIdentifier levelIdentifier;

public:
  /**
   * Creates a bare instance of a natural action model.
   * @param levelIdentifier The identifier of the level
   * @throws std::invalid_argument If levelIdentifier is invalid
   */
  explicit AbstractEnvNaturalModel(
      const microkernel::LevelIdentifier &levelIdentifier)
      : levelIdentifier(levelIdentifier) {
    if (levelIdentifier.toString().empty()) {
      throw std::invalid_argument("The level identifier cannot be empty.");
    }
  }

  virtual ~AbstractEnvNaturalModel() = default;

  /**
   * Gets the level for which this natural action model is defined.
   * @return The level identifier
   */
  microkernel::LevelIdentifier getLevel() const override {
    return levelIdentifier;
  }
};

} // namespace abstractimpl
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ABSTRACTENVNATURALMODEL_H
