#include "../../../include/libs/abstractimpl/AbstractAgtPerceptionModel.h"
#include <stdexcept>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {

AbstractAgtPerceptionModel::AbstractAgtPerceptionModel(
    const microkernel::LevelIdentifier &levelIdentifier)
    : levelIdentifier(levelIdentifier) {
  // Level identifier validation could be added here if needed
}

microkernel::LevelIdentifier AbstractAgtPerceptionModel::getLevel() const {
  return this->levelIdentifier;
}

} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
