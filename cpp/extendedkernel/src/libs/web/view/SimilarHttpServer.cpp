#include "libs/web/view/SimilarHttpServer.h"
#include "third_party/httplib.h"
#include "libs/web/SimilarWebConfig.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#ifdef __APPLE__
#include <cstdlib>
#endif

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace web {
namespace view {

SimilarHttpServer::SimilarHttpServer(IHtmlRequests *controller,
                                     IHtmlInitializationData *initData)
    : controller(controller), initData(initData), running(false) {

  port = initData->getConfig()->getPort();
  server = std::make_unique<httplib::Server>();
}

SimilarHttpServer::~SimilarHttpServer() { stop(); }

std::string SimilarHttpServer::getMimeType(const std::string &path) {
  if (path.ends_with(".html"))
    return "text/html";
  if (path.ends_with(".css"))
    return "text/css";
  if (path.ends_with(".js"))
    return "application/javascript";
  if (path.ends_with(".json"))
    return "application/json";
  if (path.ends_with(".png"))
    return "image/png";
  if (path.ends_with(".jpg") || path.ends_with(".jpeg"))
    return "image/jpeg";
  if (path.ends_with(".gif"))
    return "image/gif";
  if (path.ends_with(".svg"))
    return "image/svg+xml";
  return "application/octet-stream";
}

std::string SimilarHttpServer::loadStaticFile(const std::string &path) {
  std::string fullPath = "extendedkernel/resources/web/" + path;
  std::ifstream file(fullPath, std::ios::binary);
  if (!file) {
    return "";
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

std::string SimilarHttpServer::generateHtmlPage() {
  std::string simName = initData->getConfig()->getSimulationName();

  return R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>)HTML" + simName + R"HTML(</title>
    <link rel="stylesheet" href="/css/bootstrap.min.css">
    <link rel="stylesheet" href="/css/similar-gui.css">
</head>
<body>
    <div class="container mt-4">
        <h1 id="simulation-title">)HTML" + simName + R"HTML(</h1>
        
        <div class="card mt-4">
            <div class="card-header">
                <h3>Simulation Controls</h3>
            </div>
            <div class="card-body">
                <div class="btn-group" role="group">
                    <button id="startBtn" class="btn btn-success" onclick="startSimulation()">
                        &#9654; Start
                    </button>
                    <button id="pauseBtn" class="btn btn-warning" onclick="pauseSimulation()">
                        &#9208; Pause
                    </button>
                    <button id="stopBtn" class="btn btn-danger" onclick="stopSimulation()">
                        &#9209; Stop
                    </button>
                    <button id="exitBtn" class="btn btn-secondary" onclick="exitSimulation()">
                        &#10005; Exit
                    </button>
                </div>
                
                <div class="mt-3">
                    <strong>Status:</strong> <span id="status">IDLE</span>
                </div>
            </div>
        </div>
        
        <div class="card mt-4">
            <div class="card-header">
                <h3>Simulation Information</h3>
            </div>
            <div class="card-body">
                <p>Simulation: <strong>)HTML" + simName + R"HTML(</strong></p>
                <p>Framework: <strong>SIMILAR C++</strong></p>
                <p>Port: <strong>)HTML" + std::to_string(port) + R"HTML(</strong></p>
            </div>
        </div>
    </div>
    
    <script src="/js/jquery-3.6.0.min.js"></script>
    <script src="/js/bootstrap.min.js"></script>
    <script src="/js/similar-gui.js"></script>
    <script>
        // Poll for status updates
        setInterval(function() {
    $.get(
        '/state',
        function(data) { $('#status').text(new TextDecoder().decode(data)); });
        }, 1000);
    </script>
</body>
</html>)HTML";
}

void SimilarHttpServer::setupRoutes() {
  // Main page
  server->Get("/", [this](const httplib::Request &req, httplib::Response &res) {
    res.set_content(generateHtmlPage(), "text/html");
  });

  // API endpoints
  server->Get(
      "/state", [this](const httplib::Request &req, httplib::Response &res) {
        auto state = controller->handleSimulationStateRequest();
        res.set_content(std::string(state.begin(), state.end()), "text/plain");
      });

  server->Get("/start",
              [this](const httplib::Request &req, httplib::Response &res) {
                controller->handleNewSimulationRequest();
                res.set_content("OK", "text/plain");
              });

  server->Get("/stop",
              [this](const httplib::Request &req, httplib::Response &res) {
                controller->handleSimulationAbortionRequest();
                res.set_content("OK", "text/plain");
              });

  server->Get("/pause",
              [this](const httplib::Request &req, httplib::Response &res) {
                controller->handleSimulationPauseRequest();
                res.set_content("OK", "text/plain");
              });

  server->Get("/shutdown",
              [this](const httplib::Request &req, httplib::Response &res) {
                controller->handleShutDownRequest();
                res.set_content("OK", "text/plain");
              });

  server->Get("/setParameter",
              [this](const httplib::Request &req, httplib::Response &res) {
                for (const auto &param : req.params) {
                  controller->setParameter(param.first, param.second);
                }
                res.set_content("OK", "text/plain");
              });

  server->Get("/getParameter",
              [this](const httplib::Request &req, httplib::Response &res) {
                std::string result;
                for (const auto &param : req.params) {
                  result += param.first + ": " +
                            controller->getParameter(param.first) + "\n";
                }
                res.set_content(result, "text/plain");
              });

  // Static files
  server->Get(R"(/css/(.+))",
              [this](const httplib::Request &req, httplib::Response &res) {
                std::string path = "css/" + req.matches[1].str();
                std::string content = loadStaticFile(path);
                if (!content.empty()) {
                  res.set_content(content, getMimeType(path));
                } else {
                  res.status = 404;
                }
              });

  server->Get(R"(/js/(.+))",
              [this](const httplib::Request &req, httplib::Response &res) {
                std::string path = "js/" + req.matches[1].str();
                std::string content = loadStaticFile(path);
                if (!content.empty()) {
                  res.set_content(content, getMimeType(path));
                } else {
                  res.status = 404;
                }
              });

  server->Get(R"(/img/(.+))",
              [this](const httplib::Request &req, httplib::Response &res) {
                std::string path = "img/" + req.matches[1].str();
                std::string content = loadStaticFile(path);
                if (!content.empty()) {
                  res.set_content(content, getMimeType(path));
                } else {
                  res.status = 404;
                }
              });
}

void SimilarHttpServer::initServer() {
  setupRoutes();
  std::cout << "HTTP server initialized on port " << port << std::endl;
}

void SimilarHttpServer::showView() {
  // Start server in background thread
  running.store(true);
  std::thread serverThread([this]() {
    std::cout << "Starting HTTP server on http://localhost:" << port
              << std::endl;
    server->listen("localhost", port);
  });
  serverThread.detach();

  // Wait a bit for server to start
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  // Open browser if requested
  if (initData->getConfig()->getAutoOpenBrowser()) {
    std::string url = "http://localhost:" + std::to_string(port);
    std::cout << "Opening browser to " << url << std::endl;

#ifdef __APPLE__
    std::string cmd = "open " + url;
    system(cmd.c_str());
#elif defined(_WIN32)
    std::string cmd = "start " + url;
    system(cmd.c_str());
#else
    std::string cmd = "xdg-open " + url;
    system(cmd.c_str());
#endif
  } else {
    std::cout << "Browse to http://localhost:" << port << std::endl;
  }
}

void SimilarHttpServer::stop() {
  if (running.load()) {
    running.store(false);
    if (server) {
      server->stop();
    }
    std::cout << "HTTP server stopped" << std::endl;
  }
}

// IHtmlControls implementation (button states managed by JavaScript)
void SimilarHttpServer::setStartButtonState(bool active) {
  // In web version, button states are managed client-side via JavaScript
}

void SimilarHttpServer::setPauseButtonState(bool active) {
  // In web version, button states are managed client-side via JavaScript
}

void SimilarHttpServer::setAbortButtonState(bool active) {
  // In web version, button states are managed client-side via JavaScript
}

void SimilarHttpServer::shutDownView() { stop(); }

} // namespace view
} // namespace web
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
