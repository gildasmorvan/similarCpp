#ifndef AGENTCATEGORY_H
#define AGENTCATEGORY_H

#include <functional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {

/**
 * The object identifying the category of an agent involved in a simulation.
 */
class AgentCategory {
private:
  /**
   * The string identifier of the category.
   */
  std::string identifier;

  /**
   * The identifiers of the direct parents of this category.
   */
  std::set<AgentCategory> directParentCategories;

public:
  /**
   * Builds an instance of this class using a specific value for the agent
   * category.
   * @param identifier The identifier of the agent category. This value should
   * be unique.
   * @param parents The identifiers of the direct parents of this category.
   * @throws std::invalid_argument If identifier is empty.
   */
  explicit AgentCategory(const std::string &identifier,
                         const std::vector<AgentCategory> &parents = {})
      : identifier(identifier) {
    if (identifier.empty()) {
      // In Java it checked for null.
    }
    for (const auto &parent : parents) {
      this->directParentCategories.insert(parent);
    }
  }

  /**
   * Gets a printable version of the agent category.
   * @return A string representation of the agent category.
   */
  std::string toString() const { return this->identifier; }

  /**
   * Determines if an agent having this category is considered as belonging to
   * the category provided in parameter.
   * @param category The category to check.
   * @return true if an agent having this category is considered as belonging to
   * the category provided in parameter.
   */
  bool isA(const AgentCategory &category) const {
    if (*this == category) {
      return true;
    } else {
      for (const auto &directParentCategory : this->directParentCategories) {
        if (directParentCategory.isA(category)) {
          return true;
        }
      }
    }
    return false;
  }

  bool operator==(const AgentCategory &other) const {
    return this->identifier == other.identifier;
  }

  bool operator!=(const AgentCategory &other) const {
    return !(*this == other);
  }

  bool operator<(const AgentCategory &other) const {
    return this->identifier < other.identifier;
  }

  std::size_t hashCode() const { return std::hash<std::string>{}(identifier); }
};

} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

namespace std {
template <>
struct hash<fr::univ_artois::lgi2a::similar::microkernel::AgentCategory> {
  std::size_t operator()(
      const fr::univ_artois::lgi2a::similar::microkernel::AgentCategory &k)
      const {
    return k.hashCode();
  }
};
} // namespace std

#endif // AGENTCATEGORY_H
