#include "kernel/agents/LogoAgent.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace agents {

LogoAgent::LogoAgent(const microkernel::AgentCategory &category,
                     double initialSpeed, const std::string &color)
    : ExtendedAgent(category), speed(initialSpeed), color(color) {}

} // namespace agents
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
