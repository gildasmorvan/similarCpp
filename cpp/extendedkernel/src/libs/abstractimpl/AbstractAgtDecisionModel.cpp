#include "../../../include/libs/abstractimpl/AbstractAgtDecisionModel.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {

AbstractAgtDecisionModel::AbstractAgtDecisionModel(
    const microkernel::LevelIdentifier &levelIdentifier)
    : levelIdentifier(levelIdentifier) {}

microkernel::LevelIdentifier AbstractAgtDecisionModel::getLevel() const {
  return this->levelIdentifier;
}

} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
