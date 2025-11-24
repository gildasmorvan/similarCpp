#ifndef SIMILAR2LOGO_LOGOENVIRONMENT_H
#define SIMILAR2LOGO_LOGOENVIRONMENT_H

#include "../../../../../microkernel/include/environment/IEnvironment4Engine.h"
#include "LogoEnvPLS.h"

namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel::model::
    environment {

class LogoEnvironment : public fr::univ_artois::lgi2a::similar::microkernel::
                            environment::IEnvironment4Engine {
private:
  std::shared_ptr<LogoEnvPLS> pls;

public:
  LogoEnvironment(std::shared_ptr<LogoEnvPLS> pls) : pls(pls) {}
  virtual ~LogoEnvironment() = default;

  std::shared_ptr<fr::univ_artois::lgi2a::similar::microkernel::environment::
                      ILocalStateOfEnvironment>
  getPublicLocalState(
      const fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier
          &level) const override {
    return pls;
  }

  std::shared_ptr<fr::univ_artois::lgi2a::similar::microkernel::environment::
                      ILocalStateOfEnvironment>
  getPrivateLocalState(
      const fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier
          &level) const override {
    return nullptr;
  }

  void natural(
      const fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier
          &level,
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &timeLowerBound,
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &timeUpperBound,
      const std::map<
          fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier,
          std::shared_ptr<fr::univ_artois::lgi2a::similar::microkernel::
                              environment::ILocalStateOfEnvironment>>
          &publicLocalStates,
      std::shared_ptr<fr::univ_artois::lgi2a::similar::microkernel::
                          environment::ILocalStateOfEnvironment>
          privateLocalState,
      std::shared_ptr<fr::univ_artois::lgi2a::similar::microkernel::
                          dynamicstate::IPublicDynamicStateMap>
          dynamicStates,
      std::shared_ptr<fr::univ_artois::lgi2a::similar::microkernel::influences::
                          InfluencesMap>
          producedInfluences) override {
    // No natural dynamics in environment for now (handled by reaction)
  }

  std::map<fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier,
           std::shared_ptr<fr::univ_artois::lgi2a::similar::microkernel::
                               environment::ILocalStateOfEnvironment>>
  getPublicLocalStates() const override {
    std::map<fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier,
             std::shared_ptr<fr::univ_artois::lgi2a::similar::microkernel::
                                 environment::ILocalStateOfEnvironment>>
        map;
    map.insert({pls->getLevel(), pls});
    return map;
  }
};

} // namespace
  // fr::univ_artois::lgi2a::similar::similar2logo::kernel::model::environment

#endif
