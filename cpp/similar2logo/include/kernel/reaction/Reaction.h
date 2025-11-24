#pragma once
#include "../../microkernel/include/influences/IInfluence.h"
#include "../environment/Environment.h"
#include <memory>
#include <vector>

namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel::reaction {

using IInfluence =
    fr::univ_artois::lgi2a::similar::microkernel::influences::IInfluence;
using Environment = fr::univ_artois::lgi2a::similar::similar2logo::kernel::
    environment::Environment;

class Reaction {
public:
  Reaction() = default;
  void apply(const std::vector<std::shared_ptr<IInfluence>> &influences,
             Environment &env, double dt = 1.0);
};

} // namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel::reaction
