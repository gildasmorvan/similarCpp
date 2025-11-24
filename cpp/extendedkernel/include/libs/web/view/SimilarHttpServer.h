#ifndef SIMILARHTTPSERVER_H
#define SIMILARHTTPSERVER_H

#include "../IHtmlControls.h"
#include "../IHtmlInitializationData.h"
#include "../IHtmlRequests.h"
#include <atomic>
#include <memory>
#include <string>

// Forward declare httplib
namespace httplib {
class Server;
}

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace web {
namespace view {

/**
 * HTTP server managing the HTML view on the simulation.
 * Uses cpp-httplib for HTTP serving.
 */
class SimilarHttpServer : public IHtmlControls {
private:
  IHtmlRequests *controller;
  IHtmlInitializationData *initData;
  std::unique_ptr<httplib::Server> server;
  std::atomic<bool> running;
  int port;

  void setupRoutes();
  std::string loadStaticFile(const std::string &path);
  std::string getMimeType(const std::string &path);
  std::string generateHtmlPage();

public:
  /**
   * Creates a new HTTP server.
   * @param controller The controller handling requests
   * @param initData Initialization data
   */
  SimilarHttpServer(IHtmlRequests *controller,
                    IHtmlInitializationData *initData);

  ~SimilarHttpServer();

  /**
   * Initializes the web server and sets up routes.
   */
  void initServer();

  /**
   * Starts the server and opens the browser.
   */
  void showView();

  /**
   * Stops the server.
   */
  void stop();

  // IHtmlControls implementation
  void setStartButtonState(bool active) override;
  void setPauseButtonState(bool active) override;
  void setAbortButtonState(bool active) override;
  void shutDownView() override;
};

} // namespace view
} // namespace web
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILARHTTPSERVER_H
