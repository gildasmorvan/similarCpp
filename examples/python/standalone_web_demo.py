"""
SIMILAR2Logo Web Visualization Demo

This is a standalone demo that shows what the web interface looks like.
Since the Python bindings aren't compiled yet, this creates a mock simulation
to demonstrate the web interface functionality.

To use the real Python bindings:
1. Build C++ bindings: cd cpp/similar2logo/python/build && cmake .. && make
2. Install package: pip install -e .
3. Run actual examples: python examples/python/web_demo.py
"""

from http.server import HTTPServer, BaseHTTPRequestHandler
import json
import random
import math  # TODO: Replace with fastmath
from similar2logo.fastmath import sin, cos, atan2, sqrt, normalize_angle, PI, TWO_PI
import threading
import time


class MockSimulation:
    """Mock simulation for demonstration purposes."""
    
    def __init__(self, num_agents=100):
        self.width = 100
        self.height = 100
        self.step = 0
        self.running = False
        self.agents = []
        
        # Create agents
        for i in range(num_agents):
            self.agents.append({
                'id': i,
                'x': random.uniform(0, self.width),
                'y': random.uniform(0, self.height),
                'heading': random.uniform(0, TWO_PI),
                'speed': random.uniform(0.5, 1.5),
                'color': random.choice(['red', 'blue', 'green', 'yellow', 'purple'])
            })
    
    def update(self):
        """Update simulation state."""
        if not self.running:
            return
        
        # Simple flocking-like behavior
        for agent in self.agents:
            # Random walk
            if random.random() < 0.1:
                agent['heading'] += random.uniform(-0.3, 0.3)
            
            # Move forward
            agent['x'] += sin(agent['heading']) * agent['speed']
            agent['y'] -= cos(agent['heading']) * agent['speed']
            
            # Wrap around (toroidal)
            agent['x'] = agent['x'] % self.width
            agent['y'] = agent['y'] % self.height
        
        self.step += 1
    
    def get_state(self):
        """Get current state as JSON."""
        return {
            'step': self.step,
            'num_turtles': len(self.agents),
            'turtles': [
                {
                    'position': [agent['x'], agent['y']],
                    'heading': agent['heading'],
                    'color': agent['color']
                }
                for agent in self.agents
            ],
            'environment': {
                'width': self.width,
                'height': self.height
            }
        }


# Global simulation instance
sim = MockSimulation(num_agents=100)
sim_lock = threading.Lock()


class WebHandler(BaseHTTPRequestHandler):
    """HTTP request handler for the web interface."""
    
    def do_GET(self):
        """Handle GET requests."""
        if self.path == '/':
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write(get_html().encode())
        
        elif self.path == '/api/state':
            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            with sim_lock:
                state = sim.get_state()
            self.wfile.write(json.dumps(state).encode())
        
        else:
            self.send_response(404)
            self.end_headers()
    
    def do_POST(self):
        """Handle POST requests."""
        global sim
        
        if self.path == '/api/start':
            with sim_lock:
                sim.running = True
            self.send_json_response({'status': 'started'})
        
        elif self.path == '/api/pause':
            with sim_lock:
                sim.running = False
            self.send_json_response({'status': 'paused'})
        
        elif self.path == '/api/reset':
            sim = MockSimulation(num_agents=100)
            self.send_json_response({'status': 'reset'})
        
        else:
            self.send_response(404)
            self.end_headers()
    
    def send_json_response(self, data):
        """Send JSON response."""
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()
        self.wfile.write(json.dumps(data).encode())
    
    def log_message(self, format, *args):
        """Suppress default logging."""
        pass


def get_html():
    """Get the HTML for the web interface."""
    return """
<!DOCTYPE html>
<html>
<head>
    <title>SIMILAR2Logo Demo - Web Visualization</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            max-width: 1400px;
            margin: 0 auto;
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            overflow: hidden;
        }
        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 30px;
            text-align: center;
        }
        .header h1 {
            margin: 0;
            font-size: 2.5em;
            font-weight: 300;
        }
        .header p {
            margin: 10px 0 0 0;
            opacity: 0.9;
        }
        .content {
            display: grid;
            grid-template-columns: 1fr 300px;
            gap: 20px;
            padding: 20px;
        }
        .canvas-container {
            background: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
            box-shadow: inset 0 2px 10px rgba(0,0,0,0.1);
        }
        canvas {
            border: 2px solid #dee2e6;
            border-radius: 8px;
            background: white;
            display: block;
            width: 100%;
            height: auto;
            cursor: crosshair;
        }
        .controls {
            background: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
        }
        .control-group {
            margin-bottom: 20px;
        }
        .control-group h3 {
            margin: 0 0 15px 0;
            color: #495057;
            font-size: 1.1em;
        }
        button {
            width: 100%;
            padding: 12px;
            margin: 5px 0;
            border: none;
            border-radius: 8px;
            font-size: 1em;
            cursor: pointer;
            transition: all 0.3s;
            font-weight: 500;
        }
        button:hover {
            transform: translateY(-2px);
            box-shadow: 0 4px 12px rgba(0,0,0,0.15);
        }
        button:active {
            transform: translateY(0);
        }
        .btn-start {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
        }
        .btn-pause {
            background: #ffc107;
            color: #000;
        }
        .btn-reset {
            background: #6c757d;
            color: white;
        }
        .stats {
            background: white;
            border-radius: 8px;
            padding: 15px;
            margin-top: 10px;
        }
        .stat-item {
            display: flex;
            justify-content: space-between;
            padding: 8px 0;
            border-bottom: 1px solid #e9ecef;
        }
        .stat-item:last-child {
            border-bottom: none;
        }
        .stat-label {
            color: #6c757d;
            font-weight: 500;
        }
        .stat-value {
            color: #495057;
            font-weight: 600;
        }
        .info {
            background: #e7f3ff;
            border-left: 4px solid #2196F3;
            padding: 15px;
            margin-top: 20px;
            border-radius: 4px;
        }
        .info h4 {
            margin: 0 0 10px 0;
            color: #1976D2;
        }
        .info p {
            margin: 5px 0;
            color: #555;
            font-size: 0.9em;
            line-height: 1.5;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🐢 SIMILAR2Logo Web Visualization</h1>
            <p>Real-time Agent-Based Modeling Demo</p>
        </div>
        <div class="content">
            <div class="canvas-container">
                <canvas id="canvas" width="800" height="800"></canvas>
            </div>
            <div class="controls">
                <div class="control-group">
                    <h3>Simulation Control</h3>
                    <button class="btn-start" onclick="startSim()">▶ Start</button>
                    <button class="btn-pause" onclick="pauseSim()">⏸ Pause</button>
                    <button class="btn-reset" onclick="resetSim()">🔄 Reset</button>
                </div>
                <div class="control-group">
                    <h3>Statistics</h3>
                    <div class="stats">
                        <div class="stat-item">
                            <span class="stat-label">Step:</span>
                            <span class="stat-value" id="step">0</span>
                        </div>
                        <div class="stat-item">
                            <span class="stat-label">Agents:</span>
                            <span class="stat-value" id="turtles">0</span>
                        </div>
                        <div class="stat-item">
                            <span class="stat-label">FPS:</span>
                            <span class="stat-value" id="fps">0</span>
                        </div>
                    </div>
                </div>
                <div class="info">
                    <h4>ℹ️ Demo Mode</h4>
                    <p>This is a standalone demo showing the web interface.</p>
                    <p><strong>To use real Python examples:</strong></p>
                    <p>1. Build C++ bindings<br>
                       2. Install package<br>
                       3. Run: <code>python web_demo.py</code></p>
                </div>
            </div>
        </div>
    </div>

    <script>
        const canvas = document.getElementById('canvas');
        const ctx = canvas.getContext('2d');
        
        let animationId = null;
        let lastFrameTime = Date.now();
        let frameCount = 0;
        let fps = 0;
        
        async function fetchState() {
            try {
                const response = await fetch('/api/state');
                return await response.json();
            } catch (error) {
                console.error('Error fetching state:', error);
                return null;
            }
        }
        
        function render(state) {
            if (!state) return;
            
            // Clear canvas
            ctx.fillStyle = '#ffffff';
            ctx.fillRect(0, 0, canvas.width, canvas.height);
            
            // Draw grid
            ctx.strokeStyle = '#e9ecef';
            ctx.lineWidth = 1;
            const gridSize = 50;
            for (let x = 0; x < canvas.width; x += gridSize) {
                ctx.beginPath();
                ctx.moveTo(x, 0);
                ctx.lineTo(x, canvas.height);
                ctx.stroke();
            }
            for (let y = 0; y < canvas.height; y += gridSize) {
                ctx.beginPath();
                ctx.moveTo(0, y);
                ctx.lineTo(canvas.width, y);
                ctx.stroke();
            }
            
            // Draw agents
            const scaleX = canvas.width / state.environment.width;
            const scaleY = canvas.height / state.environment.height;
            
            state.turtles.forEach(turtle => {
                const x = turtle.position[0] * scaleX;
                const y = turtle.position[1] * scaleY;
                
                // Draw agent body
                ctx.fillStyle = turtle.color;
                ctx.beginPath();
                ctx.arc(x, y, 5, 0, 2 * Math.PI);
                ctx.fill();
                
                // Draw heading indicator
                const headingLength = 10;
                const dx = Math.sin(turtle.heading) * headingLength;
                const dy = -Math.cos(turtle.heading) * headingLength;
                
                ctx.strokeStyle = turtle.color;
                ctx.lineWidth = 2;
                ctx.beginPath();
                ctx.moveTo(x, y);
                ctx.lineTo(x + dx, y + dy);
                ctx.stroke();
            });
            
            // Update FPS
            frameCount++;
            const now = Date.now();
            if (now - lastFrameTime >= 1000) {
                fps = frameCount;
                frameCount = 0;
                lastFrameTime = now;
            }
        }
        
        function updateStats(state) {
            if (!state) return;
            document.getElementById('step').textContent = state.step;
            document.getElementById('turtles').textContent = state.num_turtles;
            document.getElementById('fps').textContent = fps;
        }
        
        async function animate() {
            const state = await fetchState();
            render(state);
            updateStats(state);
            animationId = requestAnimationFrame(animate);
        }
        
        async function startSim() {
            await fetch('/api/start', { method: 'POST' });
            if (!animationId) {
                animate();
            }
        }
        
        async function pauseSim() {
            await fetch('/api/pause', { method: 'POST' });
        }
        
        async function resetSim() {
            await fetch('/api/reset', { method: 'POST' });
        }
        
        // Start animation loop
        animate();
    </script>
</body>
</html>
    """


def simulation_loop():
    """Background thread to update simulation."""
    while True:
        with sim_lock:
            sim.update()
        time.sleep(1/30)  # 30 FPS


def main():
    """Main entry point."""
    print("=" * 70)
    print(" " * 15 + "🐢 SIMILAR2Logo Web Demo 🐢")
    print("=" * 70)
    print()
    print("Starting web server...")
    print()
    print("📍 Open your browser and navigate to:")
    print("   http://localhost:8080")
    print()
    print("🎮 Controls:")
    print("   - Click 'Start' to begin the simulation")
    print("   - Click 'Pause' to pause")
    print("   - Click 'Reset' to restart with new agents")
    print()
    print("ℹ️  This is a demo using mock simulation.")
    print("   For real Python examples, build the bindings first.")
    print()
    print("Press Ctrl+C to stop the server")
    print("=" * 70)
    print()
    
    # Start simulation thread
    sim_thread = threading.Thread(target=simulation_loop, daemon=True)
    sim_thread.start()
    
    # Start web server
    server = HTTPServer(('localhost', 8080), WebHandler)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\n\n👋 Shutting down...")
        server.shutdown()


if __name__ == "__main__":
    main()
