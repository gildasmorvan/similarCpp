#ifndef SIMILARWEBCONFIG_H
#define SIMILARWEBCONFIG_H

#include <string>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace web {

/**
 * Configuration for the SIMILAR web interface.
 */
class SimilarWebConfig {
private:
  int port;
  std::string simulationName;
  bool initialized;
  bool autoOpenBrowser;

public:
  /**
   * Creates a new configuration with default values.
   */
  SimilarWebConfig();

  /**
   * Gets the port number for the HTTP server.
   */
  int getPort() const { return port; }

  /**
   * Sets the port number for the HTTP server.
   */
  void setPort(int port) { this->port = port; }

  /**
   * Gets the name of the simulation.
   */
  std::string getSimulationName() const { return simulationName; }

  /**
   * Sets the name of the simulation.
   */
  void setSimulationName(const std::string &name) {
    this->simulationName = name;
  }

  /**
   * Checks if the configuration has been finalized.
   */
  bool isAlreadyInitialized() const { return initialized; }

  /**
   * Finalizes the configuration (prevents further changes).
   */
  void finalizeConfiguration() { initialized = true; }

  /**
   * Gets whether to automatically open browser.
   */
  bool getAutoOpenBrowser() const { return autoOpenBrowser; }

  /**
   * Sets whether to automatically open browser.
   */
  void setAutoOpenBrowser(bool autoOpen) { this->autoOpenBrowser = autoOpen; }
};

} // namespace web
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILARWEBCONFIG_H
