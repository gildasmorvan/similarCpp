#ifndef IHTMLREQUESTS_H
#define IHTMLREQUESTS_H

#include <string>
#include <vector>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace web {

/**
 * Interface for handling HTML requests from the web view.
 */
class IHtmlRequests {
public:
  virtual ~IHtmlRequests() = default;

  /**
   * Called by the view to get the current state of the simulation engine.
   * @return A byte representation of the engine state
   */
  virtual std::vector<uint8_t> handleSimulationStateRequest() = 0;

  /**
   * Called by the view when the user wants to start a new simulation.
   */
  virtual void handleNewSimulationRequest() = 0;

  /**
   * Called by the view when the user wants to abort the currently running
   * simulation.
   */
  virtual void handleSimulationAbortionRequest() = 0;

  /**
   * Called by the view when the user wants to pause or resume the simulation.
   */
  virtual void handleSimulationPauseRequest() = 0;

  /**
   * Called by the view when the user wants to shut down the server.
   */
  virtual void handleShutDownRequest() = 0;

  /**
   * Gets the value of a specific simulation parameter.
   * @param parameter The name of the parameter
   * @return The value of the parameter, or an error message if not found
   */
  virtual std::string getParameter(const std::string &parameter) = 0;

  /**
   * Sets the value of a specific simulation parameter.
   * @param parameter The name of the parameter
   * @param value The new value for the parameter
   */
  virtual void setParameter(const std::string &parameter,
                            const std::string &value) = 0;
};

} // namespace web
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IHTMLREQUESTS_H
