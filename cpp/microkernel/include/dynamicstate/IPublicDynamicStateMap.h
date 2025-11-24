#ifndef IPUBLICDYNAMICSTATEMAP_H
#define IPUBLICDYNAMICSTATEMAP_H

#include "../LevelIdentifier.h"
#include "IPublicLocalDynamicState.h"
#include <memory>
#include <set>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace dynamicstate {

/**
 * Models a map containing public local dynamic states.
 */
class IPublicDynamicStateMap {
public:
  virtual ~IPublicDynamicStateMap() = default;

  /**
   * Gets the levels which dynamic state is contained in this map.
   * @return The identifier of the levels contained in this map.
   */
  virtual std::set<LevelIdentifier> keySet() const = 0;

  /**
   * Gets the dynamic state of a level contained in this map.
   * @param level The level of the dynamic state.
   * @return The dynamic state of the level.
   * @throws std::out_of_range If the level identifier is not a key in this map.
   */
  virtual std::shared_ptr<IPublicLocalDynamicState>
  get(const LevelIdentifier &level) const = 0;

  /**
   * Puts a dynamic state into this map.
   * @param state The public local dynamic state of to add to this map.
   */
  virtual void put(std::shared_ptr<IPublicLocalDynamicState> state) = 0;
};

} // namespace dynamicstate
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IPUBLICDYNAMICSTATEMAP_H
