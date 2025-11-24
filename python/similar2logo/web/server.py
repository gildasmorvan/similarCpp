"""
FastAPI web server for real-time Logo simulation visualization and control.
"""

from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.staticfiles import StaticFiles
from fastapi.responses import HTMLResponse
import asyncio
import time
import json
from typing import Optional
import threading


class WebSimulation:
    """
    Web-enabled Logo simulation with real-time visualization.
    
    Args:
        simulation: LogoSimulation instance
        update_rate: Updates per second (default: 30)
    
    Examples:
        >>> from similar2logo import LogoSimulation, Environment
        >>> from similar2logo.web import WebSimulation
        >>> 
        >>> env = Environment(100, 100, toroidal=True)
        >>> sim = LogoSimulation(env, num_turtles=50)
        >>> web_sim = WebSimulation(sim)
        >>> web_sim.start_server(port=8080)
    """
    
    def __init__(self, simulation, update_rate=30, initial_speed=10.0):
        self.simulation = simulation
        self.update_rate = update_rate
        self.running = False
        self.paused = False
        self.sim_thread = None
        self.connected_clients = []
        self.speed_factor = initial_speed
        
        # Add RealTimeMatcherProbe for speed control
        from ..probes import RealTimeMatcherProbe
        self.speed_probe = RealTimeMatcherProbe(self.speed_factor)
        self.simulation.probe_manager.add_probe("web_speed_control", self.speed_probe)
        
        # Create FastAPI app
        self.app = FastAPI(title="SIMILAR2Logo Web Interface")
        self._setup_routes()
    
    def _setup_routes(self):
        """Setup API routes."""
        
        @self.app.get("/")
        async def root():
            return HTMLResponse(self._get_html())
        
        @self.app.get("/api/state")
        async def get_state():
            return self.simulation.get_state()
        
        @self.app.post("/api/start")
        async def start():
            self.start()
            return {"status": "started"}
        
        @self.app.post("/api/pause")
        async def pause():
            self.pause()
            return {"status": "paused"}
        
        @self.app.post("/api/resume")
        async def resume():
            self.resume()
            return {"status": "resumed"}
        
        @self.app.post("/api/stop")
        async def stop():
            self.stop()
            return {"status": "stopped"}
        
        @self.app.post("/api/reset")
        async def reset():
            self.simulation.reset()
            return {"status": "reset"}
        
        @self.app.post("/api/speed")
        async def set_speed(data: dict):
            """Update simulation speed factor."""
            speed = data.get("speed", 1.0)
            self.speed_factor = max(0.1, min(100.0, speed))  # Clamp between 0.1 and 100
            self.speed_probe.acceleration_factor = self.speed_factor
            return {"status": "speed_updated", "speed": self.speed_factor}
        
        @self.app.get("/api/speed")
        async def get_speed():
            """Get current speed factor."""
            return {"speed": self.speed_factor}
        
        @self.app.post("/api/parameters")
        async def set_parameters(data: dict):
            """Update simulation parameters."""
            new_params = data.get("parameters", {})
            updated = {}
            
            print(f"Received parameter update request: {new_params}")
            
            count = 0
            for turtle in self.simulation.turtles:
                count += 1
                # Update direct attributes
                for key, value in new_params.items():
                    # Try setting on turtle directly
                    if hasattr(turtle, key):
                         try:
                             setattr(turtle, key, value)
                             updated[key] = value
                         except Exception:
                             pass
                    
                    # Try setting on turtle.params
                    if hasattr(turtle, 'params') and hasattr(turtle.params, key):
                        try:
                            setattr(turtle.params, key, value)
                            updated[key] = value
                        except Exception:
                            pass
            
            # Also update environment params if they exist
            if hasattr(self.simulation.environment, 'params'):
                for key, value in new_params.items():
                    if hasattr(self.simulation.environment.params, key):
                        try:
                            setattr(self.simulation.environment.params, key, value)
                            updated[key] = value
                        except Exception:
                            pass
            
            print(f"Updated parameters for {count} agents: {updated}")
            return {"status": "parameters_updated", "updated": updated}
        
        @self.app.get("/api/parameters")
        async def get_parameters():
            """Get available parameters from first turtle."""
            if not self.simulation.turtles:
                return {"parameters": {}}
            
            turtle = self.simulation.turtles[0]
            params = {}
            
            # 1. Check for 'params' object (common pattern in our DSL examples)
            if hasattr(turtle, 'params') and turtle.params:
                p = turtle.params
                # If it's a dataclass
                if hasattr(p, '__dataclass_fields__'):
                    for field_name in p.__dataclass_fields__:
                        val = getattr(p, field_name)
                        if isinstance(val, (int, float)):
                            params[field_name] = val
                # If it's a normal object
                elif hasattr(p, '__dict__'):
                     for key, val in p.__dict__.items():
                        if not key.startswith('_') and isinstance(val, (int, float)):
                            params[key] = val

            # 2. Check for direct attributes on turtle, but avoid system ones
            system_attrs = {'position', 'heading', 'color', 'id', '_environment', '_id', 'acceleration', 'pen_down'}
            for key, val in turtle.__dict__.items():
                if key.startswith('_') or key in system_attrs:
                    continue
                if isinstance(val, (int, float)):
                    # Don't overwrite if already found in params
                    if key not in params:
                        params[key] = val
            
            # Also check for properties that might be settable (like speed)
            # But for now, __dict__ covers instance variables which is what we mostly care about
            
            # 3. Check for environment params
            if hasattr(self.simulation.environment, 'params'):
                ep = self.simulation.environment.params
                # If it's a dataclass
                if hasattr(ep, '__dataclass_fields__'):
                    for field_name in ep.__dataclass_fields__:
                        val = getattr(ep, field_name)
                        if isinstance(val, (int, float)):
                            params[field_name] = val
                # If it's a normal object
                elif hasattr(ep, '__dict__'):
                     for key, val in ep.__dict__.items():
                        if not key.startswith('_') and isinstance(val, (int, float)):
                            params[key] = val
            
            return {"parameters": params}
        
        @self.app.post("/api/agent_count")
        async def set_agent_count(data: dict):
            """Update the number of agents in the simulation."""
            target_count = data.get("count", len(self.simulation.turtles))
            current_count = len(self.simulation.turtles)
            
            if target_count < 0:
                return {"status": "error", "message": "Count must be non-negative"}
            
            # Get the turtle class from existing turtles or use default
            turtle_class = type(self.simulation.turtles[0]) if self.simulation.turtles else None
            
            if target_count > current_count:
                # Add more agents
                num_to_add = target_count - current_count
                # add_turtles now handles defaults using stored class/kwargs
                self.simulation.add_turtles(num_to_add)
                    
            elif target_count < current_count:
                # Remove agents
                num_to_remove = current_count - target_count
                # Remove from the end
                self.simulation.turtles = self.simulation.turtles[:target_count]
            
            return {
                "status": "agent_count_updated",
                "count": len(self.simulation.turtles),
                "previous": current_count
            }
        
        @self.app.get("/api/agent_count")
        async def get_agent_count():
            """Get current number of agents."""
            return {"count": len(self.simulation.turtles)}

        @self.app.get("/api/code")
        async def get_code():
            """Get the source code of the current simulation script."""
            import sys
            try:
                with open(sys.argv[0], 'r') as f:
                    code = f.read()
                return {"code": code, "filename": sys.argv[0]}
            except Exception as e:
                return {"error": str(e)}

        @self.app.post("/api/code")
        async def save_code(data: dict):
            """Save the source code."""
            import sys
            code = data.get("code")
            if not code:
                return {"error": "No code provided"}
            
            try:
                with open(sys.argv[0], 'w') as f:
                    f.write(code)
                return {"status": "saved"}
            except Exception as e:
                return {"error": str(e)}

        @self.app.post("/api/restart")
        async def restart_server():
            """Restart the server process."""
            import sys
            import os
            
            # We need to restart the process.
            # os.execv replaces the current process with a new one.
            # We use the same python interpreter and arguments.
            
            print("Restarting server...")
            # Give some time for the response to be sent
            def restart():
                time.sleep(0.5)
                os.execv(sys.executable, [sys.executable] + sys.argv)
            
            threading.Thread(target=restart).start()
            return {"status": "restarting"}
        
        @self.app.get("/api/models")
        async def get_models():
            """Get list of available example models."""
            from .models import get_available_models
            models = get_available_models()
            return {"models": models}
        
        @self.app.post("/api/load_model")
        async def load_model(data: dict):
            """Load a different model."""
            from .models import get_model_code
            filename = data.get("filename")
            
            if not filename:
                return {"error": "No filename provided"}
            
            code = get_model_code(filename)
            if code is None:
                return {"error": f"Model {filename} not found"}
            
            # Save to current script
            import sys
            try:
                with open(sys.argv[0], 'w') as f:
                    f.write(code)
                
                # Trigger restart
                def restart():
                    time.sleep(0.5)
                    import os
                    os.execv(sys.executable, [sys.executable] + sys.argv)
                
                threading.Thread(target=restart).start()
                return {"status": "loading", "filename": filename}
            except Exception as e:
                return {"error": str(e)}
        
        @self.app.websocket("/ws")
        async def websocket_endpoint(websocket: WebSocket):
            await websocket.accept()
            self.connected_clients.append(websocket)
            
            try:
                while True:
                    # Send state updates
                    if not self.paused:
                        state = self.simulation.get_state()
                        await websocket.send_json(state)
                    
                    await asyncio.sleep(1.0 / self.update_rate)
            except WebSocketDisconnect:
                self.connected_clients.remove(websocket)
    
    def _simulation_loop(self):
        """Run simulation in background thread."""
        while self.running:
            if not self.paused:
                self.simulation.step()
            time.sleep(1.0 / self.update_rate)
    
    def start(self):
        """Start the simulation."""
        if not self.running:
            self.running = True
            self.paused = False
            self.sim_thread = threading.Thread(target=self._simulation_loop, daemon=True)
            self.sim_thread.start()
    
    def pause(self):
        """Pause the simulation."""
        self.paused = True
    
    def resume(self):
        """Resume the simulation."""
        self.paused = False
    
    def stop(self):
        """Stop the simulation."""
        self.running = False
        if self.sim_thread:
            self.sim_thread.join(timeout=1.0)
    
    def start_server(self, host="0.0.0.0", port=8080):
        """
        Start the web server.
        
        Args:
            host: Host address (default: 0.0.0.0)
            port: Port number (default: 8080)
        """
        import uvicorn
        print(f"Starting SIMILAR2Logo web interface at http://localhost:{port}")
        print("Press Ctrl+C to stop")
        uvicorn.run(self.app, host=host, port=port)
    
    def _get_html(self):
        """Get the HTML for the web interface."""
        from .template import HTML_TEMPLATE
        return HTML_TEMPLATE
