#ifndef INFLUENCESMAP_H
#define INFLUENCESMAP_H

#include <algorithm>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "../LevelIdentifier.h"
#include "IInfluence.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace influences {

/**
 * A data structure containing a collection of influences, ordered using the
 * target level of the influences.
 */
class InfluencesMap {
private:
  std::map<LevelIdentifier, std::list<std::shared_ptr<IInfluence>>> influences;

public:
  InfluencesMap() = default;

  /**
   * Gets an unordered set containing the keys that are defined in this
   * influences map.
   */
  std::set<LevelIdentifier> getDefinedKeys() const {
    std::set<LevelIdentifier> keys;
    for (const auto &pair : influences) {
      keys.insert(pair.first);
    }
    return keys;
  }

  /**
   * Tells whether if this map contains at least one influence or not.
   */
  bool isEmpty() const {
    for (const auto &pair : influences) {
      if (!pair.second.empty()) {
        return false;
      }
    }
    return true;
  }

  /**
   * Check if this map contains no influences targeted at a specific level.
   */
  bool isEmpty(const LevelIdentifier &targetLevel) const {
    auto it = influences.find(targetLevel);
    if (it == influences.end()) {
      return true;
    }
    return it->second.empty();
  }

  /**
   * Gets the influences contained in this map that are targeted at a specific
   * level.
   */
  std::list<std::shared_ptr<IInfluence>>
  getInfluencesForLevel(const LevelIdentifier &targetLevel) {
    return influences[targetLevel];
  }

  /**
   * Adds an influence to this influence map.
   */
  void add(std::shared_ptr<IInfluence> influence) {
    if (influence == nullptr) {
      throw std::invalid_argument("The 'influence' argument cannot be null.");
    }
    influences[influence->getTargetLevel()].push_back(influence);
  }

  /**
   * Adds all the influences contained in an influence map into this influence
   * map.
   */
  void addAll(const InfluencesMap &toAdd) {
    for (const auto &pair : toAdd.influences) {
      std::list<std::shared_ptr<IInfluence>> &my_list = influences[pair.first];
      my_list.insert(my_list.end(), pair.second.begin(), pair.second.end());
    }
  }

  void clear() { influences.clear(); }
};

} // namespace influences
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // INFLUENCESMAP_H
