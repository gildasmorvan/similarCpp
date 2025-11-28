#ifndef ABSTRACTAGENT_H
#define ABSTRACTAGENT_H

#include "../agents/IAgent4Engine.h"
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace libs {

/**
 * An abstract implementation of the IAgent and IAgent4Engine interfaces,
 * providing a default behavior to the generic methods.
 */
class AbstractAgent : public agents::IAgent4Engine {
private:
  AgentCategory category;
  std::shared_ptr<agents::IGlobalState> globalState;
  std::map<LevelIdentifier, std::shared_ptr<agents::ILocalStateOfAgent>>
      publicLocalStates;
  std::map<LevelIdentifier, std::shared_ptr<agents::ILocalStateOfAgent>>
      privateLocalStates;
  std::map<LevelIdentifier, std::shared_ptr<agents::IPerceivedData>>
      lastPerceivedData;

  static std::string
  buildNullArgumentExceptionText(const std::string &argName) {
    return "The '" + argName + "' argument cannot be null.";
  }

public:
  explicit AbstractAgent(const AgentCategory &category);
  virtual ~AbstractAgent() = default;

protected:
  AbstractAgent(const AbstractAgent &other);

public:
  AgentCategory getCategory() const override;
  std::shared_ptr<agents::IGlobalState> getGlobalState() const override;
  void initializeGlobalState(
      std::shared_ptr<agents::IGlobalState> initialGlobalState);

  std::set<LevelIdentifier> getLevels() const override;
  std::shared_ptr<agents::ILocalStateOfAgent>
  getPublicLocalState(const LevelIdentifier &levelId) const override;
  std::map<LevelIdentifier, std::shared_ptr<agents::ILocalStateOfAgent>>
  getPublicLocalStates() const override;
  std::shared_ptr<agents::ILocalStateOfAgent>
  getPrivateLocalState(const LevelIdentifier &levelId) const override;

  void includeNewLevel(
      const LevelIdentifier &levelIdentifier,
      std::shared_ptr<agents::ILocalStateOfAgent> publicLocalState,
      std::shared_ptr<agents::ILocalStateOfAgent> privateLocalState) override;

  void excludeFromLevel(const LevelIdentifier &levelIdentifier) override;

  std::map<LevelIdentifier, std::shared_ptr<agents::IPerceivedData>>
  getPerceivedData() const override;
  void setPerceivedData(
      std::shared_ptr<agents::IPerceivedData> perceivedData) override;
};

} // namespace libs
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ABSTRACTAGENT_H
