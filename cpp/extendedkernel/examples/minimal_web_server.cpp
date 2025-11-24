/**
 * SIMILAR C++ - Minimal Web Server Demo
 *
 * This example demonstrates a minimal working web server using the SIMILAR
 * web infrastructure. Access at: http://localhost:8080
 */

#include "third_party/httplib.h"
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

int main() {
  std::cout << "╔══════════════════════════════════════════════════════╗"
            << std::endl;
  std::cout << "║   SIMILAR C++ - Minimal Web Server Demo             ║"
            << std::endl;
  std::cout << "╚══════════════════════════════════════════════════════╝"
            << std::endl;
  std::cout << std::endl;

  const int PORT = 8080;

  // Create HTTP server
  httplib::Server server;

  // Setup routes
  server.Get("/", [](const httplib::Request &req, httplib::Response &res) {
    std::string html = R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SIMILAR C++ - Predator-Prey Simulation</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
        }
        .container {
            background: white;
            border-radius: 15px;
            padding: 30px;
            box-shadow: 0 10px 40px rgba(0,0,0,0.2);
        }
        h1 {
            color: #667eea;
            text-align: center;
            margin-bottom: 10px;
        }
        .subtitle {
            text-align: center;
            color: #666;
            margin-bottom: 30px;
        }
        .card {
            background: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
            margin: 20px 0;
            border-left: 4px solid #667eea;
        }
        .card h2 {
            color: #333;
            margin-top: 0;
        }
        .btn-group {
            display: flex;
            gap: 10px;
            margin: 20px 0;
        }
        button {
            padding: 12px 24px;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            cursor: pointer;
            transition: all 0.3s;
            font-weight: 600;
        }
        .btn-success {
            background: #28a745;
            color: white;
        }
        .btn-success:hover {
            background: #218838;
            transform: translateY(-2px);
            box-shadow: 0 4px 12px rgba(40, 167, 69, 0.4);
        }
        .btn-warning {
            background: #ffc107;
            color: #333;
        }
        .btn-warning:hover {
            background: #e0a800;
            transform: translateY(-2px);
            box-shadow: 0 4px 12px rgba(255, 193, 7, 0.4);
        }
        .btn-danger {
            background: #dc3545;
            color: white;
        }
        .btn-danger:hover {
            background: #c82333;
            transform: translateY(-2px);
            box-shadow: 0 4px 12px rgba(220, 53, 69, 0.4);
        }
        .status {
            display: inline-block;
            padding: 8px 16px;
            border-radius: 20px;
            background: #28a745;
            color: white;
            font-weight: bold;
            margin-left: 10px;
        }
        .info-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin-top: 15px;
        }
        .info-item {
            background: white;
            padding: 15px;
            border-radius: 8px;
            border: 1px solid #dee2e6;
        }
        .info-label {
            color: #666;
            font-size: 14px;
            margin-bottom: 5px;
        }
        .info-value {
            color: #333;
            font-size: 20px;
            font-weight: bold;
        }
        .feature-list {
            list-style: none;
            padding: 0;
        }
        .feature-list li {
            padding: 8px 0;
            padding-left: 30px;
            position: relative;
        }
        .feature-list li:before {
            content: "✓";
            position: absolute;
            left: 0;
            color: #28a745;
            font-weight: bold;
            font-size: 18px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🦁 SIMILAR C++ - Predator-Prey Ecosystem</h1>
        <p class="subtitle">Multi-Agent Simulation Framework</p>
        
        <div class="card">
            <h2>🎮 Simulation Controls</h2>
            <div class="btn-group">
                <button class="btn-success" onclick="startSimulation()">
                    ▶ Start Simulation
                </button>
                <button class="btn-warning" onclick="pauseSimulation()">
                    ⏸ Pause
                </button>
                <button class="btn-danger" onclick="stopSimulation()">
                    ⏹ Stop
                </button>
            </div>
            <p><strong>Status:</strong><span class="status" id="status">READY</span></p>
        </div>
        
        <div class="card">
            <h2>📊 Simulation Statistics</h2>
            <div class="info-grid">
                <div class="info-item">
                    <div class="info-label">Prey Population</div>
                    <div class="info-value" id="prey-count">5</div>
                </div>
                <div class="info-item">
                    <div class="info-label">Predator Population</div>
                    <div class="info-value" id="predator-count">2</div>
                </div>
                <div class="info-item">
                    <div class="info-label">Time Steps</div>
                    <div class="info-value" id="time-steps">0</div>
                </div>
                <div class="info-item">
                    <div class="info-label">Avg Energy</div>
                    <div class="info-value" id="avg-energy">65</div>
                </div>
            </div>
        </div>
        
        <div class="card">
            <h2>🌟 Framework Features</h2>
            <ul class="feature-list">
                <li>Multi-level agent architecture</li>
                <li>Perception-Decision-Revision cycle</li>
                <li>Dynamic agent populations</li>
                <li>Spatial environment support</li>
                <li>Energy-based lifecycle</li>
                <li>Real-time web monitoring</li>
                <li>Thread-safe execution</li>
                <li>Cross-platform compatibility</li>
            </ul>
        </div>
        
        <div class="card">
            <h2>ℹ️ Server Information</h2>
            <p><strong>Framework:</strong> SIMILAR C++</p>
            <p><strong>Port:</strong> 8080</p>
            <p><strong>Server Status:</strong> <span style="color: #28a745;">● Online</span></p>
        </div>
    </div>
    
    <script>
        function startSimulation() {
            fetch('/start')
                .then(() => {
                    document.getElementById('status').textContent = 'RUNNING';
                    document.getElementById('status').style.background = '#28a745';
                });
        }
        
        function pauseSimulation() {
            fetch('/pause')
                .then(() => {
                    document.getElementById('status').textContent = 'PAUSED';
                    document.getElementById('status').style.background = '#ffc107';
                    document.getElementById('status').style.color = '#333';
                });
        }
        
        function stopSimulation() {
            fetch('/stop')
                .then(() => {
                    document.getElementById('status').textContent = 'STOPPED';
                    document.getElementById('status').style.background = '#dc3545';
                });
        }
        
        // Poll for status updates
        setInterval(() => {
            fetch('/state')
                .then(response => response.text())
                .then(data => {
                    if (data) {
                        document.getElementById('status').textContent = data;
                    }
                });
        }, 1000);
    </script>
</body>
</html>)HTML";

    res.set_content(html, "text/html");
  });

  // API endpoints
  server.Get("/state", [](const httplib::Request &req, httplib::Response &res) {
    res.set_content("READY", "text/plain");
  });

  server.Get("/start", [](const httplib::Request &req, httplib::Response &res) {
    std::cout << "🚀 Simulation started" << std::endl;
    res.set_content("OK", "text/plain");
  });

  server.Get("/pause", [](const httplib::Request &req, httplib::Response &res) {
    std::cout << "⏸ Simulation paused" << std::endl;
    res.set_content("OK", "text/plain");
  });

  server.Get("/stop", [](const httplib::Request &req, httplib::Response &res) {
    std::cout << "⏹ Simulation stopped" << std::endl;
    res.set_content("OK", "text/plain");
  });

  // Start server
  std::cout << "🌐 Starting web server..." << std::endl;
  std::cout << "📡 Server running at: http://localhost:" << PORT << std::endl;
  std::cout << "🌍 Open your browser and navigate to the URL above"
            << std::endl;
  std::cout << "⏹  Press Ctrl+C to stop the server" << std::endl;
  std::cout << std::endl;

  // Auto-open browser on macOS
#ifdef __APPLE__
  std::string url = "http://localhost:" + std::to_string(PORT);
  std::string cmd = "open " + url;
  std::cout << "🚀 Opening browser..." << std::endl;
  system(cmd.c_str());
#endif

  // Start listening (blocking call)
  server.listen("localhost", PORT);

  return 0;
}
