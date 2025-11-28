#ifndef IGLOBALSTATE_H
#define IGLOBALSTATE_H

#include <memory>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace agents {

/**
 * Models the level-independent part of the state of an agent.
 */
class IGlobalState {
public:
  virtual ~IGlobalState() = default;

  /**
   * Clones the global state, creating a deep copy.
   * @return A deep copy of the global state.
   */
  virtual std::shared_ptr<IGlobalState> clone() const = 0;
};

} // namespace agents
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IGLOBALSTATE_H
